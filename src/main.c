#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH            640                                             // Window width
#define SCREEN_HEIGHT           480                                             // Window heigh
#define FRAME_RATE              20
#define DEG_TO_RAD              0.01745329

#define CONVERT_POS_X(pos_X)    pos_X+SCREEN_WIDTH/2
#define CONVERT_POS_Y(pos_Y)    pos_Y+SCREEN_HEIGHT/2

#define USER_DISTANCE           300

typedef struct m3DPoint m3DPoint;
struct m3DPoint
{
    float x_3D;
    float y_3D;
    float z_3D;
    float x_2D;
    float y_2D;
    char display;
    float depth;
};

typedef struct plan plan;
struct plan
{
    m3DPoint *pt1;
    m3DPoint *pt2;
    m3DPoint *pt3;
    float coefA;
    float coefB;
    float coefC;
    float coefD;
    // Add normal vector
};

/**
 *  \fn         drawTriangle
 *  \brief      Draw a triangle
 *  \param[in]
 */
void draw2DTriangle(m3DPoint *canevas, int w, int h, plan *p)
{
    m3DPoint *Left, *Center, *Right;
    int i = 0;
    int j = 0;
    
    // We sort points from left to right
    if((p->pt1->x_2D <= p->pt2->x_2D) && (p->pt1->x_2D <= p->pt3->x_2D))
    {
        Left = p->pt1;
        if(p->pt2->x_2D < p->pt3->x_2D)
        {
            Center = p->pt2;
            Right = p->pt3;
        }
        else
        {
            Center = p->pt3;
            Right = p->pt2;
        }
    }
    else if((p->pt2->x_2D <= p->pt3->x_2D) && (p->pt2->x_2D <= p->pt1->x_2D))
    {
        Left = p->pt2;
        if(p->pt1->x_2D < p->pt3->x_2D)
        {
            Center = p->pt1;
            Right = p->pt3;
        }
        else
        {
            Center = p->pt3;
            Right = p->pt1;
        }
    }
    else
    {
        Left = p->pt3;
        if(p->pt2->x_2D < p->pt1->x_2D)
        {
            Center = p->pt2;
            Right = p->pt1;
        }
        else
        {
            Center = p->pt1;
            Right = p->pt2;
        }
    }
    
    // We calculate coef of each side of the triangle
    float coef1 = 0.0, coef2 = 0.0, coef3 = 0.0;
    if(Center->x_2D != Left->x_2D)
        coef1 = ((float)Center->y_2D-(float)Left->y_2D)/((float)Center->x_2D-(float)Left->x_2D);
    if(Right->x_2D != Left->x_2D)
        coef2 = ((float)Right->y_2D-(float)Left->y_2D)/((float)Right->x_2D-(float)Left->x_2D);
    if(Right->x_2D != Center->x_2D)
        coef3 = ((float)Right->y_2D-(float)Center->y_2D)/((float)Right->x_2D-(float)Center->x_2D);
    
    // We start to fill it
    for (i = 0; i<(Right->x_2D-Left->x_2D); i++)
    {
        int y1 = 0;
        int y2 = 0;
        int ytemp = 0;
        
        y1 = (int)((double)(coef2*i))+Left->y_2D;                                // Calculate the beginning of the line
        if(i <= Center->x_2D-Left->x_2D)
        {
            y2 = (int)((double)(coef1*i)+Left->y_2D);                            // Calculate the end of the line for the first part
        }
        else
        {
            y2 = (int)((double)(coef3*(i-Center->x_2D+Left->x_2D)+Center->y_2D));  // Calculate the end of the line for the second part
        }
        if(y1 >= y2)
        {
            ytemp = y1;
            y1 = y2;
            y2 = ytemp;
        }
        for(j = y1; j<y2; j++)                                                  // Draw the whole line
        {
            // To avoid trying accessing out of limit of the table:
            if(i<0)
                i = 0;
            if(i>w)
                i = w-1;
            if(j<0)
                j = 0;
            if(j>h)
                j = h-1;
            canevas[j*h+i+(int)Left->x_2D].depth = 1;
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
    struct m3DPoint *canevas = malloc(SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(struct m3DPoint));  // Have to use dynamic allocation 'cause of to big table!
    

    m3DPoint tabPoint[8];
    
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
    
    plan plan1;
    plan plan2;
    plan plan3;
    plan plan4;
    plan plan5;
    plan plan6;
    plan plan7;
    plan plan8;
    plan plan9;
    plan plan10;
    plan plan11;
    plan plan12;
    
    plan1.pt1 = &tabPoint[0];
    plan1.pt2 = &tabPoint[1];
    plan1.pt3 = &tabPoint[2];
    
    plan2.pt1 = &tabPoint[2];
    plan2.pt2 = &tabPoint[3];
    plan2.pt3 = &tabPoint[0];
    
    plan3.pt1 = &tabPoint[4];
    plan3.pt2 = &tabPoint[5];
    plan3.pt3 = &tabPoint[6];
    
    plan4.pt1 = &tabPoint[6];
    plan4.pt2 = &tabPoint[7];
    plan4.pt3 = &tabPoint[4];
    
    plan5.pt1 = &tabPoint[5];
    plan5.pt2 = &tabPoint[6];
    plan5.pt3 = &tabPoint[2];
    
    plan6.pt1 = &tabPoint[2];
    plan6.pt2 = &tabPoint[1];
    plan6.pt3 = &tabPoint[5];
    
    plan7.pt1 = &tabPoint[3];
    plan7.pt2 = &tabPoint[7];
    plan7.pt3 = &tabPoint[6];
    
    plan8.pt1 = &tabPoint[3];
    plan8.pt2 = &tabPoint[6];
    plan8.pt3 = &tabPoint[7];
    
    plan9.pt1 = &tabPoint[3];
    plan9.pt2 = &tabPoint[0];
    plan9.pt3 = &tabPoint[7];
    
    plan10.pt1 = &tabPoint[0];
    plan10.pt2 = &tabPoint[4];
    plan10.pt3 = &tabPoint[7];
    
    plan11.pt1 = &tabPoint[5];
    plan11.pt2 = &tabPoint[4];
    plan11.pt3 = &tabPoint[0];
    
    plan12.pt1 = &tabPoint[0];
    plan12.pt2 = &tabPoint[1];
    plan12.pt3 = &tabPoint[5];

    /*    plan1.coefA = (plan1.p2->y_3D - plan1.p1->y_3D)*(plan1.p3->z_3D - plan1.p1->z_3D) - (plan1.p2->z_3D - plan1.p1->z_3D)*(plan1.p3->y_3D-plan1.p1->y_3D);
     
     plan1.coefB = (-1)*((plan1.p2->x_3D - plan1.p1->x_3D)*(plan1.p3->z_3D - plan1.p1->z_3D) - (plan1.p2->z_3D - plan1.p1->z_3D)*(plan1.p3->x_3D-plan1.p1->x_3D));
     
     plan1.coefC = (plan1.p2->x_3D - plan1.p1->x_3D)*(plan1.p3->y_3D-plan1.p1->y_3D) - (plan1.p2->y_3D - plan1.p1->y_3D)*(plan1.p3->x_3D-plan1.p1->x_3D);
     
     plan1.coefD = (-1)*(plan1.coefA*plan1.p1->x_3D + plan1.coefB*plan1.p1->y_3D + plan1.coefC*plan1.p1->z_3D);
     */
    
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
    angle = 1;
    while(1)
    {
        
        
        for(int i = 0; i<8; i++)
        {
            tabPoint[i].x_3D = tabPoint[i].x_3D*cos(angle%360*DEG_TO_RAD)-tabPoint[i].y_3D*sin(angle%360*DEG_TO_RAD);
            tabPoint[i].y_3D = tabPoint[i].x_3D*sin(angle%360*DEG_TO_RAD)+tabPoint[i].y_3D*cos(angle%360*DEG_TO_RAD);
            tabPoint[i].z_3D = tabPoint[i].z_3D;
        }
        
        for(int i = 0; i<8; i++)
        {
            tabPoint[i].x_3D = tabPoint[i].x_3D;
            tabPoint[i].y_3D = tabPoint[i].y_3D*cos(angle%360*DEG_TO_RAD)-tabPoint[i].z_3D*sin(angle%360*DEG_TO_RAD);
            tabPoint[i].z_3D = tabPoint[i].y_3D*sin(angle%360*DEG_TO_RAD)+tabPoint[i].z_3D*cos(angle%360*DEG_TO_RAD);
        }
        
        for(int i = 0; i<8; i++)
        {
            tabPoint[i].depth = sqrtf(pow(tabPoint[i].x_3D, 2)+pow(USER_DISTANCE - tabPoint[i].y_3D, 2)+pow(tabPoint[i].z_3D, 2));
            tabPoint[i].x_2D = CONVERT_POS_X((USER_DISTANCE*tabPoint[i].x_3D)/(USER_DISTANCE+tabPoint[i].z_3D));
            tabPoint[i].y_2D = CONVERT_POS_Y((USER_DISTANCE*tabPoint[i].y_3D)/(USER_DISTANCE+tabPoint[i].z_3D));
        }
        
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);                        // We set the background color (white)
        SDL_RenderClear(ren);
        
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);                              // We will draw in black
        
        // dessin des contours de l'écran
        SDL_RenderDrawLine(ren, 0, 0, SCREEN_WIDTH-1, 0);
        SDL_RenderDrawLine(ren, SCREEN_WIDTH-1, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1);
        SDL_RenderDrawLine(ren, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, 0, SCREEN_HEIGHT-1);
        SDL_RenderDrawLine(ren, 0, SCREEN_HEIGHT-1, 0, 0);
        
        draw2DTriangle((m3DPoint*)canevas, SCREEN_WIDTH, SCREEN_HEIGHT, &plan1);
        draw2DTriangle((m3DPoint*)canevas, SCREEN_WIDTH, SCREEN_HEIGHT, &plan2);
        draw2DTriangle((m3DPoint*)canevas, SCREEN_WIDTH, SCREEN_HEIGHT, &plan3);
        draw2DTriangle((m3DPoint*)canevas, SCREEN_WIDTH, SCREEN_HEIGHT, &plan4);
        draw2DTriangle((m3DPoint*)canevas, SCREEN_WIDTH, SCREEN_HEIGHT, &plan5);
        draw2DTriangle((m3DPoint*)canevas, SCREEN_WIDTH, SCREEN_HEIGHT, &plan6);
        draw2DTriangle((m3DPoint*)canevas, SCREEN_WIDTH, SCREEN_HEIGHT, &plan7);
        draw2DTriangle((m3DPoint*)canevas, SCREEN_WIDTH, SCREEN_HEIGHT, &plan8);
        draw2DTriangle((m3DPoint*)canevas, SCREEN_WIDTH, SCREEN_HEIGHT, &plan9);
        draw2DTriangle((m3DPoint*)canevas, SCREEN_WIDTH, SCREEN_HEIGHT, &plan10);
        draw2DTriangle((m3DPoint*)canevas, SCREEN_WIDTH, SCREEN_HEIGHT, &plan11);
        draw2DTriangle((m3DPoint*)canevas, SCREEN_WIDTH, SCREEN_HEIGHT, &plan12);
 
        for(int i = 0; i<SCREEN_WIDTH; i++)
        {
            for(int j = 0; j<SCREEN_HEIGHT; j++)
            {
                if(canevas[j*SCREEN_HEIGHT+i].depth == 1)
                {
                    SDL_RenderDrawPoint(ren, i, j);
                    canevas[j*SCREEN_HEIGHT+i].depth = 0;                       // We have displayed the point. Clear it!
                }
            }
        }
        SDL_RenderPresent(ren);
        SDL_Delay(1000/FRAME_RATE);
    }
    
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();                                                                 // We leave SDL
    
    return 0;
}
