#ifndef __SUPPLIER__
#define __SUPPLIER__

#include <map>
#include <vector>
#include "scanner.hpp"
#include "preprocesser.hpp"

class OperationSupplier {
    // DIRETIVAS PRÉPROCESSAMENTO
    // Executa a diretiva EQU
    static void eval_EQU(std::vector<asm_line>::iterator&, Preprocesser*);
    // Executa a diretiva IF
    static void eval_IF(std::vector<asm_line>::iterator&, Preprocesser*);

    public:
    // Fornece as diretivas de préprocessamento e suas rotinas, como especificado no arquivo cpp
    auto supply_pre_directives() -> std::map<std::string, void(*)(std::vector<asm_line>::iterator&, Preprocesser*)>;
};

#endif