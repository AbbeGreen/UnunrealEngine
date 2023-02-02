#!/usr/bin/env bash

gcc -o test test.c `sdl2-config --cflags --libs`
./test