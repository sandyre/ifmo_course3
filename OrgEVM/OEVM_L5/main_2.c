#include <reg51.h>
char aa;
char bb;
char cc;
char dd;
char S;
main()
{   
	for(aa=-4;aa<4;aa++)
	{
		bb = aa+1;
		cc = bb;
		dd = bb+1;
		P1<<=1;
		S = (aa!=bb || cc==dd) && (bb<dd);
		P1 += S;
	}
	while(1);
}