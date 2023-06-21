#include <stdio.h>
#include <getopt.h>
#include "vina.h"

void insere_arquivos(int argc, char *argv[])
{
    struct diretorio *dir = inicia_diretorio(argv[2]);
    for (int i = 3; i < argc; i++) {
        if (add_membro(argv[i], dir))
            fprintf (stderr, "Nao foi possivel ler %s\n", argv[i]);
    }
    monta_archive(dir);
}

void extrai_arquivos(int argc, char *argv[])
{
    struct diretorio *dir = inicia_diretorio(argv[2]);
    if (argc == 3) {
        extrai_todos_membros(dir);
    }
    else {
        for (int i = 3; i < argc; i++) {
            if (extrai_membro(dir, argv[i]))
                fprintf (stderr, "Nao foi encontrado %s dentro do archive.\n", argv[i]);
        }
    }
}

int main(int argc, char *argv[])
{
    int option;
    while((option = getopt(argc, argv, "ie")) != -1) {
        switch (option) {
            case 'i': insere_arquivos(argc, argv); break;
            case 'e': extrai_arquivos(argc, argv); break;
            default: printf("...\n"); break;
        }
    }

    return 0;
}
