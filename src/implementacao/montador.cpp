#include <bits/stdc++.h>
using namespace std;

// for debugging
template <typename... A> void dbg(A const&... a) { ((cerr << "{" << a << "} "), ...); cerr << endl; }

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

//Tabela de Definições
unordered_map<string, vector<int>> tabDef;

//Tabela de Uso
unordered_map<string, vector<int>> tabUso;

static inline void trim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !isspace(ch);
    }));
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !isspace(ch);
    }).base(), s.end());
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
    tabDiret["SPACE"] = 2; // tem que aceitar args extras
    // tabDiret["SECTION"] = 1; -> considerado no preprocessamento
    tabDiret["EXTERN"] = 2;
    tabDiret["PUBLIC"] = 2;
    tabDiret["BEGIN"] = 1;
    tabDiret["END"] = 1;
}

void preprocess(string filename) {
    fstream inFile, auxFile;

    // arquivo de leitura em assembly
    inFile.open(filename + ".asm", ios::in);

    // arquivo intermediário contendo o texto preprocessado em .txt
    auxFile.open(filename + "_aux.asm", ios::out | ios::trunc);

    string line, label;
    vector<string> fileVec;

    // Lendo arquivo de entrada e limpando formatação
    if (inFile.is_open()) {
        while ( getline(inFile, line)) {
            // convertendo cada linha para upper case
            transform(line.begin(), line.end(), line.begin(), ::toupper);

            // removendo comentarios
            auto pos = line.find(";");
            if (pos != string::npos) line = line.substr(0, pos);

            // criando um espaço depois de ":" -> ": "
            auto pos2 = line.find(":");
            if (pos2 != string::npos) {
                line.insert(pos2+1, " ");
            }

            // removendo espaços em branco e tabulações desnecessárias no meio
            // Além disso, retira ',' do copy
            stringstream ss(line);
            string word;
            line = "";
            while(ss >> word) {
                if (word != ",")
                    line += word + " ";
            }
            
            // unindo " :" aos rótulos
            auto pos3 = line.find(" :");
            if (pos3 != string::npos) line.erase(pos3, 1);

            // "trim", removendo espaços no fim e começo
            trim(line);

            // retirando ':' de "Extern:" e "Public:"
            stringstream ss2(line);
            line = "";
            while(ss2 >> word) {
                if (word == "EXTERN:" or word == "PUBLIC:")
                    line += word.substr(0, word.size()-1) + " ";
                else
                    line += word + " ";
            }

            // unindo linhas diferentes que são do mesmo rótulo
            if (line.back() == ':') {
                label = line;
                continue;
            }

            // pula linhas com apenas '\n', que é removido pelo getline
            if (line.empty()) continue;

            // salvando linha no arquivo intermediário
            if (!label.empty()) { // se juntei linhas
                fileVec.push_back(label + " " + line + '\n');
                label = "";
            }
            else // se não juntei linhas
                fileVec.push_back(line + '\n');
        }
    }

    // cuidando da diretiva SECTION
    
    int secDataLine = -1, secTextLine = -1;
    int file_sz = (int) fileVec.size();
    
    for(int i=0; i<file_sz; i++) {
        line = fileVec[i];

        // .split()
        stringstream sss(line);
        string word;
        int j = 0;
        bool flagSection = false;
        while(sss >> word) {
            if (j == 0 and word == "SECTION") {
                flagSection = true;
            }
            else if (flagSection and j == 1 and word == "DATA") {
                secDataLine = i;
            }
            else if (flagSection and j == 1 and word == "TEXT") {
                secTextLine = i;
            }
            j += 1;
        }
    }

    // debbugging:
    // cout << "text = " << secTextLine << " data = " << secDataLine << endl;

    vector<string> fileVec2;
    
    if (secTextLine == -1) {
        // Nao tem SECTION TEXT -> erro sintático? (colocar isso)
        cout << "WARNING: Faltando SECTION TEXT" << endl;
        
        // Talvez de ainda para montar o arquivo, portanto:
        // removendo a linha de SECTION DATA, caso tenha.
        for(int i=0; i<file_sz; i++) {
            if (i == secDataLine) continue;
            fileVec2.push_back(fileVec[i]);
        }
    }

    // Nao tem SECTION DATA -> retirar apenas a linha de SECTION TEXT
    else if (secDataLine == -1) {
        // Nao tem SECTION DATA -> WARNING (talvez só ignorar?)
        cout << "WARNING: Faltando SECTION DATA" << endl;

        // removendo a linha de SECTION TEXT
        for(int i=0; i<file_sz; i++) {
            if (i == secTextLine) continue;
            fileVec2.push_back(fileVec[i]);
        }
    }

    // Tem SECTION DATA e TEXT, removendo essas linhas e invertendo a ordem caso necessario
    else {

        // Text vem antes de Data
        if (secTextLine < secDataLine) {
            for(int i=0; i<file_sz; i++) {
                if (i == secTextLine or i == secDataLine) continue;
                fileVec2.push_back(fileVec[i]);
            }
        }

        // DATA vem antes de TEXT
        else {
            // copiando cabeçalho
            for(int i=0; i<secDataLine; i++)
                fileVec2.push_back(fileVec[i]);
            
            // copiando TEXT
            for(int i=secTextLine+1; i<file_sz; i++)
                fileVec2.push_back(fileVec[i]);
            
            // copiando DATA
            for(int i=secDataLine+1; i<secTextLine; i++)
                fileVec2.push_back(fileVec[i]);
        }
    }
    
    for(auto str : fileVec2) 
        auxFile << str;
    
    inFile.close();
    auxFile.close();
}

