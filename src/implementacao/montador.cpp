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
// "label" -> {endereço}
// endereço == -1 -> (label externo)
unordered_map<string, int> tabSimb;

// Pendências da Tabela de Simbolos
// "label" -> vector{endereço, linha}
unordered_map<string, vector<pair<int, int>>> tabPend;

//Tabela de Definições
// "label" -> {endereço}
unordered_map<string, int> tabDef;

//Tabela de Uso
// "label" -> vector{endereço}
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
    tabDiret["SPACE"] = 2;
    tabDiret["SECTION"] = 1;
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

            // pula linhas com apenas '\n', que é removido pelo getline
            if (line == "") continue;

            // convertendo cada linha para upper case
            transform(line.begin(), line.end(), line.begin(), ::toupper);

            // removendo comentarios
            auto pos = line.find(";");
            if (pos == 0) continue; // linha só comentário
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

            // retirando ':' de "Extern:" e "Public:" e tratando Hex
            stringstream ss2(line);
            line = "";
            int hexdex;
            //int hexdex;
            while(ss2 >> word) {
                if (word == "EXTERN:" or word == "PUBLIC:"){
                    line += word.substr(0, word.size()-1) + " ";
                }
                else{
                    if(word.substr(0,2) == "0X"){
                        std::istringstream(word) >> std::hex >> hexdex;
                        //cout << "hex" << word_hexdex << endl;
                        line += to_string(hexdex) + " ";
                    }
                    else{
                        line += word + " ";
                    }
                }
            }

            // unindo linhas diferentes que são do mesmo rótulo
            if (line.back() == ':') {
                label = line;
                continue;
            }

            // pula linhas com apenas '\n', que é removido pelo getline
            if (line == "") continue;

            // salvando linha no arquivo intermediário
            if (label != "") { // se juntei linhas
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

    vector<string> fileVec2;

    // Tem SECTION DATA e TEXT, invertendo a ordem caso necessario
    if (secTextLine != -1 and secDataLine != -1) {

        // TEXT vem antes de DATA
        if (secTextLine < secDataLine) {
            fileVec2 = fileVec;
        }

        // DATA vem antes de TEXT
        else {
            // copiando cabeçalho
            for(int i=0; i<secDataLine; i++)
                fileVec2.push_back(fileVec[i]);

            // copiando TEXT
            for(int i=secTextLine; i<file_sz; i++)
                fileVec2.push_back(fileVec[i]);

            // copiando DATA
            for(int i=secDataLine; i<secTextLine; i++)
                fileVec2.push_back(fileVec[i]);
        }
    }
    // Caso falte algum SECTION
    else {
        fileVec2 = fileVec;
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

bool isNum(string token){
    // verifica se todos os caracteres são apenas: números
    for(char c : token) if (!isdigit(c)) {
        return false;
    }
    return true;
}

bool assemble(string filename) {
    fstream outFile, auxFile;

    // arquivo intermediário contendo o texto preprocessado também em .asm
    auxFile.open(filename + "_aux.asm", ios::in);

    // reiniciando as tabelas
    tabSimb.clear();
    tabPend.clear();
    tabUso.clear();
    tabDef.clear();

    vector<int> mem;

    //Fator de Offset para Argumentos do tipo X+2
    vector<int> fator_offset;

    vector<int> relativos;

    // se um label é de dado, em vez de endereço
    set<string> tipoDado;

    string line;

    // Contador do endereço de memória
    int addressCounter = 0;

    // Contador as linhas para exibir nas mensagens de erro
    int lineCounter = 1; // indexado em 1

    // Flags para verificar se existe e quantos existem: BEGIN e END
    int flag_begin = 0;
    int flag_end = 0;

    // Flags para verificar se existe EXTERN e PUBLIC
    bool flag_extern = 0;
    bool flag_public = 0;

    // Flags para verificar se existe SECTION TEXT e SECTION DATA
    bool flag_text = 0;
    bool flag_data = 0;

    // Flag para saber se o arquivo gerou um objeto corretamente
    bool no_major_errors_flag = true;

    if (auxFile.is_open()) {
        while ( getline(auxFile, line)) {

            // tokenizando a linha em palavras
            stringstream ss(line);
            string word;
            string lab_extern = "";
            vector<string> lineVec;
            while(ss >> word) lineVec.push_back(word);

            // Mais de um Rotulo na mesma linha
            int extraLabelCounter = 0;

            string label = "";
            // essa linha possui rótulo
            if (lineVec[0].back() == ':') {
                label = lineVec[0].substr(0, lineVec[0].size()-1);
                tabSimb[label] = addressCounter;
                if(lineVec.size() > 1 && lineVec[1] == "EXTERN") lab_extern = label;
                lineVec.erase(lineVec.begin());

                //Incrementa extraLabelCounter, verificando se há mais de um rótulo na linha
                vector<string> aux;
                for(auto str : lineVec) {
                    if (str.back() == ':') extraLabelCounter += 1;
                    else aux.push_back(str);
                }

                if (extraLabelCounter > 0) {
                    printf("ERROR: (Erro Sintatico) na linha %d. Mais de um rotulo na mesma linha\n", lineCounter);

                    no_major_errors_flag = false;

                    // aux armazena a linha sem labels
                    lineVec = aux;
                }

                //Verifica Erros Léxicos no Rótulo
                scanner(label, lineCounter);
            }

            string instruction = lineVec[0];

            // verifica se é uma diretiva
            if (tabDiret[instruction] != 0) {

                if (instruction == "SPACE") {

                    if (!flag_data){
                        printf("WARNING: (Erro Semântico) na linha %d. Sem SECTION DATA\n", lineCounter);
                        flag_data = 1; // imprimir uma vez apenas
                    }

                    // SPACE possui argumento
                    if(lineVec.size() > 1){
                        int n_args_space = stoi(lineVec[1])-1;
                        for(int j = 0; j < n_args_space; j++){
                            mem.push_back(0);
                            fator_offset.push_back(0);
                            addressCounter+=1;
                        }

                    }
                    mem.push_back(0);
                    fator_offset.push_back(0);
                    tipoDado.insert({label});
                    addressCounter += 1;
                }

                else if (instruction == "CONST") {

                    if(!flag_data){
                        printf("WARNING: (Erro Semântico) na linha %d. Sem SECTION DATA\n", lineCounter);
                        flag_data = 1; // imprimir uma vez apenas
                    }

                    if (lineVec.size() < 2) {
                        cout << "ERROR: (Erro Sintatico) na linha " << lineCounter << ". Instrucao CONST nao possui argumento." << endl;

                        no_major_errors_flag = false;
                        // considererar então o argumento == 0
                        // lineVec[1] = 0
                        lineVec.push_back("0");
                    }
                    mem.push_back( stoi(lineVec[1]) );
                    fator_offset.push_back(0);
                    tipoDado.insert({label});
                    addressCounter += 1;
                }

                else if (instruction == "BEGIN") {
                    flag_begin += 1;

                    // retirar aqui em baixo, tratamento de erro desnecessario
                    if (flag_begin >= 2)
                        cout << "ERROR: (Erro Sintatico) na linha " << lineCounter << ". Mais de uma Instrucao BEGIN"  << endl;
                }

                else if (instruction == "END") {
                    flag_end += 1;

                    // retirar aqui em baixo, tratamento de erro desnecessario
                    if (flag_end >= 2)
                        cout << "ERROR: (Erro Sintatico) na linha " << lineCounter << ". Mais de uma Instrucao END"  << endl;
                }

                else if (instruction == "EXTERN") {
                    if (lineVec.size() < 2 && lab_extern == "") {
                        cout << "ERROR: (Erro Sintatico) na linha " << lineCounter << ". Instrucao EXTERN nao possui argumento"  << endl;

                        // faltou label -> criar um label "de erro"
                        no_major_errors_flag = false;
                        lineVec.push_back( "ERRO_FALTA_DE_LABEL_EXTERN" );
                    }
                    string arg;
                    if(lab_extern == ""){
                        arg = lineVec[1];
                    }
                    else{
                        arg = lab_extern;
                        lab_extern = "";
                    }
                    tabUso[arg] = vector<int>();
                    tabSimb[arg] = -1; // label externo
                    flag_extern = 1;

                    if(!flag_begin)
                        printf("WARNING: (Erro Semantico) na linha %d, Uso de EXTERN sem BEGIN\n", lineCounter);
                }

                else if (instruction == "PUBLIC") {
                    if (lineVec.size() < 2) {
                        cout << "ERROR: (Erro Sintatico) na linha " << lineCounter << ". Instrucao PUBLIC nao possui argumento"  << endl;

                        // faltou label
                        no_major_errors_flag = false;
                        lineVec.push_back( "ERRO_FALTA_DE_LABEL_PUBLIC" );
                    }

                    string arg = lineVec[1];
                    // inicializando todos os labels externos com o contador de linha caso precise
                    tabDef[arg] = lineCounter;
                    flag_public = 1;

                    if(!flag_begin)
                        printf("WARNING: (Erro Semantico) na linha %d, Uso de PUBLIC sem BEGIN\n", lineCounter);
                }

                else if(instruction == "SECTION"){

                    if(lineVec[1] == "TEXT"){
                        flag_text = 1;
                    }

                    else if(lineVec[1] == "DATA"){
                        flag_data = 1;
                    }
                }

            }

            // É uma instrução do assembly
            else {

                if(!flag_text){
                    printf("ERROR: (Erro Semântico) na linha %d. Sem SECTION TEXT\n", lineCounter);
                    flag_text = 1;
                }

                // coloca o opcode na memória
                mem.push_back(tabInstr[instruction].first);
                fator_offset.push_back(0);
                addressCounter += 1;
                int argSize = tabInstr[instruction].second;
                int argumentos = 0;

                // adiciona os argumentos dessa linha na lista de pendencias
                for(int i=1; i<(int)lineVec.size(); i++) {
                    string arg = lineVec[i];

                    // É um deslocamento numérico ao label
                    if(lineVec[i-1] == "+" and isNum(arg)){
                        fator_offset[addressCounter-1] = stoi(arg);
                    }

                    // É um label
                    else if(lineVec[i] != "+"){

                        //Analisador Léxico
                        scanner(arg, lineCounter);

                        // caso o vetor de pendencias desse label nao esteja inicializado
                        if (!tabPend.count(arg)) tabPend[arg] = vector<pair<int, int>>();

                        tabPend[arg].push_back({addressCounter, lineCounter});

                        // qualquer espaço de memória que não guarda opcode de alguma instrução
                        relativos.push_back(addressCounter);

                        // adiciona pendencia
                        mem.push_back(-1);
                        fator_offset.push_back(0);
                        addressCounter += 1;

                        if(argumentos == argSize){
                            break;
                        }
                        argumentos += 1;
                    }
                }
            }
            lineCounter++;
        }
    }

    // Verifica se nao ha END, mesmo tendo BEGIN, EXTERN ou PUBLIC
    if(!flag_end)
        if (flag_begin or flag_extern or flag_public)
            printf("WARNING: (Erro Semantico) na linha %d, sem END\n", lineCounter);

    // Arruma todas as pendências da tabUso e tabPend
    for(auto [lab, vec] : tabPend) {
        for(auto [addressIdx, lineIdx] : vec) {
            // rotulo nunca definido nem internamente (>= 0), nem externamente (== -1)
            if (!tabSimb.count(lab)) {
                if (tipoDado.count(lab))
                    printf("ERROR: (Erro Semantico) na linha %d. Rotulo de Dado nao definido.\n", lineIdx);
                else
                    printf("ERROR: (Erro Semantico) na linha %d. Rotulo de Endereço nao definido.\n", lineIdx);

                no_major_errors_flag = false;
                // gerar o valor de 0 então para esse rótulo para poder compilar
                tabSimb[lab] = 0;
            }

            // rotulo externo
            if (tabSimb[lab] == -1) {
                tabUso[lab].push_back(addressIdx);
                mem[addressIdx] = fator_offset[addressIdx];;
            }

            // rotulo interno
            else {
                mem[addressIdx] = tabSimb[lab] + fator_offset[addressIdx];
            }
        }
    }

    // Configurando os endereços da tabela de definiçoes
    for(auto [lab, lineIdx] : tabDef) {

        if (!tabSimb.count(lab)) {
            if (tipoDado.count(lab))
                printf("ERROR: (Erro Semantico) na linha %d. Rotulo de Dado nao definido.\n", lineIdx);
            else
                printf("ERROR: (Erro Semantico) na linha %d. Rotulo de Endereço nao definido.\n", lineIdx);

            no_major_errors_flag = false;
            // gerar o valor de 0 então para esse rótulo para poder compilar
            tabSimb[lab] = 0;
        }

        tabDef[lab] = tabSimb[lab];
    }

    // string de todo o arquivo de saída
    string ans = "";

    // arquivo de saída em .obj que vai ser ligado
    if(flag_extern || flag_public || flag_begin || flag_end){
        outFile.open(filename + ".obj", ios::out | ios::trunc);

        ans += "USO\n";
        for(auto [lab, vec] : tabUso){
            for(auto endereco : vec){
                ans += lab + " " + to_string(endereco) + "\n";
            }
        }

        ans += "DEF\n";
        for(auto [lab, endereco] : tabDef){
            ans += lab + " " + to_string(endereco) + "\n";
        }

        ans += "RELATIVOS\n";
        for(auto endereco : relativos){
            ans+= to_string(endereco) + " ";
        }
        ans+="\nCODE\n";
    }
    // arquivo de saída em .exc que não vai ser ligado
    else {
        outFile.open(filename + ".exc", ios::out | ios::trunc);
    }

    // código binario
    for(auto i : mem) ans += to_string(i) + " ";

    // escrevendo no arquivo de saída
    if (outFile.is_open()) {
        outFile << ans << '\n';
    }

    auxFile.close();
    outFile.close();

    // nao houve problemas para montar
    return no_major_errors_flag;
}

int32_t main(int argc, char** argv) {

    // inicializa a tabela de instruções e a de diretivas
    init_fixed_tables();
    vector<string> files;

    // adquirindo o nome dos arquivos passados como argumento no terminal,
    // preprocessando e montando esses arquivos
    for(int i=1; i<argc; i++) {
        string filename = argv[i];
        files.push_back(filename);
        cout << "Preprocessando Arquivo {" << filename << "} ..." << endl;
        preprocess(filename);

        cout << "Montando Arquivo {" << filename << "} ..." << endl;

        if ( !assemble(filename) )
            cout << "Arquivo {" << filename << "} foi montado com erros :-( " << endl;
        else
            cout << "Arquivo {" << filename << "} foi montado sem erros :-) " << endl;
        
        cout << endl;
    }
}
