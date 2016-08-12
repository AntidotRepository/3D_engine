#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH            640                                             // Window width
#define SCREEN_HEIGHT           480                                             // Window heigh
#define FRAME_RATE              60
#define DEG_TO_RAD              0.01745329
#define MAX_DEPTH               1000

#define CONVERT_POS_X(pos_X)    pos_X+SCREEN_WIDTH/2
#define CONVERT_POS_Y(pos_Y)    pos_Y+SCREEN_HEIGHT/2

typedef struct camera camera;
struct camera
{
    int posX;
    int posY;
    int posZ;
};

typedef struct color color;
struct color
{
    char r;
    char g;
    char b;
};

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
    color color;
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
    color color;
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
    int L = 0;
    if(Center->x_2D != Left->x_2D)
    {
        coef1 = ((float)Center->y_2D-(float)Left->y_2D)/((float)Center->x_2D-(float)Left->x_2D);
        L = sqrt(pow(Center->x_3D-Left->x_3D, 2)+pow(Center->y_3D-Left->y_3D, 2)+pow(Center->z_3D-Left->z_3D, 2));
    }
    if(Right->x_2D != Left->x_2D)
    {
        coef2 = ((float)Right->y_2D-(float)Left->y_2D)/((float)Right->x_2D-(float)Left->x_2D);
        L = sqrt(pow(Right->x_3D-Left->x_3D, 2)+pow(Right->y_3D-Left->y_3D, 2)+pow(Right->z_3D-Left->z_3D, 2));
    }
    if(Right->x_2D != Center->x_2D)
    {
        coef3 = ((float)Right->y_2D-(float)Center->y_2D)/((float)Right->x_2D-(float)Center->x_2D);
        L = sqrt(pow(Right->x_3D-Center->x_3D, 2)+pow(Right->y_3D-Center->y_3D, 2)+pow(Right->z_3D-Center->z_3D, 2));
    }
    
    // We start to fill it
    for (i = 0; i<(Right->x_2D-Left->x_2D); i++)
    {
        m3DPoint y1;
        m3DPoint y2;
        m3DPoint ytemp;
        float tempDepth = 0;
        int index = 0;
        
        y1.y_2D = (int)((double)(coef2*i))+Left->y_2D;                          // Calculate the beginning of the line
        tempDepth = (Right->depth+Center->depth+Left->depth)/3;                 // Approximation of the distance of the plane to the camera
        
        index  = (int)(y1.y_2D)*h+i+(int)Left->x_2D;
        if(canevas[index].depth > tempDepth)
        {
            canevas[index].color = p->color;
            canevas[index].depth = tempDepth;
        }
        
        if(i <= Center->x_2D-Left->x_2D)
        {
            y2.y_2D = (int)((double)(coef1*i)+Left->y_2D);                      // Calculate the end of the line for the first part
            if(y1.y_2D >= y2.y_2D)
            {
                ytemp = y1;
                y1 = y2;
                y2 = ytemp;
            }
            for(j = y1.y_2D; j<y2.y_2D; j++)
            {
                index = (int)(j*h+i+(int)Left->x_2D);
                if(canevas[index].depth > tempDepth)
                {
                    canevas[index].color = p->color;
                    canevas[index].depth = tempDepth;
                }
            }
        }
        else
        {
            y2.y_2D = (int)((double)(coef3*(i-Center->x_2D+Left->x_2D)+Center->y_2D));// Calculate the end of the line for the second part
            if(y1.y_2D >= y2.y_2D)
            {
                ytemp = y1;
                y1 = y2;
                y2 = ytemp;
            }
            for(j = y1.y_2D; j<y2.y_2D; j++)
            {
                index = (int)(j*h+i+(int)Left->x_2D);
                if(canevas[index].depth > tempDepth)
                {
                    canevas[index].color = p->color;
                    canevas[index].depth = tempDepth;
                }
            }
        }
    }
}

