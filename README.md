Programação 2 - Projeto 4

# Arquivos
## src/  
 - main.c
   - Faz a interface com o usuário. Contém a função main.
 - diretorios.c  e diretorios.h   
   - Cuida da hierarquia de diretórios.   
 - trata_entrada.c  e trata_entrada.h  
   - Faz o intermédio entre a entrada e o módulo vina, garantindo os argumentos corretos e tratando erros.  
 - vina.c  e vina.h  
    - Módulo principal. Responsável por todo funcionamento do arquivador.  

# Estruturas
## struct membro: 
  - Guarda informações de um membro do archive, sendo elas o nome,
tamanho do nome (para facilitar a leitura após o archive ser escrito), tamanho
do arquivo em bytes, sua posição de ordem, posição de onde começa os dados e
alguns metadados.

## struct diretorio:
 - Guarda o nome do archive, quantidade atual e máxima de 
membros, próxima posição de escrita para um novo membro e um vetor de ponteiros
para struct membro. 

# Algoritmos 
Um algoritmo que vale comentar é o de permutação, responsável pelo funcionamento
da flag -m. Ao tentar mover os dados de um membro para imediatamente atrás de outro
membro especificado, surgem diversos conflitos relacionados ao tamanho da sessão de 
dados de cada membro, sendo muito fácil sobrescrever por acidente algum dado.
O algoritmo pensando foi permutar dois membros adjacentes até que o membro escolhido
fique atrás do membro alvo. Para isso, são escolhidos dois membros para permutação,
mbr_1 e mbr_2, sendo mbr_1 o membro com índice menor do que o mbr_2 no vetor,
mbr_1 será escrito ao final do arquivo e o mbr_2 é escrito começando na posição 
inicial do mbr_1, após o termino da escrita, o mbr_1 é reescrito começando na posição em
que o mbr_2 finalizou, o último passo é reescrever a área de diretório no final 
e truncar os bytes sobrando do arquivo.

# Conclusões
Com esse projeto foi possível compreender melhor a manipulação de arquivos e
diretórios. Além de agregar o conhecimento de novas bibliotecas padrões de C.
Outro ponto, foi a apresentação de uma nova estrutura para organizar o projeto,
separando arquivos fonte em uma pasta src, o que já aponta uma maneira mais 
inteligente para organizar projetos grandes.
