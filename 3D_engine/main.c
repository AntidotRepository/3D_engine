#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define FRAME_RATE              1
#define EVENT_REFRESH_RATE      1

pthread_cond_t cond_3DWorldReady;
pthread_mutex_t mut_3DWorldReady;

pthread_mutex_t mut_refreshEvent;
pthread_cond_t cond_refreshEvent;

static Uint32 callback_refreshEvent(Uint32 interval, void *parametre)
{
    printf("callback\n");
   // pthread_mutex_lock(&mut_refreshEvent);
    pthread_cond_signal(&cond_refreshEvent);
    return interval;
}

static void *thread_userEvent(void *p_data)
{
    printf("thread userevent created\n");
    while(1)
    {
        // We firstly lock the mutex
        pthread_mutex_lock(&mut_refreshEvent);
        pthread_cond_wait(&cond_refreshEvent, &mut_refreshEvent);
        printf("Refresh user event\n");
        pthread_mutex_unlock(&mut_refreshEvent);
    }
}

static void *thread_3DWorld(void *p_data)
{
    time_t prev_time = 0;
    time_t current_time = 0;
    
    while(1)
    {
        prev_time = current_time;
        current_time = time(NULL);
        if(current_time - prev_time >= 1/FRAME_RATE)
        {
            printf("thread 3D world\n");
            // We do what we have to do
            
            // We send the signal for the next step in the process: 3D to 2D world
            pthread_cond_signal(&cond_3DWorldReady);
        }
    }
}

static void *thread_3Dto2DWorld(void *p_data)
{
    while(1)
    {
        // We firstly lock the mutex
        pthread_mutex_lock(&mut_3DWorldReady);
        // We wait for the 3D_world thread having finish its calculations
        pthread_cond_wait(&cond_3DWorldReady, &mut_3DWorldReady);
        printf("thread 3D to 2D world\n");
        // We do what we have to do
        
        // We unlock the mutex
        pthread_mutex_unlock(&mut_3DWorldReady);
        // We send the signal for the next step of the process: filling triangles
        // pthread_cond_signal(...);
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
    pthread_t threadT_timer;
    pthread_t threadT_userEvent;
    pthread_t threadT_3DWorld;
    pthread_t threadT_3Dto2DWorld;
    pthread_t threadT_fillTriangles;
    pthread_t threadT_painter;
    
    pthread_mutex_init(&mut_3DWorldReady, NULL);
    pthread_cond_init(&cond_3DWorldReady, NULL);
    
    pthread_mutex_init(&mut_refreshEvent, NULL);
    pthread_cond_init(&cond_refreshEvent, NULL);
    
    
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
    uint32_t delay = 1000/EVENT_REFRESH_RATE;
    SDL_AddTimer(delay, callback_refreshEvent, delay);
    
    // Threads creation
    // Creation of user events thread
    ret = pthread_create(&threadT_userEvent, NULL, thread_userEvent, NULL);
    // Creation of 3D world thread
    ret = pthread_create(&threadT_3DWorld, NULL, thread_3DWorld, NULL);
    // Creation of 3D to 2D world thread
    ret = pthread_create(&threadT_3Dto2DWorld, NULL, thread_3Dto2DWorld, NULL);
    // Creation of fill triangles thread
    // ret = pthread_create(&threadT_fillTriangles, NULL, thread_fillTriangles, NULL);
    // Creation of painter thread
    // ret = pthread_create(&threadT_painter, NULL, thread_painter, NULL);
    
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
