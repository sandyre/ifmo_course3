#include <reg51.h>
#include <stdio.h>
#include <string.h>

#define STRING "itmoetoteclassic"
#define STRINGLEN 16

char xdata output_string[STRINGLEN] = STRING;

main()
{
   char i = 0, j = 0, swapped = 0, tmp = 0;
   for( i = 0; i < STRINGLEN-1; ++i )
   {
       swapped = 0;
       for( j = 0; j < STRINGLEN-i-1; ++j)
       {
           if( output_string[j] > output_string[j+1] )
           {
               tmp = output_string[j];
               output_string[j] = output_string[j+1];
               output_string[j+1] = tmp;
               swapped = 1;
           }
       }
       
       if( !swapped ) break;
   }
   
   while(1);
}