#include "vina.h"
#include "diretorios.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void falta_memoria()
{
    fprintf(stderr, "Erro: Falta de memoria.\n");
    exit(1);
}

void cria_novo_dir(struct diretorio *dir, char *arc)
{
    dir->mbrs = malloc(sizeof(struct membro *) * QNT_MBRS);
    dir->tam = 0;
    dir->tam_max = QNT_MBRS;
    dir->prox_posi = 8;
}

void ler_archive(struct diretorio *dir, FILE *archive)
{
    unsigned long posi_dir;
    struct membro *aux;
    fread(&posi_dir, sizeof(unsigned long), 1, archive);
    fseek(archive, posi_dir, SEEK_SET);

    dir->prox_posi = posi_dir;
    fread(&dir->tam, sizeof(unsigned long), 1, archive);
    fread(&dir->tam_max, sizeof(unsigned long), 1, archive);

    dir->mbrs = malloc(sizeof(struct membro*) * dir->tam_max);

    for(unsigned long i = 0; i < dir->tam; i++) {
        aux = malloc(sizeof(struct membro));
        fread(&aux->tam_nome, sizeof(unsigned int), 1, archive);
        aux->nome = malloc(sizeof(char) * (aux->tam_nome + 1));
        fread(aux->nome, sizeof(char), aux->tam_nome, archive);
        aux->nome[aux->tam_nome] = '\0';
        fread(&aux->comeco_dados, sizeof(unsigned long), 1, archive);
        fread(&aux->posicao, sizeof(unsigned long), 1, archive);
        fread(&aux->uid, sizeof(uid_t), 1, archive);
        fread(&aux->tam, sizeof(off_t), 1, archive);
        fread(&aux->permissoes, sizeof(mode_t), 1, archive);
        fread(&aux->ult_mod, sizeof(time_t), 1, archive);
        dir->mbrs[i] = aux;
    }
    fclose(archive);
}

struct diretorio *inicia_diretorio(char *archive)
{
    struct diretorio *dir = malloc(sizeof(struct diretorio));
    if (dir == NULL)
        falta_memoria();
    FILE *arc = fopen(archive, "r");
    dir->archive = archive;
    if (arc == NULL)
        cria_novo_dir(dir, archive);
    else
        ler_archive(dir, arc);

    return dir;
}

void pega_props(char *nome, struct membro *mem, struct stat props, unsigned long tam)
{
    mem->nome = nome;
    mem->tam_nome = strlen(nome);
    mem->comeco_dados = tam;
    mem->uid = props.st_uid;
    mem->tam = props.st_size;
    mem->permissoes = props.st_mode;
    mem->ult_mod = props.st_mtim.tv_sec;
}

void aumenta_tam_dir(struct diretorio *dir)
{
    dir->tam_max += QNT_MBRS;
    dir->mbrs = realloc(dir->mbrs, sizeof(struct membro *) * dir->tam_max);
}

unsigned long membro_existe(struct diretorio *dir, char *nome)
{
    unsigned long i = 0, ind_mbr = -1;
    while (ind_mbr == -1 && i < dir->tam) {
        if (!strcmp(nome, dir->mbrs[i]->nome))
            ind_mbr = i;
        i++;
    }

    return ind_mbr;
}

int add_membro(char *nome, struct diretorio *dir, short tipo)
{
    struct stat propriedades;
    unsigned long ind_mbr;
    int ok = stat(nome, &propriedades);
    if (ok)
        return 1;
    struct membro *mem = malloc(sizeof(struct membro));
    if (mem == NULL)
        falta_memoria();

    pega_props(nome, mem, propriedades, dir->prox_posi);
    ind_mbr = membro_existe(dir, nome);
    if (ind_mbr != -1) {
        if (tipo == ATUALIZA) {
            if (dir->mbrs[ind_mbr]->ult_mod >= mem->ult_mod)
                return 0;
        }
        remove_membro(dir, mem->nome);
        remonta_archive(dir);
    }
    if (dir->tam == dir->tam_max)
        aumenta_tam_dir(dir);

    dir->mbrs[dir->tam] = mem;
    dir->prox_posi += propriedades.st_size;
    dir->tam++;
    mem->posicao = dir->tam;

    return 0;
}

