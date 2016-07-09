#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH            640                                             // Window width
#define SCREEN_HEIGHT           480                                             // Window heigh
#define FRAME_RATE              50
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
void draw2DTriangle(SDL_Renderer *renderer, mPoint p1, mPoint p2, mPoint p3)
{
    mPoint Left, Center, Right;
    
    // We sort points from left to right
    if((p1.x_2D <= p2.x_2D) && (p1.x_2D <= p3.x_2D))
    {
        Left = p1;
        if(p2.x_2D < p3.x_2D)
        {
            Center = p2;
            Right = p3;
        }
        else
        {
            Center = p3;
            Right = p2;
        }
    }
    else if((p2.x_2D <= p3.x_2D) && (p2.x_2D <= p1.x_2D))
    {
        Left = p2;
        if(p1.x_2D < p3.x_2D)
        {
            Center = p1;
            Right = p3;
        }
        else
        {
            Center = p3;
            Right = p1;
        }
    }
    else
    {
        Left = p3;
        if(p2.x_2D < p1.x_2D)
        {
            Center = p2;
            Right = p1;
        }
        else
        {
            Center = p1;
            Right = p2;
        }
    }
    
    // We calculate coef of each side of the triangle
    float coef1 = 0.0, coef2 = 0.0, coef3 = 0.0;
    if(Center.x_2D != Left.x_2D)
        coef1 = ((float)Center.y_2D-(float)Left.y_2D)/((float)Center.x_2D-(float)Left.x_2D);
    if(Right.x_2D != Left.x_2D)
        coef2 = ((float)Right.y_2D-(float)Left.y_2D)/((float)Right.x_2D-(float)Left.x_2D);
    if(Right.x_2D != Center.x_2D)
        coef3 = ((float)Right.y_2D-(float)Center.y_2D)/((float)Right.x_2D-(float)Center.x_2D);
    
    // We start to fill it
    for (int i = 0; i<(Right.x_2D-Left.x_2D); i++)
    {
        int y1 = 0;
        int y2 = 0;
        int ytemp = 0;
        
        y1 = (int)((double)(coef2*i))+Left.y_2D;                                // Calculate the beginning of the line
        if(i <= Center.x_2D-Left.x_2D)
        {
            y2 = (int)((double)(coef1*i)+Left.y_2D);                            // Calculate the end of the line for the first part
        }
        else
        {
            y2 = (int)((double)(coef3*(i-Center.x_2D+Left.x_2D)+Center.y_2D));  // Calculate the end of the line for the second part
        }
        if(y1 >= y2)
        {
            ytemp = y1;
            y1 = y2;
            y2 = ytemp;
        }
        for(int j = y1; j<y2; j++)                                              // Draw the whole line
        {
            // ToDo
            // Calculer distance à Left
            //screen[i+(int)Left.x_2D][j].x_3D = ;
            // Calculer distance à Center
            // Calculer distance à Right
            // Appliquer la fraction de la distance à chaque point sur son poids
            //screen[i+Left.x_2D][j].distance_to_eye = 1.0;//sqrt(pow(,2));
            SDL_RenderDrawPoint(renderer, i+Left.x_2D, j);
        }
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
        
        draw2DTriangle(ren, tabPointRotY[0], tabPointRotY[1], tabPointRotY[2]);
        draw2DTriangle(ren, tabPointRotY[2], tabPointRotY[3], tabPointRotY[0]);
        
        SDL_SetRenderDrawColor(ren, 50, 50, 50, 255);                              // We will draw in black
        draw2DTriangle(ren, tabPointRotY[4], tabPointRotY[5], tabPointRotY[6]);
        draw2DTriangle(ren, tabPointRotY[6], tabPointRotY[7], tabPointRotY[4]);
        
        SDL_SetRenderDrawColor(ren, 100, 100, 100, 255);                              // We will draw in black
        draw2DTriangle(ren, tabPointRotY[5], tabPointRotY[6], tabPointRotY[2]);
        draw2DTriangle(ren, tabPointRotY[2], tabPointRotY[1], tabPointRotY[5]);
        
        SDL_SetRenderDrawColor(ren, 150, 150, 150, 255);                              // We will draw in black
        draw2DTriangle(ren, tabPointRotY[3], tabPointRotY[2], tabPointRotY[6]);
        draw2DTriangle(ren, tabPointRotY[3], tabPointRotY[6], tabPointRotY[7]);
        
        SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);                              // We will draw in black
        draw2DTriangle(ren, tabPointRotY[3], tabPointRotY[0], tabPointRotY[7]);
        draw2DTriangle(ren, tabPointRotY[0], tabPointRotY[4], tabPointRotY[7]);
        
        SDL_SetRenderDrawColor(ren, 225, 225, 225, 255);                              // We will draw in black
        draw2DTriangle(ren, tabPointRotY[5], tabPointRotY[4], tabPointRotY[0]);
        draw2DTriangle(ren, tabPointRotY[0], tabPointRotY[1], tabPointRotY[5]);
        
        SDL_RenderPresent(ren);
        SDL_Delay(1000/FRAME_RATE);
    }
    
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();                                                                 // We leave SDL
    
    return 0;
}
