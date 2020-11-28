#include <string.h>
#include <iostream>
#include "include/preprocesser.hpp"
#include "include/translator.hpp"

#define ANY(thing) (!thing.empty())
#define HAS_OPERATION(line) ANY(line.operation)

using namespace std;


int main(int argc, char *argv[]) {
    // Descrição do uso correto
    const string help = "\
Forneça o caminho para o arquivo fonte\n\
\n\
Outras opções:\n\
\t--print: Imprime a estrutura do programa, como construída pelo módulo scanner\n\
\t--verbose: Imprime descrições detalhadas da execução\n\
";
    // Ajuda os necessitados
    if (argc == 2 && (!strcmp(argv[1], "help") || !strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))) {
        cout << "Bem vindo a este tradutor!\n" << help << endl;
        return 0;
    }

    // Garante que o uso foi correto
    if (argc < 2 || argc > 4) {
        cerr << "ERRO: Número de argumentos inválido.\n" << help << endl;
        return -1;
    }

    // Analise os parâmetros
    vector<char*> args (argv + 1, argv + argc);
    // Define se a árvore do programa deve ser impressa
    bool print = false;
    // Define se imprime descrições
    bool verbose = false;
    // Guardará o caminho do arquivo fonte
    string source_file_path = "";

    try {
        // Para cada argumento
        for (const char* carg : args) {
            // Transforma em string
            string arg = string(carg);
            
            if (arg == "--print") {
                print = true;
            }

            else if (arg == "--verbose") {
                verbose = true;
            }

            else if (arg[0] != '-') {
                if (source_file_path.empty()) source_file_path = arg;
                else throw "Argumentos inválidos.";
            }

            else {
                throw "Argumentos inválidos.";
            }
        }
        // Se não tiver um caminho nos argumentos, erro
        if (source_file_path.empty()) {
            throw "Arquivo fonte não especificado.";
        }
    }
    catch (char const* error) {
        cerr << __FILE__ << ":" << __LINE__ << "> ERRO: " << error << "\n" << help << endl;
        return -1;
    }
    catch (string error) {
        cerr << __FILE__ << ":" << __LINE__ << "> ERRO: " << error << "\n" << help << endl;
        return -1;
    }

    try {
        Preprocesser preprocesser(verbose);
        Translator translator(verbose);
        
        // translator.translate(
        //     preprocesser.preprocess(source_file_path, print)
        // );

        cout << "Estrutura do programa: {" << endl;
        for (const asm_line line : preprocesser.preprocess(source_file_path, print))
        {
            cout << "\tLinha " << line.number << ": {";
            string output = "";
            if ANY(line.label) output += "label: \"" + line.label + "\", ";
            if HAS_OPERATION(line) output += "operation: \"" + line.operation + "\", ";
            if ANY(line.operand[0]) output += "operand1: \"" + line.operand[0] + "\", ";
            if (line.offset[0] != 0) output += "offset1: \"" + to_string(line.offset[0]) + "\", ";
            if ANY(line.operand[1]) output += "operand2: \"" + line.operand[1] + "\", ";
            if (line.offset[1] != 0) output += "offset2: \"" + to_string(line.offset[1]) + "\", ";
            cout << output.substr(0, output.length() - 2) << "}" << endl;
        }
        cout << "}" << endl;
    }
    catch (exception &error) {
        cerr << __FILE__ << ":" << __LINE__ << "> ERRO:\n" << error.what() << endl;
    }

    return 0;
}
