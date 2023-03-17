#ifdef _WIN32a
#include "SDL.h"
#else
#include <linux/time.h>
#define __timespec_defined 1
#define __timeval_defined 1
#define __itimerspec_defined 1
#include <SDL2/SDL.h>
#endif
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "geometry.h"
#include "linalg.h"

#define QUITKEY SDLK_ESCAPE
#define WIDTH 1024
#define HEIGHT 720

SDL_Window *screen = NULL;
SDL_Renderer *renderer;
SDL_Event event;
SDL_Rect source, destination, dst;

int key_pressed;
int rectCount = 0;

float scale = 1.0;
float step_x = 0.0;
double angle = 0.0;

int rasterize_triangle(struct Tri tri, int offset, unsigned int *pixels, int width) {

    struct Point to_sort[3] = {tri.p1, tri.p2, tri.p3};

    struct Point *x_sorted = sort_points(to_sort, 'x');
    struct Point *y_sorted = sort_points(to_sort, 'y');

    struct Point *top, *middle, *bottom;
    int from, to;

    // facedness is defined as the direction (normal) of the long side
    bool right_facing;

    top = &y_sorted[0];
    middle = &y_sorted[1];
    bottom = &y_sorted[2];

    free(x_sorted);
    free(y_sorted);

    right_facing = middle->x <= top->x;

    int last_from, last_to;
    int from_diff, to_diff;
    int tmp_from, tmp_to;

    for (int i = (int)top->y; i < (int)middle->y; i += offset) {
        if (right_facing) {

            from = interpolate_x(*top, *middle, i);
            to = interpolate_x(*top, *bottom, i);

        } else {

            from = interpolate_x(*top, *bottom, i);
            to = interpolate_x(*top, *middle, i);
        }
        if (i - (int)top->y == 0) {
            last_from = from;
            last_to = to;
            //printf("here: %d\n", i);
        }
        if (last_from) {
            tmp_from = abs(from - last_from);
            tmp_to = abs(to - last_to);
            from_diff = tmp_from < 0 ? 1 : tmp_from; 
            to_diff = tmp_to < 0 ? 1 : tmp_to;
        }

        for (int o = 0; o < offset; o++) {
            for (int j = from; j <= to; j++) {
                if (i < HEIGHT && i > 0) {
                    pixels[j + (i + o) * width] = tri.c;
                }
            }
            if (last_from) {
                //printf("hello\n");
                //printf("[%d], from: %d, last from: %d\n", i, from, last_from);
                for (int k = from; k < from + from_diff; k++) {
                    pixels[k + i * width] = 0xffffffff;
                } 
                for (int k = to - to_diff; k < to; k++) {
                    pixels[k + i * width] = 0xffffffff;
                }
            }
        }
        last_from = from;
        last_to = to;
    }
    for (int i = (int)middle->y; i < (int)bottom->y; i += offset) {
        if (right_facing) {
            
            from = interpolate_x(*middle, *bottom, i);
            to = interpolate_x(*top, *bottom, i);

        } else {

            from = interpolate_x(*top, *bottom, i);
            to = interpolate_x(*middle, *bottom, i);
        }
        if (i - (int)middle->y == 0) {
            last_from = from;
            last_to = to;
        }
        if (last_from) {
            //printf("here\n");
            tmp_from = abs(from - last_from);
            tmp_to = abs(to - last_to);
            from_diff = tmp_from < 0 ? 1 : tmp_from; 
            to_diff = tmp_to < 0 ? 1 : tmp_to;
        }
        for (int o = 0; o < offset; o++) {
            for (int j = from; j <= to; j++) {
                if (i < HEIGHT && i > 0) {
                    pixels[j + (i + o) * width] = tri.c;
                }

            }
            if (last_from) {
                for (int k = from; k < from + from_diff; k++) {
                    pixels[k + i * width] = 0xffffffff;
                } 
                for (int k = to - to_diff; k < to; k++) {
                    pixels[k + i * width] = 0xffffffff;
                }
            }
        }
        last_from = from;
        last_to = to;
    }

    return 0;
}


void draw_line(struct Point p1, struct Point p2, unsigned int *pixels, int width) {

    int d_x = (int) abs(p1.x - p2.x);
    int d_y = (int) abs(p1.y - p2.y);
    //printf("p1 x: %f, p2 x: %f\n", p1.x, p2.x);
    //printf("p1 y: %f, p2 y: %f\n", p1.y, p2.y);
    //printf("d_x: %d\n", d_x);

    float ratio;

    int top, bottom, right, left;

    bool x_same = false;
    bool y_same = false;

    if (p1.x >= p2.x) {
        right = p1.x;
        left = p2.x;
    } else if (p2.x > p1.x) {
        right = p2.x;
        left = p1.x;
    }

    if (p1.y >= p2.y) {
        top = p2.y;
        bottom = p1.y;
    } else if (p2.y > p1.y) {
        top = p1.y;
        bottom = p2.y;
    }
    
    if (d_x != 0) {
        ratio = d_y / d_x;
    } else {
        ratio = 0;
    }
    
    if (ratio >= 1 && d_x > 0) {
        // interpolate x
        for (int i = top; i < bottom; i++) {
            int x = interpolate_x(p1, p2, i);
            if (x >= 0 && x <= WIDTH && i >= 0 && i <= HEIGHT)
                pixels[i * width + x] = 0xffffffff;
        }
    } else if (ratio < 1) {
        // interpolate y
        for (int i = left; i < right; i++) {
            int y = interpolate_y(p1, p2, i);
            if (y >= 0 && y <= HEIGHT && i >= 0 && i <= WIDTH)
                pixels[y * width + i] = 0xffffffff; 
        }
    }
    
     
    if (d_x == 0) {
        //printf("DX IS 0\n");
        for (int i = top; i < bottom; i++) {
            if (right >= 0 && right <= WIDTH && i >= 0 && i <= HEIGHT)
                pixels[i * width + right] = 0xffffffff;
        }
    }
    
    
}

