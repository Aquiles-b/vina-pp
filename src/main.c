#include <stdio.h>
#include <getopt.h>
#include "vina.h"

void insere_arquivos(int argc, char *argv[], short tipo)
{
    struct diretorio *dir = inicia_diretorio(argv[2]);
    for (int i = 3; i < argc; i++) {
        if (add_membro(argv[i], dir, tipo))
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

void mostra_metadados(char *archive)
{
    struct diretorio *dir = inicia_diretorio(archive);
    if (dir->tam == 0)
        return;
    mostra_propriedades(dir);
}

void apaga_arquivos(char *argv[])
{
    struct diretorio *dir = inicia_diretorio(argv[2]);
    remove_membro(dir, argv[3]);
    remonta_archive(dir);
}

int main(int argc, char *argv[])
{
    int option;
    while((option = getopt(argc, argv, "iamxrch")) != -1) {
        switch (option) {
            case 'i': insere_arquivos(argc, argv, SUBSTITUI); break;
            case 'a': insere_arquivos(argc, argv, ATUALIZA); break;
            case 'm': printf("Move para depois do target...\n"); break;
            case 'x': extrai_arquivos(argc, argv); break;
            case 'r': apaga_arquivos(argv); break;
            case 'c': mostra_metadados(argv[2]); break;
            case 'h': printf("Ajuda...\n"); break;
            default: printf("...\n"); break;
        }
    }

    return 0;
}
