//P2 = 0;
//S = 0xFF + (x>>8);
//S = 0xFF + ((x*S)>>8);
//S= 0x7F + ((x*S)>>8);
//P2 = 0x1F;
//P3 = S;

CSEG AT 0
START:
	mov r0, #0 // r0 = x;
LOOP:
	mov P2, #0
	mov a, #0xFF //S = 0xFF + (x>>8);
	mov b, r0
	mul ab
	mov a, b  
	add a, #0xFF //S = 0xFF + ((x*S)>>8);
	mov b, r0
	mul ab
	mov a, b  //((x*S)>>8);
	mov b, #0
	add a, #0x7F //S= 0x7F + ((x*S)>>8);
	mov P2, #0x1F
	mov P3, a
	inc r0
	cjne r0, #100, LOOP //jump to loop
	jmp START
END