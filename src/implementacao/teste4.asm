BIN1: BEGIN
EXTERN: BIN2
EXTERN: OLD_DATA
PUBLIC L1
SECTION TEXT
JMP BIN2
L1: DIV DOIS
STORE  NEW_DATA
MUL DOIS
STORE TMP_DATA
LOAD OLD_DATA
SUB TMP_DATA
STORE TMP_DATA
OUTPUT TMP_DATA
COPY NEW_DATA , OLD_DATA
LOAD OLD_DATA
JMPP L1
STOP
SECTION DATA
DOIS: CONST 2
NEW_DATA: SPACE
TMP_DATA: SPACE