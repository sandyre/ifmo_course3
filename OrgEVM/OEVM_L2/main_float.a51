XSEG AT 0
    result: DS 1

CSEG AT 0
    jmp START
    input: DB 0xC0 

START:
    clr a
// Transform from 2-d
// r0 - sign true/false
// r1 - low
// r2 - high
// r3 - result low
// r4 - result high
    mov r2, P1    // high
    mov r1, P0    // low
    mov a, r2
    anl a, #0x80
    mov r0, a    
//result = (high & 0x0f) * 100 + 
//         ((low & 0xf0) >> 4) * 10 +
//         (low & 0x0f);
    mov a, r2
    anl a, #0x0F
    mov b, #0x64
    mul ab
    mov r4, b // r4 = sotni
    mov r3, a
    mov a, r1
    anl a, #0xF0
    rr a
    rr a
    rr a
    rr a
    mov b, #0x0A
    mul ab
    add a, r3
    mov r3, a // r3 = sotni + desyatki
    mov a, r1
    anl a, #0x0F
    add a, r3
    mov r3, a // r4 and r3 = number
// if(negative != 0) result *= -1;
   //mov a, r4
   //orl a, r0
   //mov r4, a
//for( i; i < 7; ++i )
//   {
//        result <<= 1;
//        if( result >= 1000 )
//        {
//            trueResult++;
//            result -= 1000;
//        }
//        if( i != 6 )
//        {
//            trueResult <<= 1;
//        }
//    }
//    trueResult |= isNegative;
//    return trueResult;
	mov r1, #0	//result
	mov r5, #7 	//counter
// comparing r4 with 3 and r3 with E8
FROM2D_MAIN_LOOP:
//result <<= 1;	
	mov a, r4
	rlc a
	mov r4, a
	mov a, r3
	rlc a
	mov r3, a
	mov a, r4
	jnc FROM2D_NOT_C
		mov a, r4
		inc a
		mov r4, a
FROM2D_NOT_C:
//        if( result >= 1000 )
	mov a, r4
	mov b, #2
	subb a, b
	jc FROM2D_NOT_GREATER_THAN_1000
FROM2D_GREATER_THAN_1000_1:	
	mov a, r3
	mov b, #0xE7
	subb a, b
	jc FROM2D_NOT_GREATER_THAN_1000
FROM2D_GREATER_THAN_1000_2:
//	trueResult++;
	inc r1	   
//	result -= 1000;	
	mov a, r4
	mov b, #3
	subb a, b
	mov r4, a
	mov a, r3
	mov b, #0xE8
	subb a, b
	mov r3, a
FROM2D_NOT_GREATER_THAN_1000:
//        if( i != 6 )
//        {
//            trueResult <<= 1;
//        }
	mov a, r5
	mov b, #0
	subb a, b
	jnz FROM2D_NOT_EQU_6
	ajmp FROM2D_EQU_6
FROM2D_NOT_EQU_6:
	mov a, r1
	rl a
	mov r1, a
	dec r5
	ajmp FROM2D_MAIN_LOOP 
FROM2D_EQU_6:
FROM2D_MAIN_LOOP_END:
// set sign
	mov a, r1
	mov b, r0
	orl a, b
	mov r1, a 
// write to dataseg
    mov DPTR, #result
    mov a, r1
    movx @DPTR, a
	nop
	nop
	nop
SECOND_PART:
//byte &= 0x7f;
//    result = byte;
//    for( i; i < 7; ++i )
//    {
//        if(byte & 0x01)
//        {
//            result += 1000;
//        }
//        result >>= 1;
//        byte >>= 1;
//    }
//--------------
// load byte
//char isNegative = byte & 0x80;
	clr a
    mov DPTR, #input
    movc a, @a+DPTR
    mov r1, a
	anl a, #0x80
	mov r0, a
//byte &= 0x7f;
	mov a, r1
	anl a, #0x7F
	mov r1, a
//    for( i; i < 7; ++i )
	mov r5, #6
	mov r4, #0
	mov r3, #0
TO2D_MAIN_LOOP:
	mov a, r1
	anl a, #0x01
	jz TO2D_NOT_1
//if(byte & 0x01)
//{
//    result += 1000;
//}
	mov a, r3
	add a, #0xE8
	mov r3, a
	mov a, r4
	addc a, #0x03
	mov r4, a
TO2D_NOT_1:
//result >>= 1;
//byte >>= 1;	
	mov a, r3
	rr a
	mov r3, a
	mov a, r4
	rrc a
	mov r4, a
	jnc TO2D_ROR_HAVE_NO_1
	mov a, r3
	mov b, #0x80
	add a, b
	mov r3, a
TO2D_ROR_HAVE_NO_1:
	mov a, r1
	rr a
	mov r1, a
//check for end loop
	mov a, r5
	mov b, #0
	subb a, b
	jnz TO2D_MAIN_LOOP_AGAIN
	ajmp TO2D_MAIN_LOOP_END
TO2D_MAIN_LOOP_AGAIN:
	dec r5
	ajmp TO2D_MAIN_LOOP
TO2D_MAIN_LOOP_END:
// Transform to 2-d
// r0 - sign
// r1 - low
// r2 - high
// r3 - result low
// r4 - result high
// r5 - sotney
// r6 - desyatkey
// r7 - edenecey
	mov a, r4
	mov r2, a
	mov a, r3
	mov r1, a
 // sign detected already
// devisioning 1C8	XYZ
    mov a, r2  // mov SX to A
    anl a, #0x0F // 0X
    rl a
    rl a
    rl a
    rl a
    mov r5, a // save X0
    mov a, r1 // mov A YZ
    anl a, #0xF0 // Y0
    rr a
    rr a
    rr a
    rr a
    add a, r5 // 1C
    mov b, #0x0A
    div ab
    mov r7, b // 8 to r7
    mov r5, a // 2 to r5
    mov a, b  // A = 08
    rl a
    rl a
    rl a
    rl a
    mov b, r1 // C8
    anl b, #0x0F // 08
    add a, b	// 88
    mov b, #0x0A
    div ab // A = D B = 6
	mov r7, b
	mov b, a // save 0D to B
	mov a, r5
	rl a
	rl a
	rl a
	rl a // 20
	add a, b // 2D
	mov b, #0x0A
	div ab
	mov r5, a
	mov r6, b
// constructing the output
	mov a, r5
	orl a, r0
	mov r4, a
	mov a, r6
	rl a
	rl a
	rl a
	rl a
	orl a, r7
	mov r3, a
// send it to the ports
	mov P2, r3
	mov P3, r4
END
