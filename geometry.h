#ifndef GEOMETRY_H
#define GEOMETRY_H

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

struct Point create_point(float x, float y, float z, float w);

struct Tri create_tri(float x1, float y1, float z1, float w1, float x2, float y2, float z2, float w2, float x3, float y3, float z3, float w3, int c);

void print_point(struct Point *p);

int interpolate_x(struct Point p1, struct Point p2, int y);

int interpolate_y(struct Point p1, struct Point p2, int x);

struct Point *sort_points(struct Point *points, char attr);

struct Tri *tesselate_block(int nx, int ny, int nz, float size);

#endif