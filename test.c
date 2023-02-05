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
    char name;
};

struct Tri {
    struct Point p1;
    struct Point p2;
    struct Point p3;
};

struct Point create_point(int x, int y, char name) {

    struct Point p;

    p.x = x;
    p.y = y;
    p.name = name;

    return p;

}

struct Tri create_tri(int x1, int y1, int x2, int y2, int x3, int y3) {

    struct Tri t;

    t.p1 = create_point(x1, y1, 'a');
    t.p2 = create_point(x2, y2, 'b');
    t.p3 = create_point(x3, y3, 'c');

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

int rasterize_triangle(struct Tri tri) {
    // define left- or right-facing triangle
    struct Point a = tri.p1;
    struct Point b = tri.p2;
    struct Point c = tri.p3;

    struct Point x_sorted[3] = {a, b, c};
    struct Point y_sorted[3] = {a, b, c};

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            if (x_sorted[j].x >= x_sorted[j+1].x) {
                struct Point tmp = x_sorted[j];
                x_sorted[j] = x_sorted[j+1];
                x_sorted[j+1] = tmp;
            }    
        }
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            if (y_sorted[j].y >= y_sorted[j+1].y) {
                struct Point tmp = y_sorted[j];
                y_sorted[j] = y_sorted[j+1];
                y_sorted[j+1] = tmp;
            }    
        }
    }
    /*
    for (int i = 0; i < 3; i++) {
        printf("x sorted:\n");
        printf("%d\t%c\n", x_sorted[i].x, x_sorted[i].name);
        
    }

    for (int i = 0; i < 3; i++) {
        printf("y sorted:\n");
        printf("%d\t%c\n", y_sorted[i].y, y_sorted[i].name);
    }

    */

    SDL_Surface *window_surface = SDL_GetWindowSurface(screen);
    unsigned int *pixels = window_surface->pixels;
    int width = window_surface->w;

    struct Point *top, *middle, *bottom;

    // facedness is defined as the direction of (the normal of) the long side
    bool right_facing;

    top = &y_sorted[0];
    middle = &y_sorted[1];
    bottom = &y_sorted[2];

    right_facing = middle->x < bottom->x;

    if (right_facing) {
        for (int i = top->y; i < middle->y; i++) {
            int from = interpolate_x(*top, *middle, i);
            int to = interpolate_x(*top, *bottom, i);
            //printf("from: %d\tto:%d\n", from, to);
            for (int j = from; j < to; j++) {
                pixels[j + i * width] = 0xff4fffa0;
            }
        }
        for (int i = middle->y; i < bottom->y; i++) {
            int from = interpolate_x(*middle, *bottom, i);
            int to = interpolate_x(*top, *bottom, i);
            for (int j = from; j < to; j++) {
                pixels[j + i * width] = 0xff4fffa0;
            }
        }
    }

    return 0;

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

void draw_rectangle() {
    SDL_RenderClear(renderer);
    char buff[20];
    SDL_Rect rect;
    rect.h = 120;
    rect.w = 120;
    rect.y = HEIGHT/2;
    rect.x = WIDTH/2;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderPresent(renderer);
}

void game_loop(struct Tri tri) {

    int game_running = 1;

    /*
    for (int i = 100; i < 300; i++) {
        for (int j = 100; j < 300; j++) {
            pixels[j + i * width] = 0xff4fffa0;
        }
    }
    */


    while (game_running) {
        //draw_rectangle();
        
        rasterize_triangle(tri);

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
    
    //printf("hello");
    struct Tri tri = create_tri(300, 300, 150, 450, 500, 500);

    init_setup();
    game_loop(tri);
    finish();
    return 0;
}
