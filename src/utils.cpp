#include <fstream>
#include <iostream>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/ucnv.h>
#include <unicode/uchar.h>
#include <string>
#include <sstream> 


using namespace std;
using namespace icu;

// Retorna uma stringstream com as palavras de um arquivo de texto formatadas
stringstream read_file(const string &file_path) {
    // Abre o arquivo
    ifstream input_file(file_path, ios::binary);
    if (!input_file.is_open()) {
        cerr << "Error: Could not open file " << file_path << endl;
        exit(1);
    }
    
    // Salva o conteúdo do arquivo em uma string e converte para UnicodeString
    string file_content((istreambuf_iterator<char>(input_file)), istreambuf_iterator<char>());
    UnicodeString unicode_content = UnicodeString::fromUTF8(StringPiece(file_content));

    // Converte todos os caracteres para minúsculo e substitui pontuações por espaços
    // Obs: Falta permitir o uso de hífen em palavras compostas
    unicode_content.toLower();
    for (int i = 0; i < unicode_content.length(); i++) {
        UChar character = unicode_content[i];
        if (u_charType(character) == U_DASH_PUNCTUATION || u_charType(character) == U_OTHER_PUNCTUATION) {
            unicode_content.replace(i, 1, ' ');
        }
    }

    // Converte UnicodeString para string com o texto formatado
    string processed_content;
    unicode_content.toUTF8String(processed_content);

    // Retorna um stringstream com o conteúdo do arquivo
    return stringstream(processed_content);
    
}
