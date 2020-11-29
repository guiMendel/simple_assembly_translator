#ifndef __TRANSLATOR__
#define __TRANSLATOR__

#include <map>
#include <vector>
#include "scanner.hpp"

struct translation_entry {
    // Indica a seção na qual essa instrução deve ir
    std::string section;
    // Indica os parâmteros que devem ser transferidos da linha original para as linhas de IA32
    std::string params[2];
    // Define quais instruções IA32 equivalem à instrução correspondente
    std::vector<std::string> instructions;

    translation_entry(std::string section, std::string _params[2], std::vector<std::string> instructions) :
        section(section),
        instructions(instructions)
    {
        params[0] = _params[0];
        params[1] = _params[1];
    }

    translation_entry() {}
};

class Translator {
    // Define se descrções serão impressas
    const bool verbose;
    // Armazena a tabela de tradução
    std::map<std::string, translation_entry> translation_table;
    // Armaze o caminho onde se encontra o arquivo de funções auxiliares
    std::string auxiliary_functions_path;
    
    // Lê do arquivo fornecido a tabela de tradução
    void read_table(std::string path);

    // substitui os parâmetros da linha
    void parse_params(std::string&, const std::string[2], const asm_line&);

    public:
    // Realiza a tradução da estrutura fornecida para a linguagem assembly IA32
    void translate(const std::vector<asm_line>&, std::string);
    
    Translator(std::string assets_path, bool verbose = false) :
        verbose(verbose),
        auxiliary_functions_path(assets_path + "/callables.s")
    { read_table(assets_path + "/translation_table.txt"); }

    void print_table();
};

#endif