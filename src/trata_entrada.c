#include "trata_entrada.h"
#include "vina.h"

/* Verifica se a entrada esta correta. Retorna 1 caso sucesso e 
 * 0 caso contrario.*/
int entrada_correta(int argc, char *argv[], int opt)
{
    if (argc < 3 || opt == -1) {
        fprintf(stderr, "Numero de argumento invalido\n ./vina++ -h para mostra ajuda.\n");
        return 0;
    }

    return 1;
}

/* Insere os arquivos passados no prompt. Se o arquivo ja existir, 
 * a inserção pode ser uma atualização ou uma substituição.*/
void insere_arquivos(int argc, char *argv[], short tipo)
{
    struct diretorio *dir = inicia_diretorio(argv[2]);
    for (int i = 3; i < argc; i++) {
        if (add_membro(argv[i], dir, tipo))
            fprintf(stderr, "Nao foi possivel ler %s\n", argv[i]);
    }
    monta_archive(dir);
    libera_diretorio(dir);
}

/* Extrai os arquivos passados no prompt, se nenhum for informado 
 * irá extrair todos. */
void extrai_arquivos(int argc, char *argv[])
{
    struct diretorio *dir = inicia_diretorio(argv[2]);
    if (dir->tam == 0) {
        fprintf(stderr, "%s esta vazio ou nao existe.\n", argv[2]);
        return;
    }
    if (argc == 3) {
        for (unsigned int i = 0; i < dir->tam; i++)
            extrai_membro(dir, dir->mbrs[i]->nome);
    }
    else {
        for (int i = 3; i < argc; i++) {
            if (extrai_membro(dir, argv[i]))
                fprintf(stderr, "Nao foi encontrado %s dentro do archive.\n", argv[i]);
        }
    }
    libera_diretorio(dir);
}

/* Mostra os metadados de todos os membros do archive. */
void mostra_metadados(char *archive)
{
    struct diretorio *dir = inicia_diretorio(archive);
    if (dir->tam == 0) {
        fprintf(stderr, "Archive inexistente.\n");
        return;
    }
    mostra_propriedades(dir);
    libera_diretorio(dir);
}

/* Remove os arquivos do archive informados no prompt. */
void apaga_arquivos(int argc, char *argv[])
{
    struct diretorio *dir = inicia_diretorio(argv[2]);
    if (dir->tam == 0) {
        fprintf(stderr, "%s esta vazio ou nao existe.\n", argv[2]);
        return;
    }
    for (int i = 3; i < argc; i++){
        if(remove_membro(dir, argv[i]))
            fprintf(stderr, "Membro %s nao existe no archive\n", argv[i]);
    }
    remonta_archive(dir);
    libera_diretorio(dir);
}

/* Troca a posicao de um membro para imediatamente atras de outro membro
* denominado target. */
void reposiciona_membro(int argc, char *argv[])
{
    if (argc != 5) {
        fprintf(stderr, "Numero de argumentos invalido.\n");
        return;
    }
    struct diretorio *dir = inicia_diretorio(argv[3]);
    if (dir->tam == 0) {
        fprintf(stderr, "Archive vazio ou nao existente.\n");
        return;
    }
    int status = move_membros(dir, argv[2], argv[4]);
    libera_diretorio(dir);
    if (status == 0)
        return;
    else if (status == -1)
        fprintf(stderr, "Nao eh possivel permutar um membro com ele mesmo.\n");
    else if (status == 1)
        fprintf(stderr, "Alvo %s nao existe no archive.\n", argv[2]);
    else if (status == 2)
        fprintf(stderr, "Membro %s nao existe no archive.\n", argv[4]);
}

/* Imprime uma mensagem de ajuda sobre o funcionamento do programa. */
void imprime_ajuda()
{
    printf("./vina++ -<flag> <archive> <arg1> <arg2> ...\n");
    printf("  -i: Inserir\n");
    printf("  -a: Insere/atualiza\n");
    printf("  -x: Extrai arquivos\n");
    printf("  -r: Remove arquivos\n");
    printf("  -c: Imprime membros do archive\n"); printf("  -h: Ajuda\n");
    printf("  -m: Move membro para imediatamente depois do alvo da seguinte forma:\n");
    printf("      ./vina++ -m <alvo> <archive> <membro>\n");
}
