//
//  event.h
//  3D_engine
//
//  Created by DUCEPT on 27/12/2016.
//  Copyright © 2016 DUCEPT. All rights reserved.
//

#ifndef event_h
#define event_h

#include <stdio.h>
#include <SDL2/SDL.h>

typedef struct Input Input;
struct Input
{
    char key[SDL_NUM_SCANCODES];
    int mouseX;
    int mouseY;
    int mouseXRel;
    int mouseYRel;
    char mousebuttons[8];
    char quit;
};

void UpdateEvents(Input *in);

#endif /* event_h */
