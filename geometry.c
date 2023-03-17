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

void print_point(struct Point *p) {
    printf("%f\t", p->x);
    printf("%f\t", p->y);
    printf("%f\t", p->z);
    printf("%f\n", p->w);
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

struct Tri *tesselate_block(int nx, int ny, int nz, float size) {
    int n_tris = (nx * ny * (nz + 1) + nx * (ny + 1) * nz + (nx + 1) * ny * nz) * 2;
    printf("n tris: %d\n", n_tris);
    struct Tri *tris = (struct Tri*) malloc(sizeof(struct Tri) * n_tris);
    struct Point points[nx + 1][ny + 1][nz + 1];
    struct Point p1, p2, p3, p;
    struct Tri t;

    int i, j, k;
    
    for (i = 0; i < nx + 1; i++) {
        for (j = 0; j < ny + 1; j++) {
            for (k = 0; k < nz + 1; k++) {
                struct Point p = create_point(size * i, 
                                              size * j, 
                                              size * k, 1);
                //print_point(&p); 
                points[i][j][k] = p;
            }
        }
    }

    // XY PLANES

    int count = 0;

    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny; j++) {
            for (k = 0; k < nz + 1; k++) {

                p1 = points[i][j][k];
                p2 = points[i][j+1][k]; 
                p3 = points[i+1][j+1][k]; 

                t = create_tri(p1.x, p1.y, p1.z, p1.w,
                               p2.x, p2.y, p2.z, p2.w,
                               p3.x, p3.y, p3.z, p3.w, 
                               0xffffffff);

                tris[count++] = t;

                p1 = points[i][j][k];
                p2 = points[i+1][j+1][k];
                p3 = points[i+1][j][k];

                t = create_tri(p1.x, p1.y, p1.z, p1.w,
                               p2.x, p2.y, p2.z, p2.w,
                               p3.x, p3.y, p3.z, p3.w, 
                               0xffffffff);

                tris[count++] = t;
            }   
        }
    }
    //printf("count: %d\n", count);

    // YZ PLANES
    
    for (i = 0; i < nx + 1; i++) {
        for (j = 0; j < ny; j++) {
            for (k = 0; k < nz; k++) {

                p1 = points[i][j][k];
                p2 = points[i][j+1][k]; 
                p3 = points[i][j][k+1]; 

                t = create_tri(p1.x, p1.y, p1.z, p1.w,
                               p2.x, p2.y, p2.z, p2.w,
                               p3.x, p3.y, p3.z, p3.w, 
                               0xffffffff);
                tris[count++] = t;

                p1 = points[i][j][k+1];
                p2 = points[i][j+1][k];
                p3 = points[i][j+1][k+1];

                t = create_tri(p1.x, p1.y, p1.z, p1.w,
                               p2.x, p2.y, p2.z, p2.w,
                               p3.x, p3.y, p3.z, p3.w, 
                               0xffffffff);

                tris[count++] = t;
            }   
        }
    }

    //printf("count: %d\n", count);
    
    // XZ PLANES

    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny + 1; j++) {
            for (k = 0; k < nz; k++) {

                p1 = points[i][j][k];
                p2 = points[i+1][j][k]; 
                p3 = points[i+1][j][k+1]; 

                t = create_tri(p1.x, p1.y, p1.z, p1.w,
                               p2.x, p2.y, p2.z, p2.w,
                               p3.x, p3.y, p3.z, p3.w, 
                               0xffffffff);
                tris[count++] = t;

                p1 = points[i][j][k];
                p2 = points[i+1][j][k+1];
                p3 = points[i][j][k+1];

                t = create_tri(p1.x, p1.y, p1.z, p1.w,
                               p2.x, p2.y, p2.z, p2.w,
                               p3.x, p3.y, p3.z, p3.w, 
                               0xffffffff);
                
                tris[count++] = t;
            }   
        }
    }
    //printf("count: %d\n", count);
    return tris;
}

