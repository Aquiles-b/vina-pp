#include <stdio.h>
#include <getopt.h>
#include "vina.h"

int main(int argc, char *argv[])
{
    int option;
    printf ("%s\n", argv[1]);
    while((option = getopt(argc, argv, "ie")) != -1) {
        switch (option) {
            case 'i': printf("Inserindo...\n"); break;
            case 'e': printf("Extraindo...\n"); break;
            default: printf("...\n"); break;
        }
    }

    return 0;
}
