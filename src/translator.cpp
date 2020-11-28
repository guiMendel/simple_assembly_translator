#include "../include/scanner.hpp"
#include "../include/translator.hpp"

#include <algorithm>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

#define NOT_EMPTY(thing) (!thing.empty())

void Translator::translate(vector<asm_line>& program_lines, string name) {
    // Arquivo a ser construído
    fstream out(name, fstream::out);

    if (!out.is_open()) {
        throw invalid_argument("Não foi possível criar o arquivo \"" + name + "\"");
    }

    // // Deleta o arquivo vazio
    // remove(out_path.c_str());

}

void Translator::read_table(string path) {
    fstream source(path);
    
    if (!source.is_open()) {
        throw invalid_argument("Não foi possível abrir o arquivo da tabela de conversões em \"" + path + "\"");
    }

    string line;
    
    // Inicializa os campos com os valores padrões
    string operation = "";
    string section = "text";
    string params[2];
    params[0] = "";
    vector<string> instructions;

    // Flags que indicam a fase atual de construção de cada entrada
    bool params_ok = false;
    
    while (getline(source, line)) {
        // cout << "Linha bruta lida: \"" << line << "\"" << endl;
        // Tratamos \t, \r e \n como espaços
        replace(line.begin(), line.end(), '\t', ' ');
        replace(line.begin(), line.end(), '\n', ' ');
        replace(line.begin(), line.end(), '\r', ' ');

        // Remove espaços antes e depois da linha (se houver algo além de espaços)
        if (line.find_first_not_of(" ") != string::npos) {
            line = line.substr(line.find_first_not_of(" "));
            line = line.substr(0, line.find_last_not_of(" ") + 1);
        }

        // Descarta linhas vazias e comentários
        if (line.empty() || line[0] == '#') continue;

        // cout << "Linha lida: \"" << line << "\"" << endl;
        
        // Se já tiver operação e parâmetros, joga as linhas inteiras nas instruções
        if (NOT_EMPTY(operation) && params_ok) {
            // Verifica se essa linha especifica a seção
            if (line[0] == '(') {
                // Remove os parênteses
                line = line.substr(1, line.length() - 2);
                // Valida a seção
                if (line != "text"s && line != "data"s && line != "bss"s) throw invalid_argument(
                    "Seção da operação \"" + operation + "\" é inválida"
                );
                // Muda a seção
                section = line;

                continue;
            }

            // Verifica se essa é a chave de fim do bloco
            if (line[0] == '}') {
                // Verifica se há pelo menos alguma instruções registrada
                if (instructions.empty()) throw invalid_argument("A operação \"" + operation + "\" não tem nenhuma instrução associada");

                // Insere na tabela essa entrada
                // cout << "Salvando entrada da operação \"" + operation + "\"" << endl;
                translation_table[operation] = translation_entry(section, params, instructions);

                // Restitui os campos padrões
                operation = "";
                section = "text";
                params[0] = "";
                params[1] = "";
                instructions.clear();
                params_ok = false;

                continue;
            }

            instructions.push_back(line);
            continue;
        }
        
        // Faz um sstream para ler cada token
        string token;
        stringstream line_stream(line);
        while (getline(line_stream, token, ' ')) {
            // cout << "Token lido: \"" << token << "\"" << endl;
            // Descarta tokens vazios
            if (token.empty()) continue;
            
            // Fase de ler operador
            if (operation.empty()) {
                // Verifica se já tem essa operação
                for (auto const& entry : translation_table) {
                    if (entry.first == token) throw invalid_argument(
                        "Definição dupla para a operação \"" + token + "\""
                    );
                }

                operation = token;
                continue;
            }

            // Fase de ler parâmetros
            if (!params_ok) {
                // Verifica se é o início do bloco de instruções
                if (token == "{"s) {
                    params_ok = true;
                    continue;
                }
                
                if (params[0].empty()) {
                    // Verifica a presença da vírgula
                    if (token[token.length() - 1] == ',') {
                        token.pop_back();
                    }
                    params[0] = token;
                }
                else params[1] = token;
                continue;
            }
        }
    }
}

void Translator::print_table() {
    cout << "Tabela de tradução:" << endl;
    for (auto const& entry : translation_table) {
        cout << entry.first << " (" << entry.second.params[0] << ", " << entry.second.params[1] << "), section " << entry.second.section << ", {" << endl;
        for (auto const& instruction : entry.second.instructions) {
            cout << "\t" << instruction << endl;
        }
        cout << "}" << endl;
    }
}