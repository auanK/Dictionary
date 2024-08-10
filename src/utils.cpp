#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>

using namespace std;

// Retorna o conteúdo de um arquivo em um stringstream formatado.
stringstream read_file(const string& file_name) {
    // Abre o arquivo e verifica se foi aberto corretamente
    ifstream file(file_name);

    if (!file.is_open()) {
        cerr << "Error: Could not open file " << file_name << endl;
        exit(1);
    }

    // Itera sobre o conteúdo do arquivo e armazena em uma string
    string content((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));

    // Remove sinais de pontuação e converte todas as letras para minúsculas
    // Observações:
    // Falta fazer a conversão para minúsculas de letras acentuadas
    // e falta permitir o uso de hífen em palavras compostas.
    for (int i = 0; i < content.size(); i++) {
        content[i] = towlower(content[i]);
        if (ispunct(content[i])) {
            content[i] = ' ';
        }
    }

    return stringstream(content);
}