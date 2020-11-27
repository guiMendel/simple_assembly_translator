#include "../include/operation_supplier.hpp"

using namespace std;

#define NOT_EMPTY(thing) (!thing.empty())

auto OperationSupplier::supply_pre_directives() -> map<string, void(*)(vector<asm_line>::iterator&, Preprocesser*)> {
    // Popula a tabela de diretivas de préprocessamento
    // Implementação do padrão de projeto Command
    map<string, void(*)(vector<asm_line>::iterator&, Preprocesser*)> pre_directive_table;
    
    pre_directive_table["EQU"] = &eval_EQU;
    pre_directive_table["IF"] = &eval_IF;
    
    return pre_directive_table;
}

// DIRETIVAS DE PREPROCESSAMENTO

void OperationSupplier::eval_EQU(vector<asm_line>::iterator& line_iterator, Preprocesser *pre_instance) {
    const asm_line line = *line_iterator;
    bool verbose = pre_instance->is_verbose();
    map<string, int> &synonym_table = pre_instance->get_synonym_table();

    // Descobre o valor da definição
    int value;
    try {
        value = stoi(line.operand[0]);
    }
    // Se o operando for outro rótulo, stoi() lançará uma exceção
    catch (...) {
        // Verifica se é que havia um operando
        if (line.operand[0].empty()) {
            const MounterException error (-1, "sintático",
                "A diretiva EQU recebe exatamente um parâmetro"
            );
            throw error;
        }
        // Aponta erro, não deveria receber um rótulo
        string att;
        for(auto it = synonym_table.cbegin(); it != synonym_table.cend(); ++it) {
            att += it->first + ": " + to_string(it->second) + "\n";
        }
        att = (att == "" ? "Nenhuma registrada" : att.substr(0, att.length()-1));

        const MounterException error (-1, "semântico",
            "Rótulo \"" + line.operand[0] + "\" não foi atribuído por um EQU antes de ser utilizado por diretiva de pré-processamento.\nAtribuições:\n" + att
        );
        throw error;
    }
    if (verbose) {
        cout << "[" << __FILE__ << "]> Encontrado EQU. Definindo o rótulo \"" + line.label + "\" como " << value << "...";
    }
    // Verifica por rótulos repetidos
    if (synonym_table.find(line.label) != synonym_table.end()) {
        throw MounterException(line.number, "semântico",
            string("Redefinição do rótulo \"" + line.label + "\"")
        );
    }
    synonym_table[line.label] = value;
    if (verbose) cout << "OK" << endl;
}

void OperationSupplier::eval_IF(vector<asm_line>::iterator& line_iterator, Preprocesser *pre_instance) {
    asm_line &line = *line_iterator;
    bool verbose = pre_instance->is_verbose();

    // Descobre o valor do operando
    int value;
    try {
        value = stoi(line.operand[0]);
    }
    // Se o operando for outro rótulo, stoi() lançará uma exceção
    catch (...) {
        // Verifica se é que havia um operando
        if (line.operand[0].empty()) {
            const MounterException error (-1, "sintático",
                "A diretiva IF recebe exatamente um parâmetro"
            );
            throw error;
        }

        // Aponta erro, não deveria receber um rótulo
        map<string, int> &synonym_table = pre_instance->get_synonym_table();
        string att;
        for(auto it = synonym_table.cbegin(); it != synonym_table.cend(); ++it) {
            att += it->first + ": " + to_string(it->second) + "\n";
        }
        att = (att == "" ? "Nenhuma registrada" : att.substr(0, att.length()-1));
        
        const MounterException error (-1, "semântico",
            "Rótulo \"" + line.operand[0] + "\" não foi atribuído por um EQU antes de ser utilizado por diretiva de pré-processamento.\nAtribuições:\n" + att
        );
        throw error;
    } 
    
    // Executa a regra de negócio
    if (value == 1) {
        if (verbose) cout << "[" << __FILE__ << "]> Encontrado IF avaliado verdadeiro. Mantendo próxima linha" << endl;
    }
    else {
        if (verbose) cout << "[" << __FILE__ << "]> Encontrado IF avaliado falso. Pulando próxima linha" << endl;
        line_iterator++;
    }

    // Se tiver rótulo é erro
    if NOT_EMPTY(line.label) {
        throw MounterException(line.number, "sintático"s,
            "Rótulos são proibidos para a diretiva IF"s
        );
    }
    line.label = "";
}
