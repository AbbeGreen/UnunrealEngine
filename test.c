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
};

struct Point create_point(int x, int y) {

    struct Point p;

    p.x = x;
    p.y = y;

    return p;

}

void init_setup() {
    srand((int)time(NULL));
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_SHOWN, &screen, &renderer);
    if (!screen) {
        printf("Failed to create window!");
    }

    SDL_Surface *window_surface = SDL_GetWindowSurface(screen);
    unsigned int *pixels = window_surface->pixels;
    int x = 10;
    int y = 30;

    int width = window_surface->w;
    pixels[x + y * width] = 0xffffffff;
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

void game_loop() {
    int game_running = 1;

    struct Point p1 = create_point(1, 2);

    while (game_running) {
        draw_rectangle();

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
