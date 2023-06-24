#include "vina.h"

/* Verifica se a entrada esta correta. Retorna 1 caso sucesso e 
 * 0 caso contrario.*/
int entrada_correta(int argc, char *argv[], int opt);

/* Insere os arquivos passados no prompt. Se o arquivo ja existir, 
 * a inserção pode ser uma atualização ou uma substituição.*/
void insere_arquivos(int argc, char *argv[], short tipo);

/* Extrai os arquivos passados no prompt, se nenhum for informado 
 * irá extrair todos. */
void extrai_arquivos(int argc, char *argv[]);

/* Mostra os metadados de todos os membros do archive. */
void mostra_metadados(char *archive);

/* Remove os arquivos do archive informados no prompt. */
void apaga_arquivos(int argc, char *argv[]);

/* Troca a posicao de um membro para imediatamente atras de outro membro
* denominado target. */
void reposiciona_membro(int argc, char *argv[]);

/* Imprime uma mensagem de ajuda sobre o funcionamento do programa. */
void imprime_ajuda();
