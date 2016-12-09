#include "reg51.h"

main(){
	float x,y;
	for (x=0;x<1;x+=0.01){	 
		P2 = 0;
		y = (1+x)/((1-x)*2);
		P2 = 0x1F;
		P3 = 100*y;
	}
	while(1);
}