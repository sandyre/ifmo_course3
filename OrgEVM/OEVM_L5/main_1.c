#include <reg51.h>
char bdata mem;
sbit x1=mem^0;
sbit x2=mem^1;
sbit y1=mem^2;
sbit y2=mem^3;
sbit z=P1^0;
main()
{   
	for(mem=0;mem<8;mem++)
	{
		P1<<=1;
		z = (y1&!x1 | y2&x2)&(!y1 | x2);
	}
	while(1);
}