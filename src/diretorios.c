#include "diretorios.h"

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
