aa equ r1
bb equ r2
cc equ r3
dd equ r4
i equ r0

cseg at 0x00
	jmp START
START:	   
// r0 - counter
// r1 - aa
// r2 - bb
// r3 - cc
// r4 - dd
	mov i, #0
	mov P1, #0
LOOP:
	mov a, i
	mov b, #8
	cjne a, b, LOOP_CONTINUE
	jmp LOOP_END
LOOP_CONTINUE:
	mov a, P1
	rl a
	mov P1, a
// aa = i - 4
// bb = aa+1;
// dd = bb;
// cc = bb+1;
	mov a, i
	subb a, #4
	mov aa, a
	inc a
	mov bb, a
	mov cc, a
	inc a
	mov dd, a
// S = (aa!=bb || cc==dd) && (bb<dd);
// (bb<dd)
	clr c 
	mov a, dd
	dec a
	mov b, bb
	subb a, b // dd-bb
	jnc M1
	jmp SET_FALSE
M1:
	mov a, aa
	mov b, bb
	cjne a, b, SET_TRUE
M2: 
	mov a, cc
	mov b, dd
	cjne a, b, SET_FALSE
SET_TRUE:
	mov a, P1
	inc a
	mov P1, a
SET_FALSE:
	jmp LOOP
LOOP_END:
END