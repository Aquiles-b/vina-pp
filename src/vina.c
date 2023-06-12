#include <stdio.h>
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

int main()
{

    return 0;
}
