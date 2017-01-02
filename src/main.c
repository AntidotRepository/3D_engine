#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "event.h"

#define FRAME_RATE              60
#define EVENT_REFRESH_RATE      60
#define SCREEN_WIDTH            600                                             // Window width
#define SCREEN_HEIGHT           480                                             // Window heigh
#define FRAME_RATE              60
#define DEG_TO_RAD              0.01745329
#define MAX_DEPTH               5000
#define MOUSE_SENSITIVITY       1

#define CONVERT_POS_X(pos_X)    pos_X+SCREEN_WIDTH/2
#define CONVERT_POS_Y(pos_Y)    pos_Y+SCREEN_HEIGHT/2


//
//typedef struct camera camera;
//struct camera
//{
//    int posX;
//    int posY;
//    int posZ;
//    int angleX;
//    int angleY;
//    int angleZ;
//};
//
//typedef struct color color;
//struct color
//{
//    char r;
//    char g;
//    char b;
//};
//
//typedef struct m3DPoint m3DPoint;
//struct m3DPoint
//{
//    float x_3D;
//    float y_3D;
//    float z_3D;
//    float x_2D;
//    float y_2D;
//    char display;
//    float depth;
//    color color;
//};
//
//typedef struct plan plan;
//struct plan
//{
//    m3DPoint *pt1;
//    m3DPoint *pt2;
//    m3DPoint *pt3;
//    float coefA;
//    float coefB;
//    float coefC;
//    float coefD;
//    color color;
//    // Add normal vector
//};
//
//int get_index(x, y)
//{
//    if(x<0)
//        x = 0;
//    if(y<0)
//        y = 0;
//    if(x>=SCREEN_WIDTH || y>=SCREEN_HEIGHT)
//        return SCREEN_HEIGHT*SCREEN_WIDTH-1;
//    return SCREEN_WIDTH*y+x;
//}
//
///**
// *  \fn         drawTriangle
// *  \brief      Draw a triangle
// *  \param[in]
// */
//void draw2DTriangle(m3DPoint *canevas, int w, int h, plan *p)
//{
//    m3DPoint *Left, *Center, *Right;
//    int i = 0;
//    int j = 0;
//    
//    // If all points are out of screen -> We don't draw something
//    if(p->pt1->depth > 0 || p->pt2->depth > 0 || p->pt3->depth > 0)
//    {
//        // We sort points from left to right
//        if((p->pt1->x_2D <= p->pt2->x_2D) && (p->pt1->x_2D <= p->pt3->x_2D))
//        {
//            Left = p->pt1;
//            if(p->pt2->x_2D < p->pt3->x_2D)
//            {
//                Center = p->pt2;
//                Right = p->pt3;
//            }
//            else
//            {
//                Center = p->pt3;
//                Right = p->pt2;
//            }
//        }
//        else if((p->pt2->x_2D <= p->pt3->x_2D) && (p->pt2->x_2D <= p->pt1->x_2D))
//        {
//            Left = p->pt2;
//            if(p->pt1->x_2D < p->pt3->x_2D)
//            {
//                Center = p->pt1;
//                Right = p->pt3;
//            }
//            else
//            {
//                Center = p->pt3;
//                Right = p->pt1;
//            }
//        }
//        else
//        {
//            Left = p->pt3;
//            if(p->pt2->x_2D < p->pt1->x_2D)
//            {
//                Center = p->pt2;
//                Right = p->pt1;
//            }
//            else
//            {
//                Center = p->pt1;
//                Right = p->pt2;
//            }
//        }
//        
//        // We calculate coef of each side of the triangle
//        float coef1 = 0.0, coef2 = 0.0, coef3 = 0.0;
//        int L = 0;
//        if(Center->x_2D != Left->x_2D)
//        {
//            coef1 = ((float)Center->y_2D-(float)Left->y_2D)/((float)Center->x_2D-(float)Left->x_2D);
//            L = sqrt(pow(Center->x_3D-Left->x_3D, 2)+pow(Center->y_3D-Left->y_3D, 2)+pow(Center->z_3D-Left->z_3D, 2));
//        }
//        if(Right->x_2D != Left->x_2D)
//        {
//            coef2 = ((float)Right->y_2D-(float)Left->y_2D)/((float)Right->x_2D-(float)Left->x_2D);
//            L = sqrt(pow(Right->x_3D-Left->x_3D, 2)+pow(Right->y_3D-Left->y_3D, 2)+pow(Right->z_3D-Left->z_3D, 2));
//        }
//        if(Right->x_2D != Center->x_2D)
//        {
//            coef3 = ((float)Right->y_2D-(float)Center->y_2D)/((float)Right->x_2D-(float)Center->x_2D);
//            L = sqrt(pow(Right->x_3D-Center->x_3D, 2)+pow(Right->y_3D-Center->y_3D, 2)+pow(Right->z_3D-Center->z_3D, 2));
//        }
//        
//        // We start to fill it
//        for (i = 0; i<(Right->x_2D-Left->x_2D); i++)
//        {
//            m3DPoint y1;
//            m3DPoint y2;
//            m3DPoint ytemp;
//            float tempDepth = 0;
//            int index = 0;
//            
//            y1.y_2D = (int)((double)(coef2*i))+Left->y_2D;                          // Calculate the beginning of the line
//            tempDepth = (Right->depth+Center->depth+Left->depth)/3;                 // Approximation of the distance of the plane to the camera
//            
//            index = get_index(i+(int)Left->x_2D,(int)(y1.y_2D));
//            if(canevas[index].depth > tempDepth)
//            {
//                canevas[index].color = p->color;
//                canevas[index].depth = tempDepth;
//            }
//            
//            if(i <= Center->x_2D-Left->x_2D)
//            {
//                y2.y_2D = (int)((double)(coef1*i)+Left->y_2D);                      // Calculate the end of the line for the first part
//                if(y1.y_2D >= y2.y_2D)
//                {
//                    ytemp = y1;
//                    y1 = y2;
//                    y2 = ytemp;
//                }
//                for(j = y1.y_2D; j<y2.y_2D; j++)
//                {
//                    index = get_index(i+(int)Left->x_2D,j);
//                    if(canevas[index].depth > tempDepth)
//                    {
//                        canevas[index].color = p->color;
//                        canevas[index].depth = tempDepth;
//                    }
//                }
//            }
//            else
//            {
//                y2.y_2D = (int)((double)(coef3*(i-Center->x_2D+Left->x_2D)+Center->y_2D));// Calculate the end of the line for the second part
//                if(y1.y_2D >= y2.y_2D)
//                {
//                    ytemp = y1;
//                    y1 = y2;
//                    y2 = ytemp;
//                }
//                for(j = y1.y_2D; j<y2.y_2D; j++)
//                {
//                    index = get_index(i+(int)Left->x_2D,j);
//                    if(canevas[index].depth > tempDepth)
//                    {
//                        canevas[index].color = p->color;
//                        canevas[index].depth = tempDepth;
//                    }
//                }
//            }
//        }
//    }
//}
//
//void calc_coef_plan(plan *p)
//{
//    p->coefA = (p->pt2->y_3D - p->pt1->y_3D)*(p->pt3->z_3D - p->pt1->z_3D) - (p->pt2->z_3D - p->pt1->z_3D)*(p->pt3->y_3D-p->pt1->y_3D);
//    p->coefB = (-1)*((p->pt2->x_3D - p->pt1->x_3D)*(p->pt3->z_3D - p->pt1->z_3D) - (p->pt2->z_3D - p->pt1->z_3D)*(p->pt3->x_3D-p->pt1->x_3D));
//    p->coefC = (p->pt2->x_3D - p->pt1->x_3D)*(p->pt3->y_3D-p->pt1->y_3D) - (p->pt2->y_3D - p->pt1->y_3D)*(p->pt3->x_3D-p->pt1->x_3D);
//    p->coefD = (-1)*(p->coefA*p->pt1->x_3D + p->coefB*p->pt1->y_3D + p->coefC*p->pt1->z_3D);
//}
//
///**
// *  \fn         matrix_product
// *  \brief      Multiplication of a 4x4 matrix by a 4-elements vector
// *  \param[out] res result of the calculation
// *  \param[in]  mat1 4x4 matrix
// *  \param[in]  mat2 4-elements vector
// */
//void matrix_product(float res[4], float mat1[4][4], float mat2[4])
//{
//    res[0] = mat1[0][0]*mat2[0]+mat1[0][1]*mat2[1]+mat1[0][2]*mat2[2]+mat1[0][3]*mat2[3];
//    res[1] = mat1[1][0]*mat2[0]+mat1[1][1]*mat2[1]+mat1[1][2]*mat2[2]+mat1[1][3]*mat2[3];
//    res[2] = mat1[2][0]*mat2[0]+mat1[2][1]*mat2[1]+mat1[2][2]*mat2[2]+mat1[2][3]*mat2[3];
//    res[3] = mat1[3][0]*mat2[0]+mat1[3][1]*mat2[1]+mat1[3][2]*mat2[2]+mat1[3][3]*mat2[3];
//}
//
///**
// *  \fn             rotate
// *  \brief          Rotate a point p by an angle gived by "angle" around an axis gived by x, y and z
// *  \param[in]      x, y, z Unity axis of rotation
// *  \param[in]      angle Angle to rotate
// *  \param[inout]   *p Point to rotate
// */
//void rotate(float x, float y, float z, int angle, m3DPoint *p)
//{
//    float teta = angle % 360 * DEG_TO_RAD;
//    float mat1[4][4] =
//    {{x*x*(1-cos(teta))+cos(teta), x*y*(1-cos(teta))-z*sin(teta), x*z*(1-cos(teta))+y*sin(teta), 0},
//        {x*y*(1-cos(teta))+z*sin(teta), y*y*(1-cos(teta))+cos(teta), y*z*(1-cos(teta))-x*sin(teta), 0},
//        {x*z*(1-cos(teta))-y*sin(teta), y*z*(1-cos(teta))+x*sin(teta), z*z*(1-cos(teta))+cos(teta), 0},
//        {0, 0, 0, 1}};
//    float mat2[4] = {p->x_3D, p->y_3D, p->z_3D, 1};
//    float res[4] = {0};
//    matrix_product(res, mat1, mat2);
//    
//    p->x_3D = res[0];
//    p->y_3D = res[1];
//    p->z_3D = res[2];
//}
//
///**
// *  \fn             translate
// *  \brief          Translate a point p by a distance gived by x, y and z
// *  \param[in]      x, y, z Distance to move on each axis
// *  \param[inout]   *p Point to move
// */
//void translate(float x, float y, float z, m3DPoint *p)
//{
//    float mat1[4][4] = {{1, 0, 0, x},
//        {0, 1, 0, y},
//        {0, 0, 1, z},
//        {0, 0, 0, 1}};
//    float mat2[4] = {p->x_3D, p->y_3D, p->z_3D, 1};
//    float res[4] = {0};
//    matrix_product(res, mat1, mat2);
//    p->x_3D = res[0];
//    p->y_3D = res[1];
//    p->z_3D = res[2];
//}
//
//
///**
// *  \fn     logSDLError
// *  \brief  Display a given message with the last error get by SDL
// *  \param  msg String to display with the error
// */
//void logSDLError(char *msg)
//{
//    printf("%s error: %s\n", msg, SDL_GetError());
//}
//

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
        UpdateEvents((Input*)p_data);
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
    
    Input in;
    
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
    ret = pthread_create(&threadT_userEvent, NULL, thread_userEvent, &in);
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
