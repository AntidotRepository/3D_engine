#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

static void *thread_userEvent(void *p_data)
{
    while(1)
    {
        printf("thread event\n");
    }
}

static void *thread_3DWorld(void *p_data)
{
    while(1)
    {
        printf("thread 3D world\n");
    }
}

static void *thread_3Dto2DWorld(void *p_data)
{
    while(1)
    {
        printf("thread 3D to 2D world\n");
    }
}

static void *thread_fillTriangles(void *p_data)
{
    while(1)
    {
        printf("thread fill triangle\n");
    }
}

static void *thread_painter(void *p_data)
{
    while(1)
    {
        printf("thread painter\n");
    }
}

int main(int argc, char **argv)
{
    SDL_Window *window = NULL;
    SDL_Event event;
    bool end;
    int8_t ret = 0;
    pthread_t threadT_userEvent;
    pthread_t threadT_3DWorld;
    pthread_t threadT_3Dto2DWorld;
    pthread_t threadT_fillTriangles;
    pthread_t threadT_painter;
    
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
    
    // Threads creation
    // Creation of user events thread
    ret = pthread_create(&threadT_userEvent, NULL, thread_userEvent, NULL);
    // Creation of 3D world thread
    ret = pthread_create(&threadT_3DWorld, NULL, thread_3DWorld, NULL);
    // Creation of 3D to 2D world thread
    ret = pthread_create(&threadT_3Dto2DWorld, NULL, thread_3Dto2DWorld, NULL);
    // Creation of fill triangles thread
    ret = pthread_create(&threadT_fillTriangles, NULL, thread_fillTriangles, NULL);
    // Creation of painter thread
    ret = pthread_create(&threadT_painter, NULL, thread_painter, NULL);
    
    if(ret == 0)
    {
        printf("thread creation successfull\n");
    }
    else
    {
        printf("thread creation failed\n");
        end = false;
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
