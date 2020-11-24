Nome do aluno: Guilherme Mendel de Almeida Nascimento
Matrícula: 170143970

O aluno optou por implementar a solução de ERROS.

É importantíssimo ressaltar que o modo de pré-processamento LEVANTA ALGUNS ERROS, pois
alguns erros no arquivo fonte podem impedir ou tornar ambíguo o pré-processamenteo (sobretudo erros de rótulos)

########## EXECUÇÃO ##########

Para compilar, basta executar no terminal:
g++ assembler.cpp src/*.cpp -o montador

Em seguida, para pré-processar:
./montador <arquivo.asm> -p

E para montar:
./montador <arquivo.pre> -o

Existem 2 parâmetros opcionais: --print e --verbose, que mostram detalhes do processamento, em cada uma das etapas.
O print mostra a estrutura do programa como lida pelo módulo scanner, e o verbose mostra alguns detalhes específicos.

########## MÓDULOS ##########
O arquivo assembler.cpp simplesmente recebe as instruções do usuário e invoca OU o módulo de montagem (two_pass.cpp) OU o módulo de préprocessamento (preprocesser.cpp)

>> scanner.cpp
Lê o arquivo especificado pelo usuário e o transforma numa estrutura de vetor de structs (cada struct representa uma linha), que será utilizado pelos outros módulos.
Pode retornar alguns erros para os módulos levantarem.

>> operation_supplier.cpp
Lê o arquivo instructions.txt e descobre as instruções do assembler.
Tem a definição e o corpo de execução das diretivas.
Fornece os outros módulos com as operações (instruções ou diretivas) que eles necessitam.

>> preprocesser.cpp
Utiliza o scanner.cpp para ler o arquivo, utiliza o operation_supplier.cpp para receber quais são as diretivas que deve executar e como executá-las.
Passa por cada linha da estrutura do programa, executa as diretivas que conhece (e substitui ocorrências dos sinônimos já definidos pelo EQU).
Gera um arquivo .pre

>> two_pass.cpp
Utiliza o scanner.cpp para ler o arquivo, utiliza o operation_supplier.cpp para receber quais são as diretivas que deve executar e como executá-las e as instruções, seus opcodes e tamanhos.
Passa por cada linha da estrutura do programa, executa as diretivas que conhece e traduz as instruções encontradas.
Pode levantar uma série de erros.
Gera um arquivo .obj
NOTA: A primeira e a segunda passagem foram implementadas com foco em eficiência, portanto a primeira passagem realiza algumas tarefas a mais que lhe foram convenientes.

>> mounter_exception.hpp
Define um tipo de exceção que é utilizado pelos outros módulos para registrar erros do arquivo lido.
Define a linha, tipo e mensagem de cada erro.
O scanner.cpp define um tipo ainda mais específico de exceção, o ScannerException, que somente ele utiliza para conseguir passar adiante as linhas que tiverem erros.