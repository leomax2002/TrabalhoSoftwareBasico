MOD_4:  BEGIN
N1:     EXTERN
N2:     EXTERN
N3:     EXTERN
FINISH: EXTERN
        PUBLIC MOD_4
        SECTION TEXT
        INPUT N4
        LOAD N1
        ADD N2
        ADD N3
        ADD N4
        STORE N4
        OUTPUT N4
        JMP FINISH
        SECTION DATA
N4:     SPACE
        END
