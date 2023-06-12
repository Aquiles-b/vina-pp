#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

struct membro {
    char *nome;
    unsigned long comecoDados;
    unsigned long posicao;
    uid_t uid;
    off_t tam;
    mode_t permissoes;
    struct timespec ultimaMod;
};

struct diretorio {
    FILE *archive;
    unsigned long tam;
    struct membro *mbrs;
};

void faltaDeMemoria()
{
    fprintf(stderr, "Erro: Falta de memoria.\n");
    exit(1);
}

void criaNovoDir(struct diretorio *dir, char *arc)
{
    dir->archive = fopen(arc, "w");
    dir->mbrs = NULL;
    dir->tam = 0;
}

void lerArchive(struct diretorio *dir)
{
    unsigned long posiDir;
    fscanf(dir->archive, "%ld", &posiDir);
    fseek(dir->archive, posiDir, SEEK_SET);
    fscanf(dir->archive, "%ld", &dir->tam);
    dir->mbrs = malloc(sizeof(struct membro*) * dir->tam);

    for (unsigned long i = 0; i < dir->tam; i++) {
        fread(dir->mbrs + i, sizeof(struct membro), 1, dir->archive);
    }
}

int iniciaDiretorio(char *archive)
{
    struct diretorio *dir = malloc(sizeof(struct diretorio));
    if (dir == NULL)
        faltaDeMemoria();
    dir->archive = fopen(archive, "r+");
    if (dir->archive == NULL)
        criaNovoDir(dir, archive);
    else
        lerArchive(dir);

    return 0;
}

int adicionaMembro(char *nome, struct diretorio *dir)
{
    struct stat propriedades;
    int ok = stat(nome, &propriedades);
    if (!ok)
        return 1;
    struct membro *mem = malloc(sizeof(struct membro));
    if (mem == NULL)
        faltaDeMemoria();
    mem->nome = nome;
    mem->comecoDados = ftell(dir->archive);
    mem->uid = propriedades.st_uid;
    mem->tam = propriedades.st_size;
    mem->permissoes = propriedades.st_mode;
    mem->ultimaMod = propriedades.st_mtim;

    return 0;
}

int main()
{
    /* adicionaMembro("makefile"); */

    return 0;
}
