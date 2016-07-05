#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH            640                                             // Window width
#define SCREEN_HEIGHT           480                                             // Window heigh
#define FRAME_RATE              10
#define DEG_TO_RAD              0.01745329

#define CONVERT_POS_X(pos_X)    pos_X+SCREEN_WIDTH/2
#define CONVERT_POS_Y(pos_Y)    pos_Y+SCREEN_HEIGHT/2

#define USER_DISTANCE           300

typedef struct mPoint mPoint;
struct mPoint
{
    float x_3D;
    float y_3D;
    float z_3D;
    float x_2D;
    float y_2D;
    char display;
    float distance_to_eye;
};

//#error http://www.willusher.io/sdl2%20tutorials/2013/08/18/lesson-3-sdl-extension-libraries

/**
 *  \fn         drawTriangle
 *  \brief      Draw a triangle
 *  \param[in]  Coordinates of points of triangle
 */
void draw2DTriangle(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, int x3, int y3)
{
    int xL, xC, xR, yL, yC, yR = 0;
    //printf("x1=%d, y1=%d, x2=%d, y2=%d, x3=%d, y3=%d\n", x1, y1, x2, y2, x3, y3);
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
            yR = y1;
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
         //printf("x=%d, y=%d, x=%d, y=%d\n", i+xL, y1, i+xL, y2);
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
    //int tabPointRotY[i].x_2D[8],tabPointRotY[i].y_2D[8];
    int angle = 0;
    float zBuffer[SCREEN_WIDTH][SCREEN_HEIGHT] = {{0}, {0}};
    
    mPoint tabPoint[8];
    mPoint tabPointRotX[8];
    mPoint tabPointRotY[8];
    mPoint tabPointRotZ[8];
    
    tabPoint[0].x_3D =  50;
    tabPoint[0].y_3D =  50;
    tabPoint[0].z_3D = -50;
    
    tabPoint[1].x_3D = -50;
    tabPoint[1].y_3D =  50;
    tabPoint[1].z_3D = -50;
    
    tabPoint[2].x_3D = -50;
    tabPoint[2].y_3D = -50;
    tabPoint[2].z_3D = -50;
    
    tabPoint[3].x_3D =  50;
    tabPoint[3].y_3D = -50;
    tabPoint[3].z_3D = -50;
    
    tabPoint[4].x_3D =  50;
    tabPoint[4].y_3D =  50;
    tabPoint[4].z_3D =  50;
    
    tabPoint[5].x_3D = -50;
    tabPoint[5].y_3D =  50;
    tabPoint[5].z_3D =  50;
    
    tabPoint[6].x_3D = -50;
    tabPoint[6].y_3D = -50;
    tabPoint[6].z_3D =  50;
    
    tabPoint[7].x_3D =  50;
    tabPoint[7].y_3D = -50;
    tabPoint[7].z_3D =  50;
    
    
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
    
    while(1)
    {
        angle += 1;
        
        for(int i = 0; i<8; i++)
        {
            tabPointRotX[i].x_3D = tabPoint[i].x_3D*cos(angle%360*DEG_TO_RAD)-tabPoint[i].y_3D*sin(angle%360*DEG_TO_RAD);
            tabPointRotX[i].y_3D = tabPoint[i].x_3D*sin(angle%360*DEG_TO_RAD)+tabPoint[i].y_3D*cos(angle%360*DEG_TO_RAD);
            tabPointRotX[i].z_3D = tabPoint[i].z_3D;
        }
        
        for(int i = 0; i<8; i++)
        {
            tabPointRotY[i].x_3D = tabPointRotX[i].x_3D;
            tabPointRotY[i].y_3D = tabPointRotX[i].y_3D*cos(angle%360*DEG_TO_RAD)-tabPointRotX[i].z_3D*sin(angle%360*DEG_TO_RAD);
            tabPointRotY[i].z_3D = tabPointRotX[i].y_3D*sin(angle%360*DEG_TO_RAD)+tabPointRotX[i].z_3D*cos(angle%360*DEG_TO_RAD);
        }
        
        for(int i = 0; i<8; i++)
        {
            tabPointRotY[i].distance_to_eye = sqrtf(pow(tabPointRotY[i].x_3D, 2)+pow(USER_DISTANCE - tabPointRotY[i].y_3D, 2)+pow(tabPointRotY[i].z_3D, 2));
            tabPointRotY[i].x_2D = CONVERT_POS_X((USER_DISTANCE*tabPointRotY[i].x_3D)/(USER_DISTANCE+tabPointRotY[i].z_3D));
            tabPointRotY[i].y_2D = CONVERT_POS_Y((USER_DISTANCE*tabPointRotY[i].y_3D)/(USER_DISTANCE+tabPointRotY[i].z_3D));
        }
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);                        // We set the background color (white)
        SDL_RenderClear(ren);
        
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);                              // We will draw in black
        
        // dessin des contours de l'écran
        SDL_RenderDrawLine(ren, 0, 0, SCREEN_WIDTH-1, 0);
        SDL_RenderDrawLine(ren, SCREEN_WIDTH-1, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1);
        SDL_RenderDrawLine(ren, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, 0, SCREEN_HEIGHT-1);
        SDL_RenderDrawLine(ren, 0, SCREEN_HEIGHT-1, 0, 0);
        
        draw2DTriangle(ren, tabPointRotY[0].x_2D, tabPointRotY[0].y_2D, tabPointRotY[1].x_2D, tabPointRotY[1].y_2D, tabPointRotY[2].x_2D, tabPointRotY[2].y_2D);
        draw2DTriangle(ren, tabPointRotY[2].x_2D, tabPointRotY[2].y_2D, tabPointRotY[3].x_2D, tabPointRotY[3].y_2D, tabPointRotY[0].x_2D, tabPointRotY[0].y_2D);
        
        SDL_SetRenderDrawColor(ren, 50, 50, 50, 255);                              // We will draw in black
        draw2DTriangle(ren, tabPointRotY[4].x_2D, tabPointRotY[4].y_2D, tabPointRotY[5].x_2D, tabPointRotY[5].y_2D, tabPointRotY[6].x_2D, tabPointRotY[6].y_2D);
        draw2DTriangle(ren, tabPointRotY[6].x_2D, tabPointRotY[6].y_2D, tabPointRotY[7].x_2D, tabPointRotY[7].y_2D, tabPointRotY[4].x_2D, tabPointRotY[4].y_2D);
        
        SDL_SetRenderDrawColor(ren, 100, 100, 100, 255);                              // We will draw in black
        draw2DTriangle(ren, tabPointRotY[5].x_2D, tabPointRotY[5].y_2D, tabPointRotY[6].x_2D, tabPointRotY[6].y_2D, tabPointRotY[2].x_2D, tabPointRotY[2].y_2D);
        draw2DTriangle(ren, tabPointRotY[2].x_2D, tabPointRotY[2].y_2D, tabPointRotY[1].x_2D, tabPointRotY[1].y_2D, tabPointRotY[5].x_2D, tabPointRotY[5].y_2D);
        
        SDL_SetRenderDrawColor(ren, 150, 150, 150, 255);                              // We will draw in black
        draw2DTriangle(ren, tabPointRotY[3].x_2D, tabPointRotY[3].y_2D, tabPointRotY[2].x_2D, tabPointRotY[2].y_2D, tabPointRotY[6].x_2D, tabPointRotY[6].y_2D);
        draw2DTriangle(ren, tabPointRotY[3].x_2D, tabPointRotY[3].y_2D, tabPointRotY[6].x_2D, tabPointRotY[6].y_2D, tabPointRotY[7].x_2D, tabPointRotY[7].y_2D);
        
        SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);                              // We will draw in black
        draw2DTriangle(ren, tabPointRotY[3].x_2D, tabPointRotY[3].y_2D, tabPointRotY[0].x_2D, tabPointRotY[0].y_2D, tabPointRotY[7].x_2D, tabPointRotY[7].y_2D);
        draw2DTriangle(ren, tabPointRotY[0].x_2D, tabPointRotY[0].y_2D, tabPointRotY[4].x_2D, tabPointRotY[4].y_2D, tabPointRotY[7].x_2D, tabPointRotY[7].y_2D);
        
        SDL_SetRenderDrawColor(ren, 225, 225, 225, 255);                              // We will draw in black
        draw2DTriangle(ren, tabPointRotY[5].x_2D, tabPointRotY[5].y_2D, tabPointRotY[4].x_2D, tabPointRotY[4].y_2D, tabPointRotY[0].x_2D, tabPointRotY[0].y_2D);
        draw2DTriangle(ren, tabPointRotY[0].x_2D, tabPointRotY[0].y_2D, tabPointRotY[1].x_2D, tabPointRotY[1].y_2D, tabPointRotY[5].x_2D, tabPointRotY[5].y_2D);
        
        SDL_RenderPresent(ren);
        SDL_Delay(1000/FRAME_RATE);
    }
    
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();                                                                 // We leave SDL
    
    return 0;
}
