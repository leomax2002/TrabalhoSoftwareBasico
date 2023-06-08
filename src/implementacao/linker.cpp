#include <bits/stdc++.h>
using namespace std;

void linker(vector<string> objs){
    fstream outFile, auxFile;
    string line;

    // Tamanho Objs
    int tamanho_objs = (int) objs.size();

    // Flags
    int fator_correcao = 0;
    int proximo_uso = 0;
    int proximo_def = 0;
    int proximo_relativo = 0;
    int proximo_codigo = 0;

    //Variáveis Auxiliares
    string arg;
    string resp = "";
    vector<int> cod_ligado;
    vector<int> mem_aux;
    //Tabela Geral de Definições e Tabela de Uso
    unordered_map<string, int> tab_ger_def;
    unordered_map<string,vector<int>> tab_uso;
    vector<int> relativos;
    //Código

    //Arruma Tabelas
    for(int i = 0; i < tamanho_objs;i++){
        string obj = objs[i];
        relativos.clear();
        auxFile.open(obj+".obj",ios::in);

        if(auxFile.is_open()){
            while( getline(auxFile,line) ){
                stringstream ss(line);
                string word;
                vector<string> lineVec;
                while(ss >> word) lineVec.push_back(word);
            if(lineVec[0] == "USO") {
                       proximo_uso = 1;
                       proximo_def = 0;
                       proximo_relativo = 0;
                       proximo_codigo = 0;
            }
                if(lineVec[0] == "DEF") {
                       proximo_uso = 0;
                       proximo_def = 1;
                       proximo_relativo = 0;
                       proximo_codigo = 0;
                }
                if(lineVec[0] == "CODE"){
                       proximo_uso = 0;
                       proximo_def = 0;
                       proximo_relativo = 0;
                       proximo_codigo = 1;
                }
                if(lineVec[0] == "RELATIVOS"){
                    proximo_uso = 0;
                    proximo_def = 0;
                    proximo_relativo = 1;
                    proximo_codigo = 0;
                }
                if(proximo_uso && lineVec[0] != "USO" && lineVec[0] != "DEF" && lineVec[0] != "CODE"){
                   arg = lineVec[0];
                   int val_uso = stoi(lineVec[1]) + fator_correcao;
                   if (!tab_uso.count(arg)) tab_uso[arg] = vector<int>();
                   tab_uso[arg].push_back(val_uso);
            }
            if(proximo_def && lineVec[0] != "USO" && lineVec[0] != "DEF" && lineVec[0] != "CODE" && lineVec[0] != "RELATIVOS"){
                string arg = lineVec[0];
                int val_uso = stoi(lineVec[1]) + fator_correcao;
                tab_ger_def[arg] = val_uso;
            }

            if(proximo_relativo && lineVec[0] != "USO" && lineVec[0] != "DEF" && lineVec[0] != "CODE" && lineVec[0] != "RELATIVOS"){
                for(int i = 0; i < (int)lineVec.size();i++){
                    int id_rel = stoi(lineVec[i]);
                    relativos.push_back(id_rel);
                }

            }
            if(proximo_codigo && lineVec[0] != "USO" && lineVec[0] != "DEF" && lineVec[0] != "CODE" && lineVec[0] != "RELATIVOS"){
                    proximo_codigo = 0;
                    int contador_pos_cod = 0;
                    for(int i = 0; i < (int)lineVec.size(); i++){
                        int id = stoi(lineVec[i]);
                        mem_aux.push_back(id);
                        auto it = find(relativos.begin(), relativos.end(), contador_pos_cod);
                        if(it != relativos.end()) id+=fator_correcao;
                        cod_ligado.push_back(id);
                        contador_pos_cod++;
                    }
                    fator_correcao = (int) lineVec.size();
            }

            }

        }
        auxFile.close();
    }

    //Resolve relativos
    //for(auto [lab, vec])
    for(auto [lab, vec] : tab_uso) {

        for(auto idx : vec){

            cod_ligado[idx] = tab_ger_def[lab] + mem_aux[idx];
        }
}

    outFile.open(objs[0] + ".exc", ios::out | ios::trunc);
    for(auto i : cod_ligado) resp += to_string(i) + " ";

        if (outFile.is_open()) {
            outFile << resp << '\n';
        }

    outFile.close();
}

int32_t main(int argc, char** argv) {
    vector<string> files;

    for(int i=1; i<argc; i++) {
        string filename = argv[i];
        files.push_back(filename);
    }

    if(argc >= 3) {
        cout << "Mais de um arquivo passado como argumento" << endl;
        cout << "Ligando arquivos..." << endl;
        linker(files);
    }
}