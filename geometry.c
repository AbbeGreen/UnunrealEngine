#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "geometry.h"

/*
struct Point {
    float x;
    float y;
    float z;
    float w;
};

struct Tri {
    struct Point p1;
    struct Point p2;
    struct Point p3;
    int c;
};
*/

struct Point create_point(float x, float y, float z, float w) {

    struct Point p;

    p.x = x;
    p.y = y;
    p.z = z;
    p.w = w;

    return p;

}

struct Tri create_tri(float x1, float y1, float z1, float w1, float x2, float y2, float z2, float w2, float x3, float y3, float z3, float w3, int c) {

    struct Tri t;

    t.p1 = create_point(x1, y1, z1, w1);
    t.p2 = create_point(x2, y2, z2, w2);
    t.p3 = create_point(x3, y3, z3, w3);
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