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
    // Armazena a table a de tradução
    std::map<std::string, translation_entry> translation_table;
    
    void read_table(std::string path);

    public:
    // Realiza a tradução da estrutura fornecida para a linguagem assembly IA32
    void translate(std::vector<asm_line>&, std::string);
    
    Translator(std::string path, bool verbose = false) : verbose(verbose) { read_table(path); };

    void print_table();
};

#endif