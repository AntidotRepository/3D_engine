//
//  event.c
//  3D_engine
//
//  Created by DUCEPT on 27/12/2016.
//  Copyright © 2016 DUCEPT. All rights reserved.
//

#include "event.h"

void UpdateEvents(Input *in)
{
    SDL_Event event;
    
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_KEYDOWN:
                in->key[event.key.keysym.sym] = 1;
                break;
            case SDL_KEYUP:
                in->key[event.key.keysym.sym] = 0;
                break;
            case SDL_MOUSEMOTION:
                in->mouseX = event.motion.x;
                in->mouseY = event.motion.y;
                in->mouseXRel = event.motion.xrel;
                in->mouseYRel = event.motion.yrel;
                break;
            case SDL_MOUSEBUTTONDOWN:
                in->mousebuttons[event.button.button] = 1;
                break;
            case SDL_MOUSEBUTTONUP:
                in->mousebuttons[event.button.button] = 0;
                break;
            case SDL_QUIT:
                in->quit = 1;
                break;
            default:
                break;
        }
    }
}
