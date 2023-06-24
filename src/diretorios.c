#include "diretorios.h"

/* Monta a hierarquia de diretorios de um arquivo. Retorna o nome do 
 * arquivo.*/
char *monta_hierarquia(char *path)
{
    char *dir_atual, *dir_anter;
    dir_anter = strtok(path, "/");
    dir_atual = strtok(NULL, "/");
    while (dir_atual != NULL) {
        if(strcmp(dir_anter, "..")) {
            mkdir(dir_anter, PERM_PADRAO_DIR);
            chdir(dir_anter);
        }
        dir_anter = dir_atual;
        dir_atual = strtok(NULL, "/");
    }
    return dir_anter;
}
