#include <stdio.h>
#include "trata_entrada.h"
#include <getopt.h>

int main(int argc, char *argv[])
{
    int option;
    option = getopt(argc, argv, "iamxrch");
    if (option == 'h') {
        imprime_ajuda();
        return 0;
    }
    if (!entrada_correta(argc, argv, option))
        return 1;
    switch (option) {
        case 'i': insere_arquivos(argc, argv, SUBSTITUI); break;
        case 'a': insere_arquivos(argc, argv, ATUALIZA); break;
        case 'm': reposiciona_membro(argc, argv); break;
        case 'x': extrai_arquivos(argc, argv); break;
        case 'r': apaga_arquivos(argc, argv); break;
        case 'c': mostra_metadados(argv[2]); break;
        default: fprintf (stderr, "%s -h para mostrar opcoes.\n", argv[0]); break;
    }

    return 0;
}
