bseg at 0x10 
x1 bit ACC.0
x2 bit ACC.1
y1 bit ACC.2
y2 bit ACC.3

cseg at 0x00
	jmp START
START:	   
// r0 - mem
// r1 - res
	mov r0, #0
	mov P1, #0
LOOP:
	mov a, r0
	mov b, #8
	cjne a, b, LOOP_CONTINUE
	jmp LOOP_END
LOOP_CONTINUE:
	mov a, P1
	rl a
	mov P1, a
	mov a, r0
// z = (y1&!x1 | y2&x2)&(!y1 | x2);
	mov c, y1
	cpl c
	orl c, x2
	mov ACC.4, c // (!y1 | x2)
	mov c, x1
	cpl c
	anl c, y1
	mov ACC.5, c	// y1&!x1
	mov c, x2
	anl c, y2
	orl c, ACC.5
	anl c, ACC.4
	mov P1.0, c
	inc r0
	jmp LOOP
LOOP_END:
END