void rasterize_tri_wire(struct Tri *tri, unsigned int *pixels, int width) {
    struct Point points[3] = {tri->p1, tri->p2, tri->p3};
    
    for (int i = 0; i < 3; i++) {
        //printf("hello: %d\n", i);
        draw_line(points[i % 3], points[(i + 1) % 3], pixels, width);
    }
    
    //draw_line(tri->p3, tri->p1, pixels, width);
    //printf("POINT 1 X: %f, POINT 2 X: %f\n", tri->p3.x, tri->p1.x);
}

void render(struct Tri *tris, int n_tris, bool wireframe) {

    // DO RENDERING THINGS

    // SET UP SURFACE AND DEFINE WINDOW PARAMS

    SDL_Surface *window_surface = SDL_GetWindowSurface(screen);
    unsigned int *pixels = window_surface->pixels;
    int width = window_surface->w;

    // CLEAR SCREEN
    for (size_t i = 0; i < HEIGHT * WIDTH; i++) {
        pixels[i] = 0xff000000;
    }

    int offset = 1; //pixel size
  
    for (int i = 0; i < n_tris; i++) {
        if (wireframe) {
            rasterize_tri_wire(&tris[i], pixels, width);
        } else { // this should also take address...
            rasterize_triangle(tris[i], offset, pixels, width);
        }
    }
}

void transform_geometry(struct Tri *tris, int n_tris) {

    float ret[4];
    float tmp[4][4];

    float scale_m[4][4] = {{scale, 0, 0, 0}, 
                           {0, scale, 0, 0}, 
                           {0, 0, scale, 0}, 
                           {0, 0, 0,     0}};

    float translate_x_m[4][4] = {{1, 0, 0, step_x},
                                 {0, 1, 0,      0},
                                 {0, 0, 1,      0},
                                 {0, 0, 0,      1}};
                                 
    float translate_xy_m[4][4] = {{1, 0, 0, 300},
                                 {0, 1, 0, 300},
                                 {0, 0, 1,   0},
                                 {0, 0, 0,   1}};

    float rotate_x[4][4] = {{1, 0, 0, 0},
                          {0, cos(angle), -sin(angle), 0},
                          {0, sin(angle), cos(angle), 0},
                          {0, 0, 0, 0}};

    float rotate_z[4][4] = {{cos(angle), -sin(angle), 0, 0},
                          {sin(angle), cos(angle), 0, 0},
                          {0, 0, 1, 0},
                          {0, 0, 0, 0}};
    
    mm_sq(rotate_x, rotate_z, tmp);

    for (int i = 0; i < n_tris; i++) {

        transform_tri(tris + i, tmp, ret);
        //print_point(&(*(tris + i)).p1);
    } 

}


void init_setup() {
    srand((int)time(NULL));
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_SHOWN, &screen, &renderer);
    if (!screen) {
        printf("Failed to create window!");
    }
}

void finish() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);
    SDL_Quit();
    exit(0);
}

void game_loop() {

    int game_running = 1;

    float scale_incr = 0;
    /*
    struct Tri tri = create_tri(400, 200, 0, 1, 200, 400, 0, 1, 400, 400, 0, 1, 0xff00ffff);
    struct Tri tri2 = create_tri(200, 400, 0, 1, 400, 400, 0, 1, 400, 600, 0, 1, 0x00ff00ff);
    struct Tri tri3 = create_tri(400, 400, 0, 1, 400, 600, 0, 1, 600, 400, 0, 1, 0xffff0000);
    struct Tri tri4 = create_tri(400, 200, 0, 1, 400, 400, 0, 1, 600, 400, 0, 1, 0xff00ff00);
    
    struct Tri tris[4] = {tri, tri2, tri3, tri4};
    */

    int x = 2;
    int y = 2;
    int z = 2;

    int tris_size = (x * y * (z + 1) + x * (y + 1) * z + (x + 1) * y * z) * 2;

    struct Tri *tris = tesselate_block(x, y, z, 200);

    for (int i = 0; i < 12; i++) {
        print_point(&tris[i].p3);
    }

    //int tris_size = sizeof(tris) / sizeof(tris[0]);

    printf("size_tris: %d\n", tris_size);

    while (game_running) {
        transform_geometry(tris, tris_size);
        scale = 1;

        render(tris, tris_size, true);
        //draw_rectangle();
        SDL_UpdateWindowSurface(screen);
        
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            scale_incr = 0.003;
                            //printf("step_x: %f\n", step_x);
                            break;
                        case SDLK_DOWN:
                            scale_incr = -0.003;
                            break;
                        case SDLK_ESCAPE:
                            game_running = 0;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            if (scale_incr > 0) {
                                scale_incr = 0;
                            }
                            break;
                        case SDLK_DOWN:
                            if (scale_incr < 0) {
                                scale_incr = 0;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_QUIT:
                    game_running = 0;
                    break;
                default:
                    break;
            }
        }
        //scale += scale_incr;
        //step_x = scale_incr;
        angle = scale_incr;
        //printf("scale: %f\n", scale);
    }

    free(tris);
}

int main(int argc, char **arg) {

    //renderer = SDL_CreateRenderer(screen, -1, 0);

    init_setup();
    game_loop();
    finish();
    return 0;
}
