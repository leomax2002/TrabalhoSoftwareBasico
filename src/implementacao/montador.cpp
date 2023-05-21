#include <bits/stdc++.h>
using namespace std;

// Tabela de Instruções
// "mnemônico" -> {opcode, tamanho de palavras}
unordered_map<string, pair<int, int>> tabInstr;

// Tabela de Diretivas
// "mnemônico" -> {tamanho de palavras}
unordered_map<string, int> tabDiret;

// Tabela de Simbolos
// "label" -> {linha}
unordered_map<string, int> tabSimb;

// Pendências da Tabela de Simbolos
unordered_map<string, vector<int>> tabPend;

static inline void trim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !isspace(ch);
    }));
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !isspace(ch);
    }).base(), s.end());
}

void assemble(string filename) {
    fstream outFile, auxFile;
    
    // arquivo de saída em binário
    outFile.open(filename + ".exc", ios::out | ios::trunc);

    // arquivo intermediário contendo o texto preprocessado em .txt
    auxFile.open(filename + "_aux.txt", ios::in);

    // ignorando directiva SECTION por enquanto, o que exatamente precisa fazer?

    // reiniciando as tabelas de simbolos
    tabSimb.clear();
    tabPend.clear();

    vector<int> mem;
    string line;
    int counter = 0;

    if (auxFile.is_open()) {
        while ( getline(auxFile, line)) {
            
            stringstream ss(line);
            string word;
            vector<string> lineVec;
            while(ss >> word) lineVec.push_back(word);

            // essa linha possui rótulo
            if (lineVec[0].back() == ':') {
                string label = lineVec[0].substr(0, lineVec[0].size()-1);
                tabSimb[label] = counter;
                lineVec.erase(lineVec.begin());
            }

            string instruction = lineVec[0];

            // verifica se é uma diretiva
            if (tabDiret[instruction] != 0) {
                if (instruction == "SECTION") continue; // <- linha temporária de codigo
                
                else if (instruction == "SPACE") {
                    mem.push_back(0);
                    counter += 1;
                }

                else if (instruction == "CONST") {
                    mem.push_back( stoi(lineVec[1]) );
                    counter += 1;
                }
            }

            // é uma instrução do assembly
            else {
                // coloca o opcode na memória
                mem.push_back(tabInstr[instruction].first);
                counter += 1;
                int argSize = tabInstr[instruction].second;

                // adiciona linha a lista de pendencias
                for(int i=1; i<argSize; i++) {
                    string arg = lineVec[i];
                    mem.push_back(-1);
                    if (!tabPend.count(arg)) tabPend[arg] = vector<int>();
                    tabPend[arg].push_back(counter);
                    counter += 1;
                }
            }
        }
    }

    // arruma todas as pendências
    for(auto [lab, vec] : tabPend) {
        for(auto idx : vec) {
            mem[idx] = tabSimb[lab];
        }
    }

    // escrevendo no arquivo de saída
    string ans;
    for(auto i : mem) ans += to_string(i) + " ";
    if (outFile.is_open()) {
        outFile << ans << '\n';
    }

    auxFile.close();
    outFile.close();
}

void init_fixed_tables() {
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

    tabDiret["CONST"] = 2;
    tabDiret["SPACE"] = 1;
    tabDiret["SECTION"] = 1;
} 

void preprocess(string filename) {
    fstream inFile, auxFile;

    // arquivo de leitura em assembly
    inFile.open(filename + ".asm", ios::in);

    // arquivo intermediário contendo o texto preprocessado em .txt
    auxFile.open(filename + "_aux.txt", ios::out | ios::trunc);

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

            // criando um espaço depois de ": " 
            auto pos3 = line.find(":");
            if (pos3 != string::npos) {
                line.insert(pos3+1, " ");
            }

            // removendo espaços em branco e tabulações desnecessárias no meio e (',') do copy
            stringstream ss(line);
            string word;
            line = "";
            while(ss >> word) if (word != ",") line += word + " ";

            // unindo " :" aos rótulos
            auto pos2 = line.find(" :");
            if (pos2 != string::npos) line.erase(pos2, 1);

            // "trim", removendo espaços no fim e começo
            trim(line);

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

    inFile.close();
    auxFile.close();
}

int32_t main(int argc, char** argv) {

    init_fixed_tables();

    // nome dos arquivos passados como argumento no terminal
    for(int i=1; i<argc; i++) {
        string filename = argv[i];
        preprocess(filename);
        assemble(filename);
    }
}