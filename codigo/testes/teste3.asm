; Teste 3 - Testando alguns erros mistos

EXTERN  ; Uso de Extern Sem Begin, 
        ; falta operando para o Extern

        ; falta de Section Text

INPUT &OLD_DATA ; Erro lexicos
LOAD OLD_DATA
DIV TRES ; label de dado não declarado
L1: L3: STORE  NEW_DATA ; Erro Sintatico -> mais de um label
MUL DOIS
STORE   ; falta argumento, 
LOAD OLD_DATA
SUB 3TMP_DATA ; erro lexico
STORE TMP_DATA
OUTPUT TMP_DATA
COPY NEW_DATA , OLD_DATA
LOAD OLD_DATA
JMPP L2 ; label de rótulo não declarado
STOP

SECTION DATA

DOIS: CONST 2
OLD_DATA: SPACE
NEW_DATA: SPACE
TMP_DATA: SPACE
