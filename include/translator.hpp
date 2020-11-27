#ifndef __TRANSLATOR__
#define __TRANSLATOR__

#include <vector>
#include "scanner.hpp"

class Translator {
    // Define se descrções serão impressas
    const bool verbose;

    public:
    // Realiza a tradução da estrutura fornecida para a linguagem assembly IA32
    void translate(std::vector<asm_line>&);
    
    Translator(bool verbose = false) : verbose(verbose) {};
};

#endif