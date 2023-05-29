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

//Verifica a existência de erros léxicos em Tokens
void analisador_lexico(string arg, int cont){
    bool f = isalpha(arg[0]);
    if(!f && arg[0] != '_'){
        printf("Erro Lexico na linha %d, Token invalido\n", cont);
    }
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
    //Conta as linhas para exibir nas mensagens de erro
    int cont_linha = 0;
    //Flags para verificar Section Text e Section Data
    int flag_texto = 1;
    int flag_data = 1;
    //Flags para verificar BEGIN e END
    int flag_begin = 1;
    int flag_end = 1;
    //Flags para verificar EXTERN e PUBLIC
    int flag_extern = 0;
    int flag_public = 0;
    //Apenas um Alerta de Erro será mostrado
    int erros_ditos_Data = 1;
    int erros_ditos_Txt = 1;
    //Mais de um Rotulo
    int flag_mais_de_um_rotulo = 0;
    if (auxFile.is_open()) {
        while ( getline(auxFile, line)) {
            cont_linha++;
            stringstream ss(line);
            string word;
            vector<string> lineVec;
            while(ss >> word) lineVec.push_back(word);

            // essa linha possui rótulo
            if (lineVec[0].back() == ':') {
                string label = lineVec[0].substr(0, lineVec[0].size()-1);
                tabSimb[label] = counter;
                lineVec.erase(lineVec.begin());
                vector<string> auxiliar;
                //Incrementa flag_rot e verifica se há mais de um rótulo na linha
                int numWords = lineVec.size();
                for(int i = 0; i < numWords; i++){
                    if(lineVec[i].back() == ':'){
                        if(flag_mais_de_um_rotulo == 0){
                        printf("Erro Sintatico na linha %d, mais de um rotulo na mesma linha\n", cont_linha);
                        }
                        flag_mais_de_um_rotulo++;
                    }
                    else{

                        auxiliar.push_back(lineVec[i]);
                    }
                }
                if(flag_mais_de_um_rotulo > 0){
                lineVec.clear();
                copy(auxiliar.begin(), auxiliar.end(), back_inserter(lineVec));
                }
                //Verifica Erros Léxicos no Rótulo
                analisador_lexico(label, cont_linha);
            }

            string instruction = lineVec[0];

            // verifica se é uma diretiva
            if (tabDiret[instruction] != 0) {

                if (instruction == "SECTION"){
                    string tipo = lineVec[1];
                    if(tipo == "TEXT"){
                        flag_texto = 0;
                    }
                    else if(tipo == "DATA"){
                        flag_data = 0;
                    }

                }

                else if (instruction == "SPACE") {
                        //Verifica Section DATA
                        if(flag_data && erros_ditos_Data){
                            erros_ditos_Data = 0;
                            printf("Erro Semantico, sem Section DATA antes da linha %d\n", cont_linha);
                        }
                    mem.push_back(0);
                    counter += 1;
                }

                else if (instruction == "CONST") {
                        //Verifica Section DATA
                        if(flag_data && erros_ditos_Data){
                            printf("Erro Semantico, sem Section DATA antes da linha %d\n", cont_linha);
                        }
                    mem.push_back( stoi(lineVec[1]) );
                    counter += 1;
                }

                else if (instruction == "BEGIN") {
                        flag_begin = 0;

                }
                else if (instruction == "END") {
                        flag_end = 0;
                }
                else if (instruction == "EXTERN") {
                        flag_extern = 1;
                        if(flag_begin) printf("Erro Semantico na linha %d, sem BEGIN\n", cont_linha);

                }
                else if (instruction == "PUBLIC") {
                        flag_public = 1;
                        if(flag_begin) printf("Erro Semantico na linha %d, sem BEGIN\n", cont_linha);

                }

            }

            // é uma instrução do assembly
            else {
                // coloca o opcode na memória
                //Verifica se Section Text foi definida
                    if(flag_texto && erros_ditos_Txt){
                        erros_ditos_Txt = 0;
                        printf("Erro Semantico, sem Section TEXT antes da linha %d\n", cont_linha);
                    }

                mem.push_back(tabInstr[instruction].first);
                counter += 1;
                int argSize = tabInstr[instruction].second;

                // adiciona linha a lista de pendencias
                for(int i=1; i<argSize; i++) {
                    string arg = lineVec[i];
                    //Analisador Léxico
                    analisador_lexico(arg, cont_linha);
                    mem.push_back(-1);
                    if (!tabPend.count(arg)) tabPend[arg] = vector<int>();
                    tabPend[arg].push_back(counter);
                    tabPend[arg].push_back(cont_linha);
                    if(instruction == "JMP" || instruction == "JMPZ" || instruction == "JMPP" || instruction == "JMPN"){
                        tabPend[arg].push_back(1);
                    }
                    else{
                        tabPend[arg].push_back(0);
                    }
                    counter += 1;
                }
            }
        }
    }

    //Verifica se há BEGIN e END na linha
    if((flag_extern && flag_end) || (flag_public && flag_end)){
            printf("Erro Semantico na linha %d, sem END\n", cont_linha);

    }

    // arruma todas as pendências
    for(auto [lab, vec] : tabPend) {
        int flag_erro = 0;
        int flag_idx = 0;
        for(auto idx : vec) {
            flag_idx++;
            if(flag_idx == 1){
                mem[idx] = tabSimb[lab];
            }

            if(flag_idx == 3){
                flag_idx = 0;
            }

            if(tabSimb[lab] == 0 && vec[2] == 0 && flag_erro == 0){
                printf("Erro Semantico, dado nao definido na linha %d\n", vec[1]);
                flag_erro = 1;
            }
            if(tabSimb[lab] == 0 && vec[2] == 1 && flag_erro == 0){
                printf("Erro Semantico, label nao definido na linha %d\n", vec[1]);
                flag_erro = 1;
            }
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
    tabDiret["EXTERN"] = 1;
    tabDiret["PUBLIC"] = 1;
    tabDiret["BEGIN"] = 1;
    tabDiret["END"] = 1;
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
