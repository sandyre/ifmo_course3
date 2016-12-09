#include "reg51.h"

main(){
	unsigned int x,S;
	// 1/2 + x + x^2 + x^3;
	// 1/2 + x(1+x(1+x))
	// m/2 + x(m + x(m+x/m)/m)/m
	while(1){
	P3 = 0;
	for (x = 0; x<0xFF; x++){// from 3 to 4 then deltaX = 1
		P2 = 0;
		S = 0xFF + (x>>8);
		S = 0xFF + ((x*S)>>8);
		S= 0x7F + ((x*S)>>8);
		P2 = 0x1F;
		P3 = S;
	}
	}
}