//Verifica a existência de erros léxicos em Tokens
void scanner(string token, int lineCounter){
    int sz = (int) token.size();
    for(int i=0; i<sz; i++) {
        // verifica se o primeiro caractere é um número (proibido)
        if ( i == 0 and isdigit(token[i])) {
            cout << "ERROR: Erro Lexico no token {" << token << "} da linha: " << lineCounter << endl;
            return;
        }
        // verifica se todos os caracteres são apenas: letras, underscores ou números
        if ( !(isalpha(token[i]) or isdigit(token[i]) or token[i] == '_') ) {
            cout << "ERROR: Erro Lexico no token {" << token << "} da linha: " << lineCounter << endl;
            return;
        }
    }
}

bool assemble(string filename, int programas) {
    fstream outFile, auxFile;

    // arquivo intermediário contendo o texto preprocessado também em .asm
    auxFile.open(filename + "_aux.asm", ios::in);

    // reiniciando as tabelas
    tabSimb.clear();
    tabPend.clear();
    tabUso.clear();
    tabDef.clear();

    vector<int> mem;
    vector<int> relativos;
    string line;

    //Struct que define pendências

    // Mais de um Rotulo na mesma linha
    int extraLabelCounter = 0;

    // Contador do endereço de memória
    int addressCounter = 0;

    // Contador as linhas para exibir nas mensagens de erro
    int lineCounter = 1; // indexado em 1
    
    // Flags para verificar se existe BEGIN e END
    int flag_begin = 0;
    int flag_end = 0;

    // Flags para verificar se existe EXTERN e PUBLIC
    int flag_extern = 0;
    int flag_public = 0;

    // Apenas um Alerta de Erro será mostrado
    int erros_ditos_Data = 1;
    int erros_ditos_Txt = 1;

    if (auxFile.is_open()) {
        while ( getline(auxFile, line)) {
            dbg(line);
            
            // tokenizando a linha em palavras
            stringstream ss(line);
            string word;
            vector<string> lineVec;
            while(ss >> word) lineVec.push_back(word);

            // essa linha possui rótulo
            if (lineVec[0].back() == ':') {
                string label = lineVec[0].substr(0, lineVec[0].size()-1);
                tabSimb[label] = addressCounter;
                lineVec.erase(lineVec.begin());

                //Incrementa extraLabelCounter, verificando se há mais de um rótulo na linha
                vector<string> aux;
                for(auto str : lineVec) {
                    if (str.back() == ':') extraLabelCounter += 1;
                    else aux.push_back(str);
                }
                if (extraLabelCounter > 0) {
                    printf("ERROR: (Erro Sintatico) na linha %d. Mais de um rotulo na mesma linha\n", lineCounter);
                    lineVec = aux;
                }

                //Verifica Erros Léxicos no Rótulo
                scanner(label, lineCounter);
            }

            string instruction = lineVec[0];

            // verifica se é uma diretiva
            if (tabDiret[instruction] != 0) {

                if (instruction == "SPACE") {

                    // TODO: Implementar caso em que ha argumentos passados para SPACE
    
                    mem.push_back(0);
                    addressCounter += 1;
                }

                else if (instruction == "CONST") {
                    // Se não tiver o segundo argumento -> segfault
                    if (lineVec.size() < 2) {
                        cout << "ERROR: (Erro Sintatico) na linha " << lineCounter << ". Instrucao CONST nao possui argumento." << endl;
                        return false;
                    }
                    mem.push_back( stoi(lineVec[1]) );
                    addressCounter += 1;
                }

                else if (instruction == "BEGIN") {
                    flag_begin = 1;
                }

                else if (instruction == "END") {
                    flag_end = 1;
                }

                else if (instruction == "EXTERN") {
                    if (lineVec.size() < 2) {
                        cout << "ERROR: (Erro Sintatico) na linha " << lineCounter << ". Instrucao EXTERN nao possui argumento"  << endl;
                        return false;
                    }

                    string arg = lineVec[1];
                    tabUso[arg] = vector<int>();
                    tabSimb[arg] = -1;
                    flag_extern = 1;

                    if(flag_begin) 
                        printf("WARNING: (Erro Semantico) na linha %d, sem BEGIN\n", lineCounter);
                }

                else if (instruction == "PUBLIC") {
                    if (lineVec.size() < 2) {
                        cout << "ERROR: (Erro Sintatico) na linha " << lineCounter << ". Instrucao PUBLIC nao possui argumento"  << endl;
                        return false;
                    }

                    string arg = lineVec[1];
                    tabDef[arg] = vector<int>();
                    flag_public = 1;
                    if(flag_begin) 
                        printf("WARNING: (Erro Semantico) na linha %d, sem BEGIN\n", lineCounter);
                }

            }

            // é uma instrução do assembly
            else {
                // coloca o opcode na memória
                mem.push_back(tabInstr[instruction].first);
                addressCounter += 1;
                int argSize = tabInstr[instruction].second;

                //int ejmp = 0; // ? 

                // adiciona linha a lista de pendencias
                for(int i=1; i<argSize; i++) {
                    string arg = lineVec[i];

                    //Analisador Léxico
                    scanner(arg, lineCounter);

                    // adiciona pendencia
                    mem.push_back(-1); 

                    // caso o vetor de pendencias desse label nao esteja inicializado
                    if (!tabPend.count(arg)) tabPend[arg] = vector<int>();

                    tabPend[arg].push_back(addressCounter);
                    relativos.push_back(addressCounter);

                    // tabPend[arg].push_back(lineCounter);
                    //
                    // if(instruction == "JMP" || instruction == "JMPZ" || instruction == "JMPP" || instruction == "JMPN"){
                    //     tabPend[arg].push_back(1);
                    // }
                    // else{
                    //     tabPend[arg].push_back(0);
                    // }

                    addressCounter += 1;
                }
            }
            lineCounter++;
        }
    }

    // Verifica se nao ha END, mesmo tendo BEGIN, EXTERN ou PUBLIC
    if(!flag_end)
        if (flag_begin or flag_extern or flag_public)
            printf("WARNING: (Erro Semantico) na linha %d, sem END\n", lineCounter);

    // Arruma todas as pendências (continuar verificando daqui)
    for(auto [lab, vec] : tabPend) {
        int flag_erro = 0;
        int flag_idx = 0;
        for(auto idx : vec) {
            flag_idx++;
            if(flag_idx == 1){
                mem[idx] = tabSimb[lab];
                if(tabDef.count(lab)){tabDef[lab].push_back(tabSimb[lab]);}

                if(tabUso.count(lab)){tabUso[lab].push_back(idx);}
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

    // arquivo de saída em binário que não vai ser ligado
     if(flag_extern || flag_public || !flag_begin || !flag_end){
        outFile.open(filename + ".obj", ios::out | ios::trunc);
     }
     else{
        outFile.open(filename + ".exc", ios::out | ios::trunc);
     }

    // escrevendo no arquivo de saída
    string ans;
    if(programas > 2 || flag_extern){
        ans+="USO\n";
        for(auto [lab, vec] : tabUso){
            for(auto aux : vec){
                ans+= lab + " " + to_string(aux) + "\n";
            }
        }
        ans+="DEF\n";
        for(auto [lab, vec] : tabDef){
            for(auto aux : vec){
                ans+= lab + " " + to_string(aux) + "\n";
            }
        }
        ans+="RELATIVOS\n";
        for(auto vec : relativos){
            ans+= to_string(vec) + " ";
        }
        ans+="\nCODE\n";
    }
    for(auto i : mem) ans += to_string(i) + " ";
    if (outFile.is_open()) {
        outFile << ans << '\n';
    }

    auxFile.close();
    outFile.close();

    // nao houve problemas para montar
    return true; 
}

void linker(string obj1, string obj2 = "", string obj3 = "", string obj4 = ""){
    fstream outFile, auxFile1,auxFile2,auxFile3,auxFile4;
    string line;
    //Variáveis Auxiliares
    int fator_correcao = 0;
    int proximo_uso = 0;
    int proximo_def = 0;
    int proximo_codigo = 0;
    auxFile1.open(obj1 + ".obj", ios::in);

    unordered_map<string, int> tgs;
    unordered_map<string,vector<int>> t_uso;
    if (auxFile1.is_open()) {

        while ( getline(auxFile1, line)) {

            stringstream ss(line);
            string word;
            vector<string> lineVec;
            while(ss >> word) lineVec.push_back(word);
            if(lineVec[0] == "USO"){proximo_uso = 1;}
            else if(lineVec[0] == "DEF"){proximo_def = 1;}
            else if(lineVec[0] == "CODE"){proximo_codigo = 1;}

            if(proximo_uso){
                string arg = lineVec[0];
                int val_uso = stoi(lineVec[1]) + fator_correcao;
                if (!t_uso.count(arg)) t_uso[arg] = vector<int>();
                t_uso[arg].push_back(val_uso);
                proximo_uso = 0;
            }
            if(proximo_def){
                string arg = lineVec[0];
                int val_uso = stoi(lineVec[1]) + fator_correcao;
                tgs[arg] = val_uso;
                proximo_def = 0;
            }
            if(proximo_codigo){
                    fator_correcao = lineVec.size();
                    proximo_codigo = 0;
            }


        }

    }
    auxFile1.close();
}


int32_t main(int argc, char** argv) {

    // inicializa a tabela de instruções e a de diretivas
    init_fixed_tables();

    // adquirindo o nome dos arquivos passados como argumento no terminal, 
    // preprocessando e montando esses arquivos
    for(int i=1; i<argc; i++) {
        string filename = argv[i];
        preprocess(filename);
        if ( !assemble(filename,argc) )
            cout << "Falha na Montagem do Arquivo" << filename << endl;
    }

    //Trocar >= por > para teste
    if(argc >= 2){
        linker(argv[1]);
    }
}
