
SECTION TEXT
INPUT OLD_DATA
LOAD old_Data
L1: DIV DOIS
STORE  NEW_DATA
MUL DOIS
    STORE TMP_DATA ; tabulação
LOAD OLD_DATA ; comentario
SUB TMP_DATA
      STORE TMP_DATA ; espaços no começo
OUTPUT TMP_DATA
COPY NEW_DATA , OLD_DATA
LOAD           OLD_DATA ; espaços no meio
JMPP L1
STOP               ; espaços no fim

SECTION DATA
DOIS  :        CONST 2 ; tem que juntar ':' com o rótulo
OLD_DATA:SPACE
NEW_DATA: 
SPACE ; faz parte do label de cima
TMP_DATA: 

SPACE
