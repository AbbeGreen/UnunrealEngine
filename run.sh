#!/usr/bin/env bash

gcc -o game game.c geometry.c linalg.c -lm `sdl2-config --cflags --libs`
./game