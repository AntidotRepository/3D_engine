#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char **argv)
{
    SDL_Window *window = NULL;
    SDL_Event event;
    bool end;
    
    // Initialisation de la SDL
    
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Erreur lors de l'initialisation de la SDL\n");
        SDL_Quit();
        
        return -1;
    }
    
    window = SDL_CreateWindow("3D_engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    
    if(window == 0)
    {
        printf("Erreur lors de la création de la fenetre: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    while(!end)
    {
        SDL_WaitEvent(&event);
        if(event.window.event == SDL_WINDOWEVENT_CLOSE)
        {
            end = true;
        }
    }
    
    // On quitte la SDL
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
