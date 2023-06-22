#include "trata_entrada.h"

void insere_arquivos(int argc, char *argv[], short tipo)
{
    struct diretorio *dir = inicia_diretorio(argv[2]);
    for (int i = 3; i < argc; i++) {
        if (add_membro(argv[i], dir, tipo))
            fprintf(stderr, "Nao foi possivel ler %s\n", argv[i]);
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
                fprintf(stderr, "Nao foi encontrado %s dentro do archive.\n", argv[i]);
        }
    }
}

void mostra_metadados(char *archive)
{
    struct diretorio *dir = inicia_diretorio(archive);
    if (dir->tam == 0 || archive == NULL) {
        fprintf(stderr, "Archive inexistente.\n");
        return;
    }
    mostra_propriedades(dir);
}

void apaga_arquivos(int argc, char *argv[])
{
    struct diretorio *dir = inicia_diretorio(argv[2]);
    for (int i = 3; i < argc; i++){
        if(remove_membro(dir, argv[i]))
            fprintf(stderr, "Membro %s nao existe no archive\n", argv[i]);
    }
    remonta_archive(dir);
}

void reposiciona_membro(char *argv[])
{
    struct diretorio *dir = inicia_diretorio(argv[3]);
    if (dir->tam == 0) {
        fprintf(stderr, "Archive vazio ou nao existente.\n");
        return;
    }
    int status = move_membros(dir, argv[2], argv[4]);
    if (status == 0)
        return;
    else if (status == -1)
        fprintf(stderr, "Nao eh possivel permutar um membro com ele mesmo.\n");
    else if (status == 1)
        fprintf(stderr, "Alvo %s nao existe no archive.\n", argv[2]);
    else if (status == 2)
        fprintf(stderr, "Membro %s nao existe no archive.\n", argv[4]);
}

void imprime_ajuda()
{
    printf("./vina++ -<flag> <archive> <arg1> <arg2> ...\n");
    printf("  -i: Inserir\n");
    printf("  -a: Insere/atualiza\n");
    printf("  -x: Extrai arquivos\n");
    printf("  -r: Remove arquivos\n");
    printf("  -c: Imprime membros do archive\n");
    printf("  -h: Ajuda\n");
    printf("  -m: Move membro para imediatamente depois do alvo da seguinte forma:\n");
    printf("      ./vina++ -m <alvo> <archive> <membro>\n");
}

