#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

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
    dir->mbrs = NULL;
    dir->tam = 0;
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

int inicia_diretorio(char *archive)
{
    struct diretorio *dir = malloc(sizeof(struct diretorio));
    if (dir == NULL)
        falta_de_memoria();
    dir->archive = fopen(archive, "r+");
    if (dir->archive == NULL)
        cria_novo_dir(dir, archive);
    else
        ler_archive(dir);

    return 0;
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
    if (!ok)
        return 1;
    struct membro *mem = malloc(sizeof(struct membro));
    if (mem == NULL)
        falta_de_memoria();

    pega_props(nome, mem, propriedades, ftell(dir->archive));
    
    dir->mbrs[dir->tam] = mem;

    return 0;
}

int main()
{
    /* add_membro("makefile"); */

    return 0;
}
