COPY ZERO OLDER 
COPY ONE LIMIT + 2 
COPY ONE OLD 
INPUT LIMIT + 2 
OUTPUT OLD 
FRONT: LOAD OLDER 
ADD OLD 
STORE NEW 
SUB LIMIT + 2 
JMPP FINAL 
OUTPUT NEW 
COPY OLD OLDER 
COPY NEW OLD 
JMP FRONT 
FINAL: OUTPUT LIMIT + 2 
STOP 
ZERO: CONST 0 
ONE: CONST 1 
OLDER: SPACE 
OLD: SPACE 
NEW: SPACE 
LIMIT: SPACE 4 
