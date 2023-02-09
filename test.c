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

#define QUITKEY SDLK_ESCAPE
#define WIDTH 1024
#define HEIGHT 720

SDL_Window *screen = NULL;
SDL_Renderer *renderer;
SDL_Event event;
SDL_Rect source, destination, dst;

int key_pressed;
int rectCount = 0;

struct Point {
    int x;
    int y;
    int z;
    char name;
};

struct Tri {
    struct Point p1;
    struct Point p2;
    struct Point p3;
    int c;
};

struct Point create_point(int x, int y, int z, char name) {

    struct Point p;

    p.x = x;
    p.y = y;
    p.z = z;
    p.name = name;

    return p;

}

struct Tri create_tri(int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3, int c) {

    struct Tri t;

    t.p1 = create_point(x1, y1, z1, 'a');
    t.p2 = create_point(x2, y2, z2, 'b');
    t.p3 = create_point(x3, y3, z3, 'c');
    t.c = c;

    return t;
}

int interpolate_x(struct Point p1, struct Point p2, int y) {
    
    int x;
    
    x = (int)(p1.x + ((p2.x - p1.x) * (y - p1.y)) / (p2.y - p1.y));

    // this can be heavily optimized but is left for now.

    return x;

}

int interpolate_y(struct Point p1, struct Point p2, int x) {
    
    int y;

    y = (int)(p1.y + ((p2.y - p1.y) * (x - p1.x)) / (p2.x - p1.x));

    return y;

}

struct Point *sort_points(struct Point *points, char attr) {

    struct Point *sorted = (struct Point*) malloc(sizeof(struct Point) * 3);

    for (int i = 0; i < 3; i++) {
        sorted[i] = points[i];
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            if (attr == 'x') {
                if (sorted[j].x >= sorted[j+1].x) {
                    struct Point tmp = sorted[j];
                    sorted[j] = sorted[j+1];
                    sorted[j+1] = tmp;
                }
            } else if (attr == 'y') {
                if (sorted[j].y >= sorted[j+1].y) {
                    struct Point tmp = sorted[j];
                    sorted[j] = sorted[j+1];
                    sorted[j+1] = tmp;
                }
            } else if (attr == 'z') {
                if (sorted[j].z >= sorted[j+1].z) {
                    struct Point tmp = sorted[j];
                    sorted[j] = sorted[j+1];
                    sorted[j+1] = tmp;
                }
            }
        }
    }
    return sorted;
}

int rasterize_triangle(struct Tri tri, int offset) {

    struct Point to_sort[3] = {tri.p1, tri.p2, tri.p3};

    struct Point *x_sorted = sort_points(to_sort, 'x');
    struct Point *y_sorted = sort_points(to_sort, 'y');

    SDL_Surface *window_surface = SDL_GetWindowSurface(screen);
    unsigned int *pixels = window_surface->pixels;
    int width = window_surface->w;

    struct Point *top, *middle, *bottom;
    int from, to;

    // facedness is defined as the direction of (the normal of) the long side
    bool right_facing;

    top = &y_sorted[0];
    middle = &y_sorted[1];
    bottom = &y_sorted[2];

    right_facing = middle->x <= top->x;

    for (int i = top->y; i < middle->y; i += offset) {
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
    for (int i = middle->y; i < bottom->y; i += offset) {
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

    int offset = 1; //pixel size

    for (int i = 0; i < n_tris; i++) {
        rasterize_triangle(tris[i], offset);
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

    struct Tri tri = create_tri(400, 200, 0, 200, 400, 0, 400, 400, 0, 0xff00ffff);
    struct Tri tri2 = create_tri(200, 400, 0, 400, 400, 0, 400, 600, 0, 0x00ff00ff);
    struct Tri tri3 = create_tri(400, 400, 0, 400, 600, 0, 600, 400, 0, 0xffff0000);
    struct Tri tri4 = create_tri(400, 200, 0, 400, 400, 0, 600, 400, 0, 0xff00ff00);
    struct Tri tris[4] = {tri, tri2, tri3, tri4};

    int tris_size = sizeof(tris) / sizeof(tris[0]);

    while (game_running) {

        render(tris, tris_size);
        //draw_rectangle();
        SDL_UpdateWindowSurface(screen);
        
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    key_pressed = event.key.keysym.sym;
                    if (key_pressed == QUITKEY) {
                        game_running = 0;
                        break;
                    }
                    break;
                case SDL_QUIT:
                    game_running = 0;
                    break;
                case SDL_KEYUP:
                    break;
            }
        }
    }
}

int main(int argc, char **arg) {

    init_setup();
    game_loop();
    finish();
    return 0;
}
