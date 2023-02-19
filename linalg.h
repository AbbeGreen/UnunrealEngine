#ifndef LINALG_H
#define LINALG_H

void mm_sq(float m1[4][4], float m2[4][4], float m3[4][4]);

void mm_vec(float m1[4], float m2[4][4], float m3[4]);

void point2mat(struct Point *p, float m[]);

void mat2point(float m[], struct Point *p);

void transform_tri(struct Tri *t, float m[4][4], float ret[4]);

void transform(struct Point *p, float m[4][4], float ret[4]);

#endif