#include <iostream>
#include <sstream>
#include <string>

#include "utils.cpp"

using namespace std;

int main() {
    stringstream file_stream = read_file("in/memorias_postumas_de_braz_cubas.txt");
    string word;
    while (file_stream >> word) {
        cout << word << endl;
    }
}