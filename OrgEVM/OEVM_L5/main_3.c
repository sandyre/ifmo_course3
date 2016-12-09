#include <reg51.h>
char bdata mem;
sbit x1=mem^0;
sbit x2=mem^1;
sbit x3=mem^2;
sbit x4=mem^3;
sbit z=P1^0;
sbit s=P2^0;
//	/(/x2x3)/(/(/x1x3)/(x2x4))
main()
{   
	for(mem=0;mem<16;mem++)
	{
		if(mem<8){
			P1<<=1;
			z = !(!x2&x3)&!(!(!x1&x3)&!(x2&x4));
		} else {
			P2<<=1;
			s = !(!x2&x3)&!(!(!x1&x3)&!(x2&x4));
		}
	}
	while(1);
}