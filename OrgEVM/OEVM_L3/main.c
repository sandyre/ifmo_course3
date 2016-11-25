#include <reg51.h>

void div(){
	unsigned short A;
  	unsigned long S;
  	char i;

  	S=P1; 
  	A=P3<<8 ;

  	for (i=0 ; i<8; i++ ){   
		S = S<<1;
		if (S-A>=0)
			S = S-A+1;
	}
	P2 = S;
}

void mul(){					  
	unsigned short handl = P1;
	unsigned char val2 = P2;
	unsigned short result = 0;
	int i = 0;
	for (i = 0; i< 8; i++){
		if (0x01&val2){
			result += handl;
		}
		handl<<=1;
		val2>>=1;
	}	
	P3 = *((char*)&result);
	//P0 = *((char*)&result+1);
}

main()
{
while(1){
	P1 = 0xAA;
	P2 = 0xAA;
	P3 = 0xCC; 
	P0 = 0x00;
	mul();
	P0 = 0xAA;
	div();
	}
  	//while(1);
}
