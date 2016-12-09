#include "reg51.h"
main(){
	unsigned int x,S;
	// 1/2 + x + x^2 + x^3;
	// 1/2 + x(1+x(1+x))
	// m/2 + x(m + x(m+x/m)/m)/m
	while(1){
	for (x = 0; x<100; x++){// from 3 to 4 then deltaX = 1
		P2 = 0;
		S = 100 + x/100;
		S = 100 + x*S/100;
		S= 50 + x*S/100;
		P2 = 0x1F;
		P3 = S;
	}
	}
}