#ifndef _H_VINA
#define _H_VINA

#include <stdio.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
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
    unsigned int tam_nome;
    unsigned long comeco_dados;
    unsigned long posicao;
    uid_t uid;
    off_t tam;
    mode_t permissoes;
    time_t ult_mod;
};

struct diretorio {
    char *archive;
    unsigned long tam;
    unsigned long tam_max;
    unsigned long prox_posi;
    struct membro **mbrs;
};

struct diretorio *inicia_diretorio(char *archive);

int add_membro(char *nome, struct diretorio *dir);

int monta_archive(struct diretorio *dir);

void mostra_propriedades(struct diretorio *dir);

void extrai_todos_membros(struct diretorio *dir);

int extrai_membro(struct diretorio *dir, char *nome_mbr);

#endif // _H_VINA
