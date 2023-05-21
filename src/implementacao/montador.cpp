#include <bits/stdc++.h>
using namespace std;

// Tabela de Instruções
unordered_map<string, pair<int, int>> tabInstr;

// Tabela de Diretivas

// Tabela de Simbolos



// "mnemônico" -> {opcode, tamanho de palavras}
void init_tabInstr() {
    tabInstr["ADD"] = {1, 2};
    tabInstr["SUB"] = {2, 2};
    tabInstr["MUL"] = {3, 2};
    tabInstr["DIV"] = {4, 2};
    tabInstr["JMP"] = {5, 2};
    tabInstr["JMPN"] = {6, 2};
    tabInstr["JMPP"] = {7, 2};
    tabInstr["JMPZ"] = {8, 2};
    tabInstr["COPY"] = {9, 3};
    tabInstr["LOAD"] = {10, 2};
    tabInstr["STORE"] = {11, 2};
    tabInstr["INPUT"] = {12, 2};
    tabInstr["OUTPUT"] = {13, 2};
    tabInstr["STOP"] = {14, 1};
} 

static inline void trim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !isspace(ch);
    }));
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !isspace(ch);
    }).base(), s.end());
}

void preprocess(fstream &inFile, fstream &auxFile) {
    string line, label;

    if (inFile.is_open()) {
        while ( getline(inFile, line)) {

            // skip lines with only '\n'
            if (line.empty()) continue;

            // convertendo cada linha para upper case
            transform(line.begin(), line.end(), line.begin(), ::toupper);

            // removendo comentarios
            auto pos = line.find(";");
            if (pos != string::npos) line = line.substr(0, pos);

            // "trim", removendo espaços no fim e começo
            trim(line);

            // removendo espaços em branco e tabulações desnecessárias no meio
            string newline;
            int sz = (int) line.size();
            for(int i=0; i<sz; i++) {
                char c = line[i];
                if (c == '\t') continue;
                if (c == ' ' and line[i-1] == ' ') continue;
                
                newline += c;
            }
            line = newline;

            // unindo ':' aos rótulos
            auto pos2 = line.find(" :");
            if (pos2 != string::npos) line.erase(pos2, 1);

            // unindo linhas que são do mesmo rótulo
            if (line.back() == ':') {
                label = line;
                continue;
            }

            // salvando linha no arquivo intermediário
            if (label != "") {
                auxFile << label << " " << line << '\n';
                label = "";
            }
            else auxFile << line << '\n';
        }
    }
}

int32_t main(int argc, char** argv) {
    fstream inFile, outFile, auxFile;

    // nome do arquivo passado como argumento no terminal
    string filename = argv[1];
    if (argc != 2) cout << "CMD ERROR when passing arguments" << endl;

    // arquivo de leitura em assembly
    inFile.open(filename + ".asm", ios::in);

    // arquivo intermediário contendo o texto preprocessado em .txt
    auxFile.open(filename + "_aux.txt", ios::in | ios::out | ios::trunc);
    
    // arquivo final em binário
    outFile.open(filename + ".exc", ios::out | ios::trunc);

    preprocess(inFile, auxFile);

    init_tabInstr();
}