unsigned long le_dados_membro(unsigned long *tam_mbr, unsigned char *buffer, FILE *arq)
{
    long status = (long) *tam_mbr - TAM_BUFFER;
    if (status >= 0) {
        fread(buffer, sizeof(unsigned char), TAM_BUFFER, arq);
        *tam_mbr -= TAM_BUFFER;
        return TAM_BUFFER;
    }
    fread(buffer, sizeof(unsigned char), *tam_mbr, arq);
    unsigned long aux = *tam_mbr;
    *tam_mbr = 0;

    return aux;
}

unsigned long int escreve_dir(struct diretorio *dir, FILE *archive)
{
    struct membro *mbr;
    fseek(archive, dir->prox_posi, SEEK_SET);

    fwrite(&dir->tam, sizeof(unsigned long), 1, archive);
    fwrite(&dir->tam_max, sizeof(unsigned long), 1, archive);
    for(unsigned long i = 0; i < dir->tam; i++) {
        mbr = dir->mbrs[i];
        fwrite(&mbr->tam_nome, sizeof(unsigned int), 1, archive);
        fprintf(archive, "%s", mbr->nome);
        fwrite(&mbr->comeco_dados, sizeof(unsigned long), 1, archive);
        fwrite(&mbr->posicao, sizeof(unsigned long), 1, archive);
        fwrite(&mbr->uid, sizeof(uid_t), 1, archive);
        fwrite(&mbr->tam, sizeof(off_t), 1, archive);
        fwrite(&mbr->permissoes, sizeof(mode_t), 1, archive);
        fwrite(&mbr->ult_mod, sizeof(time_t), 1, archive);
    }

    return ftell(archive);
}

void escreve_dados_mbrs(struct diretorio *dir, unsigned char *buffer_write, FILE *archive)
{
    unsigned long tam_mbr, limite_buffer, i = 0;
    FILE *arq = fopen(dir->mbrs[i]->nome, "r");
    short status = TRUE;
    tam_mbr = dir->mbrs[i]->tam;

    while (status) {
        limite_buffer = le_dados_membro(&tam_mbr, buffer_write, arq);
        fwrite(buffer_write, sizeof(unsigned char), limite_buffer, archive);
        if (tam_mbr == 0) {
            fclose(arq);
            if (i != dir->tam - 1) {
                i++;
                arq = fopen(dir->mbrs[i]->nome, "r");
                tam_mbr = dir->mbrs[i]->tam;
            }
            else {
                status = FALSE;
            }
        }
    }
}

int monta_archive(struct diretorio *dir)
{
    if (dir->tam == 0) {
        fprintf(stderr, "Nenhum arquivo a ser arquivado.\n");
        return 1;
    }
    unsigned char *buffer_write = malloc(sizeof(unsigned char) * TAM_BUFFER);
    if (buffer_write == NULL) {
        fprintf (stderr, "Espaco insuficiente para buffer.\n");
        return 1;
    }
    FILE *archive = fopen(dir->archive, "w");
    fwrite(&dir->prox_posi, sizeof(unsigned long), 1, archive);

    escreve_dados_mbrs(dir, buffer_write, archive);
    escreve_dir(dir, archive);
    fclose(archive);

    return 0;
}

char tipo_arquivo(int perms)
{
    switch (perms & S_IFMT) {
        case S_IFBLK: return 'b';
        case S_IFCHR: return 'c';
        case S_IFDIR: return 'd';
        case S_IFIFO: return 'p';
        case S_IFLNK: return 'l';
        case S_IFREG: return '-';
        case S_IFSOCK: return 's';
        default: return '-';
    }
}

void imprime_permissoes(struct membro *mbr)
{
    char perms[] = "-rwxrwxrwx";
    perms[0] = tipo_arquivo(mbr->permissoes);
    for (short i = 1; i < 10; i++)
        perms[i] = (mbr->permissoes & (1 << (9-i))) ? perms[i] : '-';

    printf ("%s ", perms);
}

