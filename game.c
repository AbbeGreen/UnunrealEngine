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

    right_facing = middle->x <= top->x;

    for (int i = (int)top->y; i < (int)middle->y; i += offset) {
        if (right_facing) {

            from = interpolate_x(*top, *middle, i);
            to = interpolate_x(*top, *bottom, i);

        } else {

            from = interpolate_x(*top, *bottom, i);
            to = interpolate_x(*top, *middle, i);
        }
        for (int o = 0; o < offset; o++) {
            for (int j = from; j <= to; j++) {
                pixels[j + (i + o) * width] = tri.c;
            }
        }
    }
    for (int i = (int)middle->y; i < (int)bottom->y; i += offset) {
        if (right_facing) {
            
            from = interpolate_x(*middle, *bottom, i);
            to = interpolate_x(*top, *bottom, i);

        } else {

            from = interpolate_x(*top, *bottom, i);
            to = interpolate_x(*middle, *bottom, i);
        }
        for (int o = 0; o < offset; o++) {
            for (int j = from; j <= to; j++) {
                pixels[j + (i + o) * (width)] = tri.c;
            }
        }
    }

    return 0;

}

void render(struct Tri *tris, int n_tris) {

    // DO RENDERING THINGS

    // SET UP SURFACE AND DEFINE WINDOW PARAMS

    SDL_Surface *window_surface = SDL_GetWindowSurface(screen);
    unsigned int *pixels = window_surface->pixels;
    int width = window_surface->w;

    for (size_t i = 0; i < sizeof pixels; i++) {
        pixels[i] = 0;
    }

    int offset = 1; //pixel size
  
    for (int i = 0; i < n_tris; i++) {
        rasterize_triangle(tris[i], offset, pixels, width);
    }
}

void transform_geometry(struct Tri *tris, int n_tris) {

    float ret[4];

    float scale_m[4][4] = {{scale, 0, 0, 0}, 
                           {0, scale, 0, 0}, 
                           {0, 0, scale, 0}, 
                           {0, 0, 0,     0}};
 

    for (int i = 0; i < n_tris; i++) {

        transform_tri(tris + i, scale_m, ret);
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

    struct Tri tri = create_tri(400, 200, 0, 0, 200, 400, 0, 0, 400, 400, 0, 0, 0xff00ffff);
    struct Tri tri2 = create_tri(200, 400, 0, 0, 400, 400, 0, 0, 400, 600, 0, 0, 0x00ff00ff);
    struct Tri tri3 = create_tri(400, 400, 0, 0, 400, 600, 0, 0, 600, 400, 0, 0, 0xffff0000);
    struct Tri tri4 = create_tri(400, 200, 0, 0, 400, 400, 0, 0, 600, 400, 0, 0, 0xff00ff00);
    struct Tri tris[4] = {tri, tri2, tri3, tri4};

    int tris_size = sizeof(tris) / sizeof(tris[0]);

    printf("coordinates: %f, %f, %f, %f\n", tris[0].p1.x, tris[0].p1.y, tris[0].p1.z, tris[0].p1.w);
    printf("coordinates: %f, %f, %f, %f\n", tris[0].p1.x, tris[0].p1.y, tris[0].p1.z, tris[0].p1.w);
    while (game_running) {
        transform_geometry(tris, tris_size);
        scale = 1;

        render(tris, tris_size);
        //draw_rectangle();
        SDL_UpdateWindowSurface(screen);
        
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            scale_incr = 0.0005;
                            printf("hello\n");
                            break;
                        case SDLK_DOWN:
                            scale_incr = -0.0005;
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
        scale += scale_incr;
        //printf("scale: %f\n", scale);
    }
}

int main(int argc, char **arg) {

    renderer = SDL_CreateRenderer(screen, -1, 0);

    init_setup();
    game_loop();
    finish();
    return 0;
}
