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

typedef struct Input Input;
struct Input
{
    char key[SDL_NUM_SCANCODES];
    int mouseX;
    int mouseY;
    int mouseXRel;
    int mouseYRel;
    char mousebuttons[8];
    char quit;
};

typedef struct camera camera;
struct camera
{
    int posX;
    int posY;
    int posZ;
    int angleX;
    int angleY;
    int angleZ;
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
 *  \fn         matrix_product
 *  \brief      Multiplication of a 4x4 matrix by a 4-elements vector
 *  \param[out] res result of the calculation
 *  \param[in]  mat1 4x4 matrix
 *  \param[in]  mat2 4-elements vector
 */
void matrix_product(float res[4], float mat1[4][4], float mat2[4])
{
    res[0] = mat1[0][0]*mat2[0]+mat1[0][1]*mat2[1]+mat1[0][2]*mat2[2]+mat1[0][3]*mat2[3];
    res[1] = mat1[1][0]*mat2[0]+mat1[1][1]*mat2[1]+mat1[1][2]*mat2[2]+mat1[1][3]*mat2[3];
    res[2] = mat1[2][0]*mat2[0]+mat1[2][1]*mat2[1]+mat1[2][2]*mat2[2]+mat1[2][3]*mat2[3];
    res[3] = mat1[3][0]*mat2[0]+mat1[3][1]*mat2[1]+mat1[3][2]*mat2[2]+mat1[3][3]*mat2[3];
}

/**
 *  \fn             rotate
 *  \brief          Rotate a point p by an angle gived by "angle" around an axis gived by x, y and z
 *  \param[in]      x, y, z Unity axis of rotation
 *  \param[in]      angle Angle to rotate
 *  \param[inout]   *p Point to rotate
 */
void rotate(float x, float y, float z, int angle, m3DPoint *p)
{
    float teta = angle % 360 * DEG_TO_RAD;
    float mat1[4][4] =
    {{x*x*(1-cos(teta))+cos(teta), x*y*(1-cos(teta))-z*sin(teta), x*z*(1-cos(teta))+y*sin(teta), 0},
        {x*y*(1-cos(teta))+z*sin(teta), y*y*(1-cos(teta))+cos(teta), y*z*(1-cos(teta))-x*sin(teta), 0},
        {x*z*(1-cos(teta))-y*sin(teta), y*z*(1-cos(teta))+x*sin(teta), z*z*(1-cos(teta))+cos(teta), 0},
        {0, 0, 0, 1}};
    float mat2[4] = {p->x_3D, p->y_3D, p->z_3D, 1};
    float res[4] = {0};
    matrix_product(res, mat1, mat2);
    
    p->x_3D = res[0];
    p->y_3D = res[1];
    p->z_3D = res[2];
}

/**
 *  \fn             translate
 *  \brief          Translate a point p by a distance gived by x, y and z
 *  \param[in]      x, y, z Distance to move on each axis
 *  \param[inout]   *p Point to move
 */
void translate(float x, float y, float z, m3DPoint *p)
{
    float mat1[4][4] = {{1, 0, 0, x},
        {0, 1, 0, y},
        {0, 0, 1, z},
        {0, 0, 0, 1}};
    float mat2[4] = {p->x_3D, p->y_3D, p->z_3D, 1};
    float res[4] = {0};
    matrix_product(res, mat1, mat2);
    p->x_3D = res[0];
    p->y_3D = res[1];
    p->z_3D = res[2];
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

void UpdateEvents(Input *in)
{
    SDL_Event event;
    
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_KEYDOWN:
                in->key[event.key.keysym.sym] = 1;
                break;
            case SDL_KEYUP:
                in->key[event.key.keysym.sym] = 0;
                break;
            case SDL_MOUSEMOTION:
                in->mouseX = event.motion.x;
                in->mouseY = event.motion.y;
                in->mouseXRel = event.motion.xrel;
                in->mouseYRel = event.motion.yrel;
                break;
            case SDL_MOUSEBUTTONDOWN:
                in->mousebuttons[event.button.button] = 1;
                break;
            case SDL_MOUSEBUTTONUP:
                in->mousebuttons[event.button.button] = 0;
                break;
            case SDL_QUIT:
                in->quit = 1;
                break;
            default:
                break;
        }
    }
}

