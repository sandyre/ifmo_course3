//
//  main.cpp
//  HI_L1
//
//  Created by Aleksandr Borzikh on 23.10.16.
//  Copyright © 2016 sandyre. All rights reserved.
//

#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string>
#include <chrono>

const std::array< std::string, 10 > g_abmpFilePaths
{
    "/Users/aleksandr/ifmo_course3/HI/HI_L1/data/Picture1.bmp",
    "/Users/aleksandr/ifmo_course3/HI/HI_L1/data/Picture2.bmp",
    "/Users/aleksandr/ifmo_course3/HI/HI_L1/data/Picture3.bmp",
    "/Users/aleksandr/ifmo_course3/HI/HI_L1/data/Picture4.bmp",
    "/Users/aleksandr/ifmo_course3/HI/HI_L1/data/Picture5.bmp",
    "/Users/aleksandr/ifmo_course3/HI/HI_L1/data/Picture6.bmp",
    "/Users/aleksandr/ifmo_course3/HI/HI_L1/data/Picture7.bmp",
    "/Users/aleksandr/ifmo_course3/HI/HI_L1/data/Picture8.bmp",
    "/Users/aleksandr/ifmo_course3/HI/HI_L1/data/Picture9.bmp",
    "/Users/aleksandr/ifmo_course3/HI/HI_L1/data/Picture10.bmp"
};

int main(int argc, const char * argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    
    std::string filePath = "";
    SDL_Surface * pBMP = nullptr;
    SDL_Window * pWND = SDL_CreateWindow("HI_L1 var 1",
                                         SDL_WINDOWPOS_UNDEFINED,
                                         SDL_WINDOWPOS_UNDEFINED,
                                         1000,
                                         1000,
                                         0);
    SDL_Renderer * pRenderer = SDL_CreateRenderer(pWND, -1, 0);
    
    for(auto& str : g_abmpFilePaths)
    {
        filePath = str;
        pBMP = SDL_LoadBMP(filePath.c_str());
        
        if( pBMP == nullptr )
        {
            std::cout << "BMP file opening error." << std::endl;
            
            SDL_Quit();
            return 1;
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        SDL_LockSurface(pBMP);
        Uint8 * pBaseAddress = (Uint8*)pBMP->pixels;
        Uint8 * pPixelAddr = pBaseAddress;
        for(auto i = 0; i < pBMP->clip_rect.w; ++i)
        {
            for(auto j = 0; j < pBMP->clip_rect.h; ++j)
            {
                pPixelAddr = pBaseAddress + j * pBMP->pitch + i * pBMP->format->BytesPerPixel;
                pPixelAddr[2] = 0x0;
            }
        }
        SDL_UnlockSurface(pBMP);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << duration.count() << std::endl;
        
            // Save resulting bmp
        std::string sFileName = filePath;
        sFileName.resize(sFileName.size()-4);
        sFileName += "_mod.bmp";
        
        SDL_SaveBMP(pBMP, sFileName.c_str());
        
        delete pBMP;
        pBMP = nullptr;
    }
    
    SDL_DestroyWindow(pWND);
    
    SDL_Quit();
    return 0;
}
