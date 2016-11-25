#include <reg51.h>
typedef unsigned char uchar;

short Transform2DTo(uchar byte)
{
    int i = 0;
    int power2 = 1;
    short result = 0;
    char * ptr;
    char isNegative = byte & 0x80;
    char sotni, desyatki, edinici;
    byte &= 0x7f;
    result = byte;
    for( i; i < 7; ++i )
    {
        if(byte & 0x01)
        {
            result += 1000;
        }
        result >>= 1;
        byte >>= 1;
    }
    
    sotni = result / 100;
    desyatki = (result / 10) % 10;
    edinici = result % 10;
    
    ptr = (char*)&result;
    ptr[0] = sotni;
    if(isNegative)
    {
        ptr[0] |= 0x80;
    }
    ptr[1] = ( desyatki << 4 );
    ptr[1] |= edinici;
    
    return result;
}

uchar TransformFrom2D(uchar high,
                      uchar low)
{
    int i = 0;
    uchar trueResult = 0;
    char isNegative = high & 0x80;
    short result = 0;
    result = (high & 0x0f) * 100 + (low >> 4) * 10 + (low & 0x0f);
    for( i; i < 7; ++i )
    {
        result <<= 1;
        if( result >= 1000 )
        {
            trueResult++;
            result -= 1000;
        }
        if( i != 6 )
        {
            trueResult <<= 1;
        }
    }
    trueResult |= isNegative;
    return trueResult;
}

main()
{
    uchar res = TransformFrom2D(P1, P0);
    short res1 = Transform2DTo(res);
    char * ptr = &res1;
    P3 = *ptr;
    P2 = *(ptr+1);
    while(1);
}