/**
 *  \fn     main
 *  \brief  Main program
 *  \param
 */
int main(int argc, char **argv)
{
    Uint32 Actual_time = 0;
    Uint32 Previous_time = 0;
    Input in;
    memset(&in, 0, sizeof(in));                                                 // Initialize the structure to 0
    int pause = 0;
    int angle = 0;
    struct m3DPoint *canevas = malloc(SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(struct m3DPoint));  // Have to use dynamic allocation 'cause of too big table!
    TTF_Font *police = NULL;
    int index = 0;
    
    camera camera;
    camera.posX = 0;
    camera.posY = 300;
    camera.posZ = 0;
    
    m3DPoint model[8];
    m3DPoint view[8];
    
    model[0].x_3D =  50;
    model[0].y_3D =  50;
    model[0].z_3D = -50;
    
    model[1].x_3D = -50;
    model[1].y_3D =  50;
    model[1].z_3D = -50;
    
    model[2].x_3D = -50;
    model[2].y_3D = -50;
    model[2].z_3D = -50;
    
    model[3].x_3D =  50;
    model[3].y_3D = -50;
    model[3].z_3D = -50;
    
    model[4].x_3D =  50;
    model[4].y_3D =  50;
    model[4].z_3D =  50;
    
    model[5].x_3D = -50;
    model[5].y_3D =  50;
    model[5].z_3D =  50;
    
    model[6].x_3D = -50;
    model[6].y_3D = -50;
    model[6].z_3D =  50;
    
    model[7].x_3D =  50;
    model[7].y_3D = -50;
    model[7].z_3D =  50;
    
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
    
    plan1.pt1 = &model[0];
    plan1.pt2 = &model[1];
    plan1.pt3 = &model[2];
    plan1.color.r = 255;
    plan1.color.g = 0;
    plan1.color.b = 0;
    
    plan2.pt1 = &model[2];
    plan2.pt2 = &model[3];
    plan2.pt3 = &model[0];
    plan2.color.r = 255;
    plan2.color.g = 0;
    plan2.color.b = 0;
    
    plan3.pt1 = &model[4];
    plan3.pt2 = &model[5];
    plan3.pt3 = &model[6];
    plan3.color.r = 0;
    plan3.color.g = 0;
    plan3.color.b = 255;
    
    plan4.pt1 = &model[6];
    plan4.pt2 = &model[7];
    plan4.pt3 = &model[4];
    plan4.color.r = 0;
    plan4.color.g = 0;
    plan4.color.b = 255;
    
    plan5.pt1 = &model[5];
    plan5.pt2 = &model[6];
    plan5.pt3 = &model[2];
    plan5.color.r = 0;
    plan5.color.g = 255;
    plan5.color.b = 0;
    
    plan6.pt1 = &model[2];
    plan6.pt2 = &model[1];
    plan6.pt3 = &model[5];
    plan6.color.r = 0;
    plan6.color.g = 255;
    plan6.color.b = 0;
    
    plan7.pt1 = &model[3];
    plan7.pt2 = &model[7];
    plan7.pt3 = &model[6];
    plan7.color.r = 127;
    plan7.color.g = 127;
    plan7.color.b = 0;
    
    plan8.pt1 = &model[3];
    plan8.pt2 = &model[6];
    plan8.pt3 = &model[2];
    plan8.color.r = 127;
    plan8.color.g = 127;
    plan8.color.b = 0;
    
    plan9.pt1 = &model[3];
    plan9.pt2 = &model[0];
    plan9.pt3 = &model[7];
    plan9.color.r = 0;
    plan9.color.g = 127;
    plan9.color.b = 127;
    
    plan10.pt1 = &model[0];
    plan10.pt2 = &model[4];
    plan10.pt3 = &model[7];
    plan10.color.r = 0;
    plan10.color.g = 127;
    plan10.color.b = 127;
    
    plan11.pt1 = &model[5];
    plan11.pt2 = &model[4];
    plan11.pt3 = &model[0];
    plan11.color.r = 127;
    plan11.color.g = 0;
    plan11.color.b = 127;
    
    plan12.pt1 = &model[0];
    plan12.pt2 = &model[1];
    plan12.pt3 = &model[5];
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
    
    angle = 1;
    while(!in.key[SDLK_ESCAPE] && !in.quit)
    {
        Actual_time = SDL_GetTicks();
        
        UpdateEvents(&in);
        if(in.key[SDLK_SPACE])
        {
            in.key[SDLK_SPACE] = 0;
            if(pause == 0)
                pause = 1;
            else
                pause = 0;
        }
        
        if(!pause)
        {
            if(Actual_time - Previous_time > 1000/FRAME_RATE)
            {
                Previous_time = Actual_time;
                
                // Moving the cube
                for(int i = 0; i<8; i++)
                {
                    rotate(0, 0, 1, angle, &model[i]);
                    rotate(0, 1, 0, angle, &model[i]);
                    rotate(1, 0, 0, angle, &model[i]);
                    
                    translate(angle, angle, angle, &model[i]);
                }
                
                // Moving the camera
                for(int i = 0; i<8; i++)
                {
                    view[i] = model[i];
                    rotate(0, 0, 1, camera.angleX, &view[i]);
                    camera.angleX++;
                    translate(camera.posX, camera.posY, camera.posZ, &view[i]);
                }
                
                for(int i = 0; i<8; i++)
                {
                    model[i].depth = sqrtf(pow(view[i].x_3D, 2)+pow(view[i].y_3D, 2)+pow(view[i].z_3D, 2));
                    model[i].x_2D = CONVERT_POS_X((view[i].depth*view[i].x_3D)/(view[i].depth+view[i].y_3D));
                    model[i].y_2D = CONVERT_POS_Y((view[i].depth*view[i].z_3D*(-1))/(view[i].depth+view[i].y_3D));
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
                        canevas[j*SCREEN_HEIGHT+i].color.r = 220;                   // We have displayed the point. Clear it!
                        canevas[j*SCREEN_HEIGHT+i].color.g = 220;                   // We have displayed the point. Clear it!
                        canevas[j*SCREEN_HEIGHT+i].color.b = 220;                   // We have displayed the point. Clear it!
                        canevas[j*SCREEN_HEIGHT+i].depth = MAX_DEPTH;
                    }
                }
                for (int i = 0; i<8; i++)
                {
                    sprintf(msg_coord_pt[i], "X: %.1lf, Y: %.1lf, Z: %.1lf, D: %.1lf", model[i].x_3D, model[i].y_3D, model[i].z_3D, model[i].depth);
                    //sprintf(msg_coord_pt[i], "%lf", model[i].depth);
                    Surf_coord_pt[i] = TTF_RenderText_Solid(police, msg_coord_pt[i], black);
                    Rect_coord_pt[i].x =  view[i].x_2D;
                    Rect_coord_pt[i].y =  view[i].y_2D;
                    text_coord_pt[i] = SDL_CreateTextureFromSurface(ren, Surf_coord_pt[i]);
                    SDL_FreeSurface(Surf_coord_pt[i]);
                    SDL_RenderCopy(ren, text_coord_pt[i], NULL, &Rect_coord_pt[i]);
                    SDL_DestroyTexture(text_coord_pt[i]);
                }
                SDL_RenderPresent(ren);
                SDL_Delay(1000/FRAME_RATE);
            }
            else
            {
                SDL_Delay(Actual_time - Previous_time - 1000/FRAME_RATE);
            }
        }
    }
    
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_CloseFont(police);
    TTF_Quit();
    SDL_Quit();                                                                 // We leave SDL
    
    return 0;
}
