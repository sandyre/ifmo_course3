//
//  main.cpp
//  HI_L3
//
//  Created by Aleksandr Borzikh on 07.11.16.
//  Copyright © 2016 sandyre. All rights reserved.
//
#include "bitmap.hpp"

int main(int argc, const char * argv[])
{
    Bitmap myBitmap;
    myBitmap.InitWithFile("/users/Aleksandr/Desktop/test_converted.bmp");
    
    for(auto i = 0; i < myBitmap.GetInfoHeader().nWidth; ++i)
    {
        for(auto j = 0; j < myBitmap.GetInfoHeader().nHeight; ++j)
        {
            RGBA_Pixel stPixel = myBitmap.GetPixelByIndex(i, j);
            
            myBitmap.SetPixelByIndex(i, j, stPixel);
        }
    }
    
    myBitmap.SaveToFile("/users/Aleksandr/Desktop/test_saved.bmp");
    
    return true;
}
