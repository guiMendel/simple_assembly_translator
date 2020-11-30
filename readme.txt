Nome do aluno: Guilherme Mendel de Almeida Nascimento
Matrícula: 170143970

########## EXECUÇÃO ##########

Para compilar, basta executar no terminal:
g++ main.cpp src/*.cpp -o tradutor

Em seguida, para traduzir um arquiv .s (ou .asm):
./tradutor <arquivo.s>

Existem 2 parâmetros opcionais: --print e --verbose, que mostram detalhes do processamento, em cada uma das etapas.
O print mostra a estrutura do programa como lida pelo módulo scanner, e o verbose mostra alguns detalhes específicos.

########## MÓDULOS ##########
O arquivo main.cpp simplesmente recebe as instruções do usuário e invoca o módulo de tradução

>> scanner.cpp
Lê o arquivo especificado pelo usuário e o transforma numa estrutura de vetor de structs (cada struct representa uma linha), que será utilizado pelos outros módulos.
Pode retornar alguns erros para os módulos levantarem.

>> operation_supplier.cpp
Tem a definição e o corpo de execução das diretivas de pré-processamento.
Fornece o módulo de pré-processamento com as diretivas que ele necessita.

>> preprocesser.cpp
Utiliza o scanner.cpp para ler o arquivo, utiliza o operation_supplier.cpp para receber quais são as diretivas que deve executar e como executá-las.
Passa por cada linha da estrutura do programa, executa as diretivas que conhece (e substitui ocorrências dos sinônimos já definidos pelo EQU).
Gera uma outra estrutura de dados semelhante a gerada pelo módulo scanner, mas alterada pelas diretivas.

>> translator.cpp
Lê do arquivo "translation_table.txt" (na pasta assets) e gera uma tabela de tradução, que associa aos nomes das operações do assembly inventado uma sequência de strings das instruções correspondentes em IA32.
Também lê deste mesmo arquivo como inserir os parâmetros lidos do assembly inventado nas strings traduzidas.
Em seguida, passa linha a linha da estrutura de instruções fornecida pelo pré-processador e, utilizando sua tabela de tradução interna, popula uma estrutura de vetores de strings, em que cada vetor representa uma entre as 3 seções DATA, BSS e TEXT.
Por fim, o módulo cria um novo arquivo, com o mesmo nome do arquivo fonte, mas precedido por "x86_". Neste arquivo consta o programa traduzido e as funções de entrada e saída, assim como uma pequena rotina de tratamento de overflow.

>> mounter_exception.hpp
Define um tipo de exceção que é utilizado pelos outros módulos para registrar erros do arquivo lido.
Define a linha, tipo e mensagem de cada erro.
O scanner.cpp define um tipo ainda mais específico de exceção, o ScannerException, que somente ele utiliza para conseguir passar adiante as linhas que tiverem erros.