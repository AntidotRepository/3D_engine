#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define FRAME_RATE              60
#define EVENT_REFRESH_RATE      60

pthread_mutex_t mut_refreshEvent;
pthread_cond_t cond_refreshEvent;

pthread_mutex_t mut_refresh3DWorld;
pthread_cond_t cond_refresh3DWorld;

pthread_cond_t cond_3DWorldReady;
pthread_mutex_t mut_3DWorldReady;

pthread_cond_t cond_2DWorldReady;
pthread_mutex_t mut_2DWorldReady;

pthread_cond_t cond_trianglesReadies;
pthread_mutex_t mut_trianglesReadies;

static Uint32 callback_refreshEvent(Uint32 interval, void *parametre)
{
    printf("callback refreshEvent\n");
   // pthread_mutex_lock(&mut_refreshEvent);
    pthread_cond_signal(&cond_refreshEvent);
    return interval;
}

static Uint32 callback_refresh3DWorld(Uint32 interval, void *parametre)
{
    printf("callback 3DWorld\n");
    pthread_cond_signal(&cond_refresh3DWorld);
    return interval;
}

static void *thread_userEvent(void *p_data)
{
    while(1)
    {
        // We firstly lock the mutex
        pthread_mutex_lock(&mut_refreshEvent);
        pthread_cond_wait(&cond_refreshEvent, &mut_refreshEvent);
        printf("thread user event\n");
        // We do what we have to do here
        // ...
        // Place the event in a queue to be sure we handle everything
        
        pthread_mutex_unlock(&mut_refreshEvent);
    }
}

static void *thread_3DWorld(void *p_data)
{
    while(1)
    {
        pthread_mutex_lock(&mut_refresh3DWorld);
        pthread_cond_wait(&cond_refresh3DWorld, &mut_refresh3DWorld);
        printf("thread 3D world\n");
        // We do what we have to do
        // ...
        // Do not place the result in a queue to not be late on rendering
        
        // We send the signal for the next step in the process: 3D to 2D world
        pthread_cond_signal(&cond_3DWorldReady);
        // We unlock the mutex
        pthread_mutex_unlock(&mut_refresh3DWorld);
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
        // ...
        // We send the signal for the next step of the process: filling triangles
        pthread_cond_signal(&cond_2DWorldReady);
        // We unlock the mutex
        pthread_mutex_unlock(&mut_3DWorldReady);
    }
}

static void *thread_fillTriangles(void *p_data)
{
    while(1)
    {
        // We firstly lock the mutex
        pthread_mutex_lock(&mut_2DWorldReady);
        // We wait for the 3D to 2D world thread having finished its calculations
        pthread_cond_wait(&cond_2DWorldReady, &mut_2DWorldReady);
        printf("thread fill triangle\n");
        // We do what we have to do
        // ...
        // We send the signal for the next step of the process: paint
        pthread_cond_signal(&cond_trianglesReadies);
        // We unlock the mutex
        pthread_mutex_unlock(&mut_2DWorldReady);
    }
}

static void *thread_painter(void *p_data)
{
    while(1)
    {
        // We firstly lock the mutex
        pthread_mutex_lock(&mut_trianglesReadies);
        // We wait for the fillTriangles thread having finished its calculations
        pthread_cond_wait(&cond_trianglesReadies, &mut_trianglesReadies);
        printf("thread painter\n");
        // We do what we have to do
        // ...
        // It's the last step of the process (yet). No signal to send.
        // pthread_cond_signal(...)
        // We unlock the mutex
        pthread_mutex_unlock(&mut_trianglesReadies);
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
    
    pthread_mutex_init(&mut_refreshEvent, NULL);
    pthread_cond_init(&cond_refreshEvent, NULL);
    
    pthread_mutex_init(&mut_refresh3DWorld, NULL);
    pthread_cond_init(&cond_refresh3DWorld, NULL);
    
    pthread_mutex_init(&mut_3DWorldReady, NULL);
    pthread_cond_init(&cond_3DWorldReady, NULL);
    
    pthread_mutex_init(&mut_2DWorldReady, NULL);
    pthread_cond_init(&cond_2DWorldReady, NULL);
    
    pthread_mutex_init(&mut_trianglesReadies, NULL);
    pthread_cond_init(&cond_trianglesReadies, NULL);
    
    
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
    
    SDL_AddTimer(1000/EVENT_REFRESH_RATE, callback_refreshEvent, NULL);
    SDL_AddTimer(1000/FRAME_RATE, callback_refresh3DWorld, NULL);
    
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
