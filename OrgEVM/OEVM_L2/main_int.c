#include <reg51.h>

short TransformTo2D(short val)
{
    char * ptr;
    char sotni, desyatki, edinici;
    short result = 0;
    char negative = 0;
	if (val < 0){
		negative = 1; 
        val *= -1;
	}
    sotni = val / 100;
    desyatki = (val / 10) % 10;
    edinici = val % 10;
    
    ptr = (char*)&result;
    ptr[0] = sotni;
    if(negative != 0)
    {
        ptr[0] |= 0x80;
    }
    ptr[1] = ( desyatki << 4 );
    ptr[1] |= edinici;
    
    return result;
}

short TransformFrom2D(unsigned char high, unsigned char low)
{
    char negative = (high & 0xf0);
    short result = 0;
    result = (high & 0x0f) * 100 + 
             ((low & 0xf0) >> 4) * 10 +
             (low & 0x0f);
    if(negative != 0) result *= -1;
    
    return result;
}

main()
{
    unsigned char high = P1;
    unsigned char low = P0;
    short result = TransformFrom2D(high, low);
    short result_output = TransformTo2D(-998);
    char * ptr = &result_output;
    P3 = *ptr;
    P2 = *(ptr+1);
    while(1)
    {
    }
}
