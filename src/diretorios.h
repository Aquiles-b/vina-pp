#include <linux/limits.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#define PERM_PADRAO_DIR 16877 

/* Monta a hierarquia de diretorios de um arquivo. Retorna o nome do 
 * arquivo.*/
char *monta_hierarquia(char *path);
