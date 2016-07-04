#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH    640                                                     // Window width
#define SCREEN_HEIGHT   480                                                     // Window heigh

//#error http://www.willusher.io/sdl2%20tutorials/2013/08/18/lesson-3-sdl-extension-libraries

/**
 *  \fn         drawTriangle
 *  \brief      Draw a triangle
 *  \param[in]  Coordinates of points of triangle
 */
void drawTriangle(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, int x3, int y3)
{
    int xL, xC, xR, yL, yC, yR = 0;
    
    // We sort points from left to right
    if((x1 <= x2) && (x1 <= x3))
    {
        xL = x1;
        yL = y1;
        if(x2 < x3)
        {
            xC = x2;
            yC = y2;
            xR = x3;
            yR = y3;
        }
        else
        {
            xC = x3;
            yC = y3;
            xR = x2;
            yR = y2;
        }
    }
    else if((x2 <= x3) && (x2 <= x1))
    {
        xL = x2;
        yL = y2;
        if(x1 < x3)
        {
            xC = x1;
            yC = y1;
            xR = x3;
            yR = y3;
        }
        else
        {
            xC = x3;
            yC = y3;
            xR = x1;
            yR = y1;
        }
    }
    else
    {
        xL = x3;
        yL = y3;
        if(x2 < x1)
        {
            xC = x2;
            yC = y2;
            xR = x1;
            yR = x1;
        }
        else
        {
            xC = x1;
            yC = y1;
            xR = x2;
            yR = y2;
        }
    }

    // We calculate coef of each side of the triangle
    float coef1 = ((float)yC-(float)yL)/((float)xC-(float)xL);
    float coef2 = ((float)yR-(float)yL)/((float)xR-(float)xL);
    float coef3 = ((float)yR-(float)yC)/((float)xR-(float)xC);
    
    // We start to fill it
     for (int i = 0; i<(xR-xL); i++)
     {
         int y1 = 0;
         int y2 = 0;
         
         y1 = (int)((double)(coef2*i))+yL;                                      // Calculate the beginning of the line
         if(i <= xC-xL)
         {
             y2 = (int)((double)(coef1*i)+yL);                                  // Calculate the end of the line for the first part
         }
         else
         {
             y2 = (int)((double)(coef3*(i-xC+xL)+yC));                          // Calculate the end of the line for the second part
         }
         SDL_RenderDrawLine(renderer, i+xL, y1, i+xL, y2);                      // Draw a line filling the triangle
     }
}

/**
 *  \fn     logSDLError
 *  \brief  Display a given message with the last error get by SDL
 *  \param  msg String to display with the error
 */
void logSDLError(char *msg)
{
    printf("%s error: %s\n", msg, SDL_GetError());
}

int main(int argc, char **argv)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)                                            // SDL Initialization
    {
        logSDLError("SDL_Init");
        SDL_Quit();
        
        return -1;
    }
    
    SDL_Window *win = SDL_CreateWindow("3D_engine", SDL_WINDOWPOS_UNDEFINED,    // Create a new window
                                       SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                       SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    
    if(win == 0)                                                                // If window has not been created
    {
        logSDLError("SDL_CreateWindow");
        SDL_Quit();
        return -1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED |  // Create a thing where we will draw
                                           SDL_RENDERER_PRESENTVSYNC);
    if(ren == NULL)                                                             // If renderer has not been created
    {
        SDL_DestroyWindow(win);
        logSDLError("SDL_CreateRenderer");
        SDL_Quit();
        return 1;
    }
    
    SDL_Surface *bmp = SDL_LoadBMP("res/image.bmp");                            // Load a surface
    if(bmp == NULL)                                                             // If surface has not been loaded
    {
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        logSDLError("SDL_LoadBMP");
        SDL_Quit();
        return 1;
    }
    
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);                            // We set the background color (white)
    SDL_RenderClear(ren);
    
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);                                  // We will draw in black
    
    for(int i = 0; i<5000/20; i++)
    {
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);                        // We set the background color (white)
        SDL_RenderClear(ren);
        
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);                              // We will draw in black
        
        // dessin des contours de l'écran
        SDL_RenderDrawLine(ren, 0, 0, SCREEN_WIDTH-1, 0);
        SDL_RenderDrawLine(ren, SCREEN_WIDTH-1, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1);
        SDL_RenderDrawLine(ren, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, 0, SCREEN_HEIGHT-1);
        SDL_RenderDrawLine(ren, 0, SCREEN_HEIGHT-1, 0, 0);
        
        drawTriangle(ren, 10, SCREEN_HEIGHT-10, SCREEN_WIDTH-10, SCREEN_HEIGHT-10, SCREEN_WIDTH/2, 10);
        SDL_RenderPresent(ren);
        SDL_Delay(20);
    }
    
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();                                                                 // We leave SDL
    
    return 0;
}