void imprime_data(struct membro *mbr)
{
    char data_fmt[20];
    strftime(data_fmt, 20, "%Y-%m-%d %H:%M", localtime(&mbr->ult_mod));
    printf ("%s ", data_fmt);
}

void mostra_propriedades(struct diretorio *dir)
{
    struct membro *mbr;
    unsigned long i = 0;
    while (i < dir->tam) {
        mbr = dir->mbrs[i];
        printf ("%ldº: ", mbr->posicao);
        imprime_permissoes(mbr);
        printf ("%s ", getpwuid(mbr->uid)->pw_name);
        printf ("%ld ", mbr->tam);
        imprime_data(mbr);
        printf ("%s\n", mbr->nome);
        i++;
    }
}

int extrai_membro(struct diretorio *dir, char *nome_mbr)
{
    if (dir->tam == 0)
        return 1;
    FILE *arq = NULL;
    FILE *archive = fopen(dir->archive, "r");
    unsigned long tam_mbr, limite_buffer, ind_mbr;
    short status = TRUE;
    unsigned char *buffer_w = malloc(sizeof(unsigned char) * TAM_BUFFER);

    ind_mbr = membro_existe(dir, nome_mbr);
    if (ind_mbr == -1)
        return 1;

    char dir_atual[256];
    getcwd(dir_atual, sizeof(char) * 256);
    char *nome_arq = monta_hierarquia(dir->mbrs[ind_mbr]->nome);
    arq = fopen(nome_arq, "w");
    fseek(archive, dir->mbrs[ind_mbr]->comeco_dados, SEEK_SET);
    tam_mbr = dir->mbrs[ind_mbr]->tam;
    while (status) {
        limite_buffer = le_dados_membro(&tam_mbr, buffer_w, archive);
        fwrite(buffer_w, sizeof(unsigned char), limite_buffer, arq);
        if (tam_mbr == 0) {
            fclose(arq);
            status = FALSE;
        }
    }
    chdir(dir_atual);
    return 0;
}

int remove_membro(struct diretorio *dir, char *nome_mbr)
{
    long int ind_mbr;

    ind_mbr = membro_existe(dir, nome_mbr);
    if (ind_mbr == -1)
        return 1;

    for (unsigned int j = ind_mbr; j < dir->tam - 1; j++) {
        dir->mbrs[j] = dir->mbrs[j + 1];
        dir->mbrs[j + 1]->posicao--;
    }
    dir->tam--;
    dir->mbrs[dir->tam] = NULL;
    
    return 0;
}

unsigned long int desfragmenta_archive(struct diretorio *dir, FILE *archive)
{
    unsigned long limite_buffer, posi_w, posi_r, tam_mbr, i = 0;
    unsigned char *buf_w = malloc(sizeof(unsigned char) * TAM_BUFFER);
    struct membro *mbr = dir->mbrs[i];
    tam_mbr = mbr->tam;
    short status = TRUE;

    posi_w = 8;
    posi_r = mbr->comeco_dados;
    while (status) {
        fseek(archive, posi_r, SEEK_SET);
        limite_buffer = le_dados_membro(&tam_mbr, buf_w, archive);
        posi_r += limite_buffer;
        fseek(archive, posi_w, SEEK_SET);
        posi_w += limite_buffer;
        fwrite(buf_w, sizeof(unsigned char), limite_buffer, archive);
        if (tam_mbr == 0) {
            if (i != dir->tam - 1) {
                i++;
                mbr = dir->mbrs[i];
                posi_r = mbr->comeco_dados;
                tam_mbr = mbr->tam;
            }
            else {
                status = FALSE;
            }
        }
    }
    return posi_w;
}

