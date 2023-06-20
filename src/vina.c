#include "vina.h"

void falta_memoria()
{
    fprintf(stderr, "Erro: Falta de memoria.\n");
    exit(1);
}

void cria_novo_dir(struct diretorio *dir, char *arc)
{
    dir->archive = fopen(arc, "w");
    dir->mbrs = malloc(sizeof(struct membro *) * QNT_MBRS);
    dir->tam = 0;
    dir->tam_max = QNT_MBRS;
    dir->prox_posi = 8;
}

void ler_archive(struct diretorio *dir)
{
    unsigned long posiDir;
    struct membro *aux;
    fread(&posiDir, sizeof(unsigned long), 1, dir->archive);
    fseek(dir->archive, posiDir, SEEK_SET);

    dir->prox_posi = posiDir;
    fread(&dir->tam, sizeof(unsigned long), 1, dir->archive);
    fread(&dir->tam_max, sizeof(unsigned long), 1, dir->archive);

    dir->mbrs = malloc(sizeof(struct membro*) * dir->tam_max);

    for(unsigned long i = 0; i < dir->tam; i++) {
        aux = malloc(sizeof(struct membro));
        fread(&aux->tam_nome, sizeof(unsigned int), 1, dir->archive);
        aux->nome = malloc(sizeof(char) * (aux->tam_nome + 1));
        fread(aux->nome, sizeof(char), aux->tam_nome, dir->archive);
        aux->nome[aux->tam_nome] = '\0';
        fread(&aux->comeco_dados, sizeof(unsigned long), 1, dir->archive);
        fread(&aux->posicao, sizeof(unsigned long), 1, dir->archive);
        fread(&aux->uid, sizeof(uid_t), 1, dir->archive);
        fread(&aux->tam, sizeof(off_t), 1, dir->archive);
        fread(&aux->permissoes, sizeof(mode_t), 1, dir->archive);
        fread(&aux->ult_mod, sizeof(time_t), 1, dir->archive);
        dir->mbrs[i] = aux;
    }
}

struct diretorio *inicia_diretorio(char *archive)
{
    struct diretorio *dir = malloc(sizeof(struct diretorio));
    if (dir == NULL)
        falta_memoria();
    dir->archive = fopen(archive, "r+");
    if (dir->archive == NULL)
        cria_novo_dir(dir, archive);
    else
        ler_archive(dir);

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

int add_membro(char *nome, struct diretorio *dir)
{
    struct stat propriedades;
    int ok = stat(nome, &propriedades);
    if (ok)
        return 1;
    struct membro *mem = malloc(sizeof(struct membro));
    if (mem == NULL)
        falta_memoria();

    pega_props(nome, mem, propriedades, dir->prox_posi);

    if (dir->tam == dir->tam_max)
        aumenta_tam_dir(dir);

    dir->mbrs[dir->tam] = mem;
    dir->prox_posi += propriedades.st_size;
    dir->tam++;

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

void escreve_dir(struct diretorio *dir)
{
    struct membro *mbr;
    fwrite(&dir->tam, sizeof(unsigned long), 1, dir->archive);
    fwrite(&dir->tam_max, sizeof(unsigned long), 1, dir->archive);
    for(unsigned long i = 0; i < dir->tam; i++) {
        mbr = dir->mbrs[i];
        fwrite(&mbr->tam_nome, sizeof(unsigned int), 1, dir->archive);
        fprintf(dir->archive, "%s", mbr->nome);
        fwrite(&mbr->comeco_dados, sizeof(unsigned long), 1, dir->archive);
        fwrite(&mbr->posicao, sizeof(unsigned long), 1, dir->archive);
        fwrite(&mbr->uid, sizeof(uid_t), 1, dir->archive);
        fwrite(&mbr->tam, sizeof(off_t), 1, dir->archive);
        fwrite(&mbr->permissoes, sizeof(mode_t), 1, dir->archive);
        fwrite(&mbr->ult_mod, sizeof(time_t), 1, dir->archive);
    }
}

void escreve_dados_mbrs(struct diretorio *dir, unsigned char *buffer_write)
{
    unsigned long tam_mbr, limite_buffer, i = 0;
    FILE *arq = fopen(dir->mbrs[i]->nome, "r");
    short status = TRUE;
    tam_mbr = dir->mbrs[i]->tam;

    while (status) {
        limite_buffer = le_dados_membro(&tam_mbr, buffer_write, arq);
        fwrite(buffer_write, sizeof(unsigned char), limite_buffer, dir->archive);
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
    fwrite(&dir->prox_posi, sizeof(unsigned long), 1, dir->archive);

    escreve_dados_mbrs(dir, buffer_write);
    escreve_dir(dir);
    fclose(dir->archive);

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
        imprime_permissoes(mbr);
        printf ("%s ", getpwuid(mbr->uid)->pw_name);
        printf ("%ld ", mbr->tam);
        imprime_data(mbr);
        printf ("%s\n", mbr->nome);
        i++;
    }
}

void extrai_membros(struct diretorio *dir)
{
    if (dir->tam == 0)
        return;
    FILE *arq;
    unsigned long tam_mbr, limite_buffer, i = 0;
    short status = TRUE;
    unsigned char *buffer_w = malloc(sizeof(unsigned char) * TAM_BUFFER);

    arq = fopen(dir->mbrs[i]->nome, "w");
    fseek(dir->archive, dir->mbrs[i]->comeco_dados, SEEK_SET);
    tam_mbr = dir->mbrs[i]->tam;
    while (status) {
        limite_buffer = le_dados_membro(&tam_mbr, buffer_w, dir->archive);
        fwrite(buffer_w, sizeof(unsigned char), limite_buffer, arq);
        if (tam_mbr == 0) {
            fclose(arq);
            if (i != dir->tam - 1) {
                i++;
                arq = fopen(dir->mbrs[i]->nome, "w");
                tam_mbr = dir->mbrs[i]->tam;
                fseek(dir->archive, dir->mbrs[i]->comeco_dados, SEEK_SET);
            }
            else {
                status = FALSE;
            }
        }
    }
}
