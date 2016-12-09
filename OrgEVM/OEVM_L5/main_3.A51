bseg at 0x10 
x1 bit ACC.0
x2 bit ACC.1
x3 bit ACC.2
x4 bit ACC.3

cseg at 0x00
	jmp START
START:	   
// r0 - mem
	mov r0, #0
	mov P1, #0
	mov P2, #0
LOOP:
	mov a, r0
	mov b, #16
	cjne a, b, LOOP_CONTINUE
	jmp LOOP_END
LOOP_CONTINUE:
	mov a, r0
// z = !(!x2&x3)&!(!(!x1&x3)&!(x2&x4));	
	mov c, x2
	anl c, x4
	cpl c
	mov ACC.4, c // !(x2&x4))
	mov c, x2
	cpl c
	anl c, x3
	cpl c
	mov ACC.5, c // !(!x2&x3)
	mov c, x1
	cpl c
	anl c, x3
	cpl c		// !(!x1&x3)
	anl c, ACC.4
	cpl c	  // !(!(!x1&x3)&!(x2&x4))
	anl c, ACC.5
// -----------------
	mov P3.0, c
	mov a, r0
	mov b, #8
	subb a, b
	jnc LOOP_16
LOOP_8:
	mov a, P1
	rl a
	mov P1, a
	mov c, P3.0
	mov P1.0, c
	inc r0
	jmp LOOP
LOOP_16:
	mov a, P2
	rl a
	mov P2, a
	mov c, P3.0
	mov P2.0, c
	inc r0
	jmp LOOP
LOOP_END:
END