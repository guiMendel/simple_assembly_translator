#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "../include/scanner.hpp"
#include "../include/preprocesser.hpp"
#include "../include/mounter_exception.hpp"
#include "../include/operation_supplier.hpp"

#define NOT_EMPTY(thing) (!thing.empty())

using namespace std;

Preprocesser::Preprocesser(bool verbose/* = false */) : verbose(verbose)  {
    // Popula a tabela de diretivas de préprocessamento
    // Implementação do padrão de projeto Command
    // pre_directive_table["EQU"] = &eval_EQU;
    // pre_directive_table["IF"] = &eval_IF;
    OperationSupplier supplier;
    pre_directive_table = supplier.supply_pre_directives();
}

vector<asm_line> Preprocesser::preprocess(string path, bool print /* = false */)
{
    // O parâmtero solicita que o scanner não levante erros
    Scanner scanner(false);
    // Coleta os erros lançados
    string error_log = "";

    // Gera a estrutura do programa
    vector<asm_line> lines = scanner.scan(path, error_log, print);
    // Guarda a estrutura préprocessada do programa
    vector<asm_line> processed_lines;

    // Levanta erro se receber o tipo errado de arquivo
    const size_t dot = path.find('.');
    if (path.substr(dot) != ".asm"s && path.substr(dot) != ".s"s) {
        throw invalid_argument("Tipo de arquivo inválido. Por favor, forneça um arquivo .asm ou .s");
    }

    // Passa por cada linha
    for (auto line_iterator = lines.begin(); line_iterator != lines.end(); line_iterator == lines.end() ? line_iterator : line_iterator++) {
        try {
            // cout << "Processando linha " << line_iterator->number << endl;
            const asm_line new_line = process_line(line_iterator);
            // valores negativos representam uma linha que não deve ir para o arquivo final
            if (new_line.number > 0) processed_lines.push_back(new_line);
        }
        catch (MounterException error) {
            // Coleta informações sobre o erro
            const int line = (error.get_line() == -1 ? line_iterator->number : error.get_line());

            error_log += "Na linha " + to_string(line) + ", erro " + error.get_type() + ": " + error.what() + "\n";
        }
    }

    if (verbose) {
        cout << "Definições da tabela de sinônimos:\n";
        for (auto synonym_entry_it = synonym_table.begin(); synonym_entry_it != synonym_table.end(); synonym_entry_it++) {
            cout << "\t" << synonym_entry_it->first << ": " << synonym_entry_it->second << endl;
        }
    }
    
    // Finaliza o arquivo ou imprime os erros
    if NOT_EMPTY(error_log) {
        throw MounterException (-1, "null",
            string(__FILE__) + ":" + to_string(__LINE__) + "> ERRO:\n" + error_log.substr(0, error_log.length()-1)
        );
    }
    
    synonym_table.clear();

    return processed_lines;
}

asm_line Preprocesser::process_line(vector<asm_line>::iterator &line_iterator) {
    asm_line &line = *line_iterator;

    // cout << "Tabela de sinônimos:\n";
    // for(auto it = synonym_table.cbegin(); it != synonym_table.cend(); ++it) {
    //     cout << it->first << ": " << to_string(it->second) << endl;
    // }

    // Substitui ocorrências de sinônimos pelos seus valores
    if NOT_EMPTY(line.operand[0]) {
        auto synonym_entry = synonym_table.find(line.operand[0]);
        if (synonym_entry != synonym_table.end()) {
            line.operand[0] = to_string(synonym_entry->second);
        }
    }
    if NOT_EMPTY(line.operand[1]) {
        auto synonym_entry = synonym_table.find(line.operand[1]);
        if (synonym_entry != synonym_table.end()) {
            line.operand[1] = to_string(synonym_entry->second);
        }
    }

    // Verifica a operação da linha contra as diretivas de préprocessamento
    const auto directive_entry = pre_directive_table.find(line.operation);
    // Verifica se houve correspondência
    if (directive_entry != pre_directive_table.end()) {
        // Invoca a rotina da diretiva
        auto eval_routine = directive_entry->second;
        eval_routine(line_iterator, this);
        // A diretiva não vai para o programa final
        line.number = -1;
    }
    
    return line;
}
