    //
    //  main.cpp
    //  HI_L2
    //
    //  Created by Aleksandr Borzikh on 23.10.16.
    //  Copyright © 2016 sandyre. All rights reserved.
    //

#include <SDL2/SDL.h>
#include <iostream>
#include <string>

int main(int argc, const char * argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    
    SDL_Surface * pBMP = SDL_LoadBMP(argv[1]);
    
    if( pBMP == nullptr )
    {
        std::cout << "Wrong path passed as an argument" << std::endl;
        
        SDL_Quit();
        return 1;
    }
    
    SDL_Window * pWND = SDL_CreateWindow("HI_L2 var 2",
                                         SDL_WINDOWPOS_UNDEFINED,
                                         SDL_WINDOWPOS_UNDEFINED,
                                         pBMP->clip_rect.w,
                                         pBMP->clip_rect.h,
                                         0);
    SDL_Renderer * pRenderer = SDL_CreateRenderer(pWND, -1, 0);
    
    SDL_LockSurface(pBMP);
    Uint8   tmp;
    Uint8 * pBaseAddress = (Uint8*)pBMP->pixels;
    Uint8 * pPixelAddr = pBaseAddress;
    for(auto i = 0; i < pBMP->clip_rect.w; ++i)
    {
        for(auto j = 0; j < pBMP->clip_rect.h; ++j)
        {
            pPixelAddr = pBaseAddress + j * pBMP->pitch + i * pBMP->format->BytesPerPixel;
            
                // Swap G and B
            tmp = pPixelAddr[0];
            pPixelAddr[0] = pPixelAddr[1];
            pPixelAddr[1] = tmp;
        }
    }
    SDL_UnlockSurface(pBMP);
    
        // Save resulting bmp
    std::string sFileName = argv[1];
    sFileName.resize(sFileName.size()-4);
    sFileName += "_mod.bmp";
    
    SDL_SaveBMP(pBMP, sFileName.c_str());
    
    SDL_Texture * pTexture = SDL_CreateTextureFromSurface(pRenderer, pBMP);
    
    while(true)
    {
        SDL_RenderCopy(pRenderer, pTexture, NULL, NULL);
        SDL_RenderPresent(pRenderer);
    }
    
    SDL_DestroyWindow(pWND);
    
    SDL_Quit();
    return 0;
}
