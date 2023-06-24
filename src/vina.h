#ifndef _H_VINA
#define _H_VINA

#include <stdio.h>
#include <pwd.h>
#include <utime.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <grp.h>

#define TAM_BUFFER 1024
#define QNT_MBRS 8
#define TRUE 1
#define FALSE 0
#define ATUALIZA 0
#define SUBSTITUI 1
#define P_ESQUERDA -1
#define P_DIREITA 1

struct membro {
    char *nome;
    unsigned int tam_nome;
    unsigned long comeco_dados;
    unsigned long posicao;
    uid_t uid;
    gid_t gid;
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

/* Le ou cria um archive. Retorna um ponteiro para struct diretorio. */
struct diretorio *inicia_diretorio(char *archive);

/* Adiciona um membro ao archive. Se o tipo for SUBSTITUI vai substituir o 
* membro caso ele exista. Se for ATUALIZA, so ira substituir o membro caso o 
* novo seja mais recente. */
int add_membro(char *nome, struct diretorio *dir, short tipo);

/* Escreve o archive. */
int monta_archive(struct diretorio *dir);

/* Imprime os metadados de todos os membros. */
void mostra_propriedades(struct diretorio *dir);

/* Extrai o membro pelo nome. Caso o membro nao exista retorna 1. */
int extrai_membro(struct diretorio *dir, char *nome_mbr);

/* Remove o membro pelo nome. Se o membro nao existir retorna 1. */
int remove_membro(struct diretorio *dir, char *nome_mbr);

/* Reescreve o archive (atualiza). */
void remonta_archive(struct diretorio *dir);

/* Move o @mbr_n para imediatamente atras do @target. 
* Retorna 0 se tudo der certo, -1 se @target for igual a @mbr_n,
* 1 caso @target nao existe e 2 caso @mbr_n nao exista. */
int move_membros(struct diretorio *dir, char *target, char *mbr_n);

/* Desaloca toda memoria da struct diretorio. */
void libera_diretorio(struct diretorio *dir);

#endif // _H_VINA