void remonta_archive(struct diretorio *dir)
{
    unsigned long int posi_final;
    FILE *archive = fopen(dir->archive, "r+");
    if (archive == NULL) {
        fprintf (stderr, "Nao foi possivel ler %s\n", dir->archive);
        return;
    }
    dir->prox_posi = desfragmenta_archive(dir, archive);
    posi_final = escreve_dir(dir, archive);
    rewind(archive);
    fwrite(&dir->prox_posi, sizeof(unsigned long), 1, archive);
    fclose(archive);
    truncate(dir->archive, posi_final);
}

unsigned long move_dados(unsigned long posi_w, struct membro *mbr, FILE *archive)
{
    unsigned char *buf_w = malloc(sizeof(unsigned char) * TAM_BUFFER);
    unsigned long posi_r, tam_mbr, buf_tam;
    tam_mbr = mbr->tam;
    posi_r = mbr->comeco_dados;
    mbr->comeco_dados = posi_w;
    while(tam_mbr > 0) {
        fseek(archive, posi_r, SEEK_SET);
        if (tam_mbr >= TAM_BUFFER)
            buf_tam = TAM_BUFFER;
        else
            buf_tam = tam_mbr;
        fread(buf_w, sizeof(unsigned char), buf_tam, archive);
        fseek(archive, posi_w, SEEK_SET);
        fwrite(buf_w, sizeof(unsigned char), buf_tam, archive);
        posi_r += buf_tam;
        posi_w += buf_tam;
        tam_mbr -= buf_tam;
    }
    return posi_w;
}

/* Permuta o membro do indice @ind_mbr para o lado escolhido.*/
void permuta_membro(struct diretorio *dir, unsigned long ind_mbr, short lado)
{
    if (lado == P_ESQUERDA && ind_mbr == 0)
        return;
    if (lado == P_DIREITA && ind_mbr == dir->tam-1)
        return;
    FILE *archive = fopen(dir->archive, "r+");
    unsigned long posi_w, posi_final;
    struct membro *mbr_esq, *mbr_dir;
    if (lado == P_DIREITA) {
        mbr_esq = dir->mbrs[ind_mbr];
        mbr_dir = dir->mbrs[ind_mbr + 1];
        dir->mbrs[ind_mbr] = mbr_dir;
        dir->mbrs[ind_mbr + 1] = mbr_esq;
    } 
    else {
        mbr_esq = dir->mbrs[ind_mbr - 1];
        mbr_dir = dir->mbrs[ind_mbr];
        dir->mbrs[ind_mbr - 1] = mbr_dir;
        dir->mbrs[ind_mbr] = mbr_esq;
    }
    mbr_esq->posicao++;
    mbr_dir->posicao--;

    posi_w = mbr_esq->comeco_dados;
    move_dados(dir->prox_posi, mbr_esq, archive);
    posi_w = move_dados(posi_w, mbr_dir, archive);
    move_dados(posi_w, mbr_esq, archive);
    posi_final = escreve_dir(dir, archive);
    fclose(archive);
    truncate(dir->archive, posi_final);
}

/* Move o @mbr_n para imediatamente atras do @target. 
* Retorna 0 se tudo der certo, -1 se @target for igual a @mbr_n,
* 1 caso @target nao existe e 2 caso @mbr_n nao exista. */
int move_membros(struct diretorio *dir, char *target, char *mbr_n)
{
    if (target == mbr_n)
        return -1;
    struct membro *mbr_target, *mbr;
    unsigned long ind_mbr = membro_existe(dir, target);
    if (ind_mbr == -1)
        return 1;
    mbr_target = dir->mbrs[ind_mbr];
    ind_mbr = membro_existe(dir, mbr_n);
    if (ind_mbr == -1)
        return 2;

    mbr = dir->mbrs[ind_mbr];
    if (mbr->posicao > mbr_target->posicao) {
        while (mbr->posicao != mbr_target->posicao + 1) {
            permuta_membro(dir, ind_mbr, P_ESQUERDA);
            ind_mbr--;
        }
    } 
    else {
        while (mbr->posicao < mbr_target->posicao) {
            permuta_membro(dir, ind_mbr, P_DIREITA);
            ind_mbr++;
        }
    }
    return 0;
}
