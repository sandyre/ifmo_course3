$TITLE(LAB1_1)

XSEG AT 70
    SORTED_MSG: ds 50

CSEG AT 0
    JMP START
    ORIGINAL_MSG: DB 'itmoeneclassic', 00h

START:
    CLR A
    // copy string to XSEG
    // r0 is an address of xdata
    // r1 is a counter
    MOV r0, #SORTED_MSG
    MOV r1, #0
COPY_CICLE_ITERATION:
    MOV DPTR, #ORIGINAL_MSG
    MOV A, r1
    MOVC A, @A+DPTR
    JZ COPY_CICLE_END
    MOVX @r0, A
    INC r0
    INC r1
    AJMP COPY_CICLE_ITERATION
COPY_CICLE_END:
    // write EOL symbol to the end
    CLR A
    INC r0
    MOVX @r0, A
SORTING_START:
    // r0 stores result string address
    // r1 stores line length
    // r3 stores i
    // r4 stores j
    // r7 stores Swapped? flag
    MOV r0, #SORTED_MSG
    MOV r2, #0
    MOV r3, #0
    MOV r4, #0
    MOV r7, #0
SORTING_OUTER_CICLE_IF:
    // for( i = 0; i < STRINGLEN-1 )
    MOV A, r3 // A = i
    MOV B, r1
    DEC B // B = STRLEN-1
    CJNE A, B, SORTING_OUTER_CICLE_START
    AJMP SORTING_END
SORTING_OUTER_CICLE_START:
    MOV r7, #0 // Swapped = 0
SORTING_INNER_CICLE_IF:
    MOV A, r1
    SUBB A, r3
    DEC A
    MOV B, A // B = STRLEN-i-1
    MOV A, r4 // A = j
    CJNE A, B, SORTING_INNER_CICLE_START
    AJMP SORTING_OUTER_CICLE_END
SORTING_INNER_CICLE_START:
    // check if SWAP is needed
    MOV A, #SORTED_MSG
    ADD A, r4
    MOV r0, A
    MOVX A, @r0
    MOV B, A // LEFT char stored in B
    INC r0
    MOVX A, @r0 // RIGHT char stored in A
    MOV r5, A // r5 = A
    MOV r6, B // r6 = B
    SUBB A, B
    JNC SORTING_INNER_CICLE_END
SORTING_INNER_CICLE_SWAP_CHARS:
    MOV A, #SORTED_MSG
    ADD A, r4
    MOV r0, A
    MOV A, r5
    MOVX @r0, A
    INC r0
    MOV A, r6
    MOVX @r0, A
    MOV r7, #1
SORTING_INNER_CICLE_END:
    INC r4
    AJMP SORTING_INNER_CICLE_IF
SORTING_OUTER_CICLE_END:
    MOV A, r7
    JZ SORTING_END
    INC r3
    MOV r4, #0
    AJMP SORTING_OUTER_CICLE_IF
SORTING_END:
    MOV r0, #SORTED_MSG
    INC r0
    INC r0
    MOVX A, @r0
    NOP
    JMP $
END