void calc_coef_plan(plan *p)
{
    p->coefA = (p->pt2->y_3D - p->pt1->y_3D)*(p->pt3->z_3D - p->pt1->z_3D) - (p->pt2->z_3D - p->pt1->z_3D)*(p->pt3->y_3D-p->pt1->y_3D);
    p->coefB = (-1)*((p->pt2->x_3D - p->pt1->x_3D)*(p->pt3->z_3D - p->pt1->z_3D) - (p->pt2->z_3D - p->pt1->z_3D)*(p->pt3->x_3D-p->pt1->x_3D));
    p->coefC = (p->pt2->x_3D - p->pt1->x_3D)*(p->pt3->y_3D-p->pt1->y_3D) - (p->pt2->y_3D - p->pt1->y_3D)*(p->pt3->x_3D-p->pt1->x_3D);
    p->coefD = (-1)*(p->coefA*p->pt1->x_3D + p->coefB*p->pt1->y_3D + p->coefC*p->pt1->z_3D);
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
    TTF_Font *police = NULL;
    int index = 0;
    camera camera;
    camera.posX = 0;
    camera.posY = 0;
    camera.posZ = 300;
    
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
    plan1.color.r = 255;
    plan1.color.g = 0;
    plan1.color.b = 0;
    
    plan2.pt1 = &tabPoint[2];
    plan2.pt2 = &tabPoint[3];
    plan2.pt3 = &tabPoint[0];
    plan2.color.r = 255;
    plan2.color.g = 0;
    plan2.color.b = 0;
    
    plan3.pt1 = &tabPoint[4];
    plan3.pt2 = &tabPoint[5];
    plan3.pt3 = &tabPoint[6];
    plan3.color.r = 0;
    plan3.color.g = 0;
    plan3.color.b = 255;
    
    plan4.pt1 = &tabPoint[6];
    plan4.pt2 = &tabPoint[7];
    plan4.pt3 = &tabPoint[4];
    plan4.color.r = 0;
    plan4.color.g = 0;
    plan4.color.b = 255;
    
    plan5.pt1 = &tabPoint[5];
    plan5.pt2 = &tabPoint[6];
    plan5.pt3 = &tabPoint[2];
    plan5.color.r = 0;
    plan5.color.g = 255;
    plan5.color.b = 0;
    
    plan6.pt1 = &tabPoint[2];
    plan6.pt2 = &tabPoint[1];
    plan6.pt3 = &tabPoint[5];
    plan6.color.r = 0;
    plan6.color.g = 255;
    plan6.color.b = 0;
    
    plan7.pt1 = &tabPoint[3];
    plan7.pt2 = &tabPoint[7];
    plan7.pt3 = &tabPoint[6];
    plan7.color.r = 127;
    plan7.color.g = 127;
    plan7.color.b = 0;
    
    plan8.pt1 = &tabPoint[3];
    plan8.pt2 = &tabPoint[6];
    plan8.pt3 = &tabPoint[2];
    plan8.color.r = 127;
    plan8.color.g = 127;
    plan8.color.b = 0;
    
    plan9.pt1 = &tabPoint[3];
    plan9.pt2 = &tabPoint[0];
    plan9.pt3 = &tabPoint[7];
    plan9.color.r = 0;
    plan9.color.g = 127;
    plan9.color.b = 127;
    
    plan10.pt1 = &tabPoint[0];
    plan10.pt2 = &tabPoint[4];
    plan10.pt3 = &tabPoint[7];
    plan10.color.r = 0;
    plan10.color.g = 127;
    plan10.color.b = 127;
    
    plan11.pt1 = &tabPoint[5];
    plan11.pt2 = &tabPoint[4];
    plan11.pt3 = &tabPoint[0];
    plan11.color.r = 127;
    plan11.color.g = 0;
    plan11.color.b = 127;
    
    plan12.pt1 = &tabPoint[0];
    plan12.pt2 = &tabPoint[1];
    plan12.pt3 = &tabPoint[5];
    plan12.color.r = 127;
    plan12.color.g = 0;
    plan12.color.b = 127;
    
    calc_coef_plan(&plan1);
    calc_coef_plan(&plan2);
    calc_coef_plan(&plan3);
    calc_coef_plan(&plan4);
    calc_coef_plan(&plan5);
    calc_coef_plan(&plan6);
    calc_coef_plan(&plan7);
    calc_coef_plan(&plan8);
    calc_coef_plan(&plan9);
    calc_coef_plan(&plan10);
    calc_coef_plan(&plan11);
    calc_coef_plan(&plan12);
    
    if(SDL_Init(SDL_INIT_VIDEO) < 0)                                            // SDL Initialization
    {
        logSDLError("SDL_Init");
        SDL_Quit();
        
        return -1;
    }
    
    if(TTF_Init() == -1)
    {
        logSDLError("TTF_Init");
        SDL_Quit();
        return -1;
    }
    
    police = TTF_OpenFont("res/arial.ttf", 150);
    if(police == NULL)
    {
        logSDLError("TTF_OpenFont");
        SDL_Quit();
        return -1;
    }
    SDL_Color black = {0, 0, 0};
    SDL_Color white = {255, 255, 255};
    
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
    
    SDL_Surface *Surf_coord_pt[8];
    SDL_Rect Rect_coord_pt[8];
    SDL_Texture *text_coord_pt[8];
    char msg_coord_pt[8][100] = {0};
    for(int i  = 0; i<8; i++)
    {
        Surf_coord_pt[i] = TTF_RenderText_Blended(police, "", black);
        Rect_coord_pt[i].w = 150;
        Rect_coord_pt[i].h = 15;
        text_coord_pt[i] = SDL_CreateTextureFromSurface(ren, Surf_coord_pt[i]);
    }
    
    
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);                            // We set the background color (white)
    SDL_RenderClear(ren);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);                                  // We will draw in black
    angle = 1;
    while(1)
    //for (int k = 0; k<10; k++)
    {
        for(int i = 0; i<8; i++)
        {
            tabPoint[i].x_3D = tabPoint[i].x_3D;
            tabPoint[i].y_3D = tabPoint[i].y_3D*cos(angle%360*DEG_TO_RAD)-tabPoint[i].z_3D*sin(angle%360*DEG_TO_RAD);
            tabPoint[i].z_3D = tabPoint[i].y_3D*sin(angle%360*DEG_TO_RAD)+tabPoint[i].z_3D*cos(angle%360*DEG_TO_RAD);
        }
        
        for(int i = 0; i<8; i++)
        {
            tabPoint[i].x_3D = tabPoint[i].z_3D*sin(angle%360*DEG_TO_RAD)+tabPoint[i].x_3D*cos(angle%360*DEG_TO_RAD);
            tabPoint[i].y_3D = tabPoint[i].y_3D;
            tabPoint[i].z_3D = tabPoint[i].z_3D*cos(angle%360*DEG_TO_RAD)-tabPoint[i].x_3D*sin(angle%360*DEG_TO_RAD);
        }

        for(int i = 0; i<8; i++)
        {
            tabPoint[i].x_3D = tabPoint[i].x_3D*cos(angle%360*DEG_TO_RAD)-tabPoint[i].y_3D*sin(angle%360*DEG_TO_RAD);
            tabPoint[i].y_3D = tabPoint[i].x_3D*sin(angle%360*DEG_TO_RAD)+tabPoint[i].y_3D*cos(angle%360*DEG_TO_RAD);
            tabPoint[i].z_3D = tabPoint[i].z_3D;
        }
        
        for(int i = 0; i<8; i++)
        {
            tabPoint[i].depth = sqrtf(pow(camera.posX + tabPoint[i].x_3D, 2)+pow(camera.posY + tabPoint[i].y_3D, 2)+pow(camera.posZ + tabPoint[i].z_3D, 2));
            tabPoint[i].x_2D = CONVERT_POS_X((tabPoint[i].depth*tabPoint[i].x_3D)/(tabPoint[i].depth+tabPoint[i].z_3D));
            tabPoint[i].y_2D = CONVERT_POS_Y((tabPoint[i].depth*tabPoint[i].y_3D)/(tabPoint[i].depth+tabPoint[i].z_3D));
        }
        
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
                index = j*SCREEN_HEIGHT+i;
                SDL_SetRenderDrawColor(ren, canevas[index].color.r, canevas[index].color.g, canevas[index].color.b, 255);
                //SDL_SetRenderDrawColor(ren, canevas[index].depth, canevas[index].depth, canevas[index].depth, 255);
                SDL_RenderDrawPoint(ren, i, j);
                canevas[j*SCREEN_HEIGHT+i].color.r = 255;                       // We have displayed the point. Clear it!
                canevas[j*SCREEN_HEIGHT+i].color.g = 255;                       // We have displayed the point. Clear it!
                canevas[j*SCREEN_HEIGHT+i].color.b = 255;                       // We have displayed the point. Clear it!
                canevas[j*SCREEN_HEIGHT+i].depth = MAX_DEPTH;
               //}
            }
        }
        for (int i = 0; i<8; i++)
        {
            sprintf(msg_coord_pt[i], "X: %.1lf, Y: %.1lf, Z: %.1lf, D: %.1lf", tabPoint[i].x_3D, tabPoint[i].y_3D, tabPoint[i].z_3D, tabPoint[i].depth);
            //sprintf(msg_coord_pt[i], "%lf", tabPoint[i].depth);
            Surf_coord_pt[i] = TTF_RenderText_Solid(police, msg_coord_pt[i], black);
            Rect_coord_pt[i].x =  tabPoint[i].x_2D;
            Rect_coord_pt[i].y =  tabPoint[i].y_2D;
            text_coord_pt[i] = SDL_CreateTextureFromSurface(ren, Surf_coord_pt[i]);
            SDL_FreeSurface(Surf_coord_pt[i]);
            SDL_RenderCopy(ren, text_coord_pt[i], NULL, &Rect_coord_pt[i]);
        }
        SDL_RenderPresent(ren);
        SDL_Delay(1000/FRAME_RATE);
    }
    pause();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_CloseFont(police);
    TTF_Quit();
    SDL_Quit();                                                                 // We leave SDL
 
    return 0;
}
