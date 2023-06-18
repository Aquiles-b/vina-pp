#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#define TAM_BUFFER 1024
#define QNT_MBRS 8
#define TRUE 1
#define FALSE 0

struct membro {
    char *nome;
    unsigned long comeco_dados;
    unsigned long posicao;
    uid_t uid;
    off_t tam;
    mode_t permissoes;
    struct timespec ult_mod;
};

struct diretorio {
    FILE *archive;
    unsigned long tam;
    unsigned long prox_posi;
    struct membro **mbrs;
};

void falta_de_memoria()
{
    fprintf(stderr, "Erro: Falta de memoria.\n");
    exit(1);
}

void cria_novo_dir(struct diretorio *dir, char *arc)
{
    dir->archive = fopen(arc, "w");
    dir->mbrs = malloc(sizeof(struct membro *) * QNT_MBRS);
    dir->tam = 0;
    dir->prox_posi = 9;
}

void ler_archive(struct diretorio *dir)
{
    unsigned long posiDir;
    fscanf(dir->archive, "%ld", &posiDir);
    fseek(dir->archive, posiDir, SEEK_SET);
    fscanf(dir->archive, "%ld", &dir->tam);
    dir->mbrs = malloc(sizeof(struct membro*) * dir->tam);

    for (unsigned long i = 0; i < dir->tam; i++)
        fread(dir->mbrs[i], sizeof(struct membro), 1, dir->archive);
}

struct diretorio *inicia_diretorio(char *archive)
{
    struct diretorio *dir = malloc(sizeof(struct diretorio));
    if (dir == NULL)
        falta_de_memoria();
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
    mem->comeco_dados = tam;
    mem->uid = props.st_uid;
    mem->tam = props.st_size;
    mem->permissoes = props.st_mode;
    mem->ult_mod = props.st_mtim;
}

int add_membro(char *nome, struct diretorio *dir)
{
    struct stat propriedades;
    int ok = stat(nome, &propriedades);
    if (ok)
        return 1;
    struct membro *mem = malloc(sizeof(struct membro));
    if (mem == NULL)
        falta_de_memoria();

    pega_props(nome, mem, propriedades, ftell(dir->archive));

    dir->mbrs[dir->tam] = mem;
    dir->prox_posi += propriedades.st_size;
    dir->tam++;

    return 0;
}

unsigned long le_dados_membro(FILE *arq, unsigned char *buffer)
{
    unsigned long j = 0;
    while (j < TAM_BUFFER && !feof(arq)) {
        fread(buffer + j, sizeof(unsigned char), 1, arq);
        j++;
    }
    if (feof(arq))
        j--;

    return j;
}

int monta_archive(struct diretorio *dir)
{
    if (dir->tam == 0)
        return 1;
    FILE *arq;
    short status = TRUE;
    unsigned char *buffer_write = malloc(sizeof(unsigned char) * TAM_BUFFER);
    unsigned long posi_dir, limite_buffer, i = 0; 
    posi_dir = ftell(dir->archive);
    fwrite(&posi_dir, sizeof(unsigned long), 1, dir->archive);
    arq = fopen(dir->mbrs[i]->nome, "r");
    while (status) {
        limite_buffer = le_dados_membro(arq, buffer_write);
        fwrite(buffer_write, sizeof(unsigned char), limite_buffer, dir->archive);
        if (feof(arq)) {
            i++;
            fclose(arq);
            if (dir->tam > i)
                arq = fopen(dir->mbrs[i]->nome, "r");
            else
                status = FALSE;
        }
    }
    fclose(dir->archive);

    return 0;
}

int main()
{
    struct diretorio *dir = inicia_diretorio("archive.vpp");
    add_membro("todo.txt", dir);
    add_membro("makefile", dir);
    monta_archive(dir);

    return 0;
}
