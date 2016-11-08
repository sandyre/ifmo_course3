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

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

struct Rectangle
{
    Rectangle(const SDL_Point& center,
              const int width,
              const int height)
    {
        m_points[0].x = center.x - width/2;
        m_points[0].y = center.y - height/2;
        
        m_points[1].x = center.x - width/2;
        m_points[1].y = center.y + height/2;
        
        m_points[2].x = center.x + width/2;
        m_points[2].y = center.y + height/2;
        
        m_points[3].x = center.x + width/2;
        m_points[3].y = center.y - height/2;
    }
    
    SDL_Point m_points[4];
};

int main(int argc, const char * argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * pWND = SDL_CreateWindow("HI_L4 var 2",
                                         SDL_WINDOWPOS_UNDEFINED,
                                         SDL_WINDOWPOS_UNDEFINED,
                                         WINDOW_WIDTH,
                                         WINDOW_HEIGHT,
                                         0);
    
    SDL_Renderer * pRenderer = SDL_CreateRenderer(pWND, -1, 0);
    
    SDL_Point windowCenter;
    windowCenter.x = WINDOW_WIDTH/2;
    windowCenter.y = WINDOW_HEIGHT/2;
//    Rectangle rect(windowCenter, 50, 50);
    
    SDL_Rect rect;
    rect.x = windowCenter.x;
    rect.y = windowCenter.y;
    rect.w = 40;
    rect.h = 40;
    
    bool bQuit = false;
    SDL_Event event;
    while(!bQuit)
    {
        SDL_RenderClear(pRenderer);
        
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                bQuit = true;
            }
            
            if(event.type == SDL_KEYDOWN)
            {
                if(event.key.keysym.scancode == SDL_SCANCODE_LEFT)
                {
                    rect.x -= 2;
//                    rect.m_points[0].x -= 2;
//                    rect.m_points[1].x -= 2;
//                    rect.m_points[2].x -= 2;
//                    rect.m_points[3].x -= 2;
                }
                else if(event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
                {
                    rect.x += 2;
//                    rect.m_points[0].x += 2;
//                    rect.m_points[1].x += 2;
//                    rect.m_points[2].x += 2;
//                    rect.m_points[3].x += 2;
                }
            }
        }
        
        SDL_RenderDrawRect(pRenderer, &rect);
        SDL_RenderPresent(pRenderer);
    }
    
    SDL_DestroyWindow(pWND);
    
    SDL_Quit();
    return 0;
}
