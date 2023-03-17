#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "geometry.h"
#include "linalg.h"

void mm_sq(float m1[4][4], float m2[4][4], float m3[4][4]) {

    //printf("hello");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m3[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                //printf("%d\n", m1[i][k] * m2[k][j]);
                m3[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
}

void mm_vec(float m1[4], float m2[4][4], float m3[4]) {
    for (int i = 0; i < 4; i++) {
        m3[i] = 0;
        for (int j = 0; j < 4; j++) {
            m3[i] += m1[j] * m2[j][i];
        }
    }
}

void mm_t_vec(float m1[4][4], float m2[4], float m3[4]) {
    for (int i = 0; i < 4; i++) {
        m3[i] = 0;
        for (int j = 0; j < 4; j++) {
            m3[i] += m1[i][j] * m2[j];
        }
    }
}

void point2mat(struct Point *p, float m[]) {
    m[0] = p->x;
    m[1] = p->y;
    m[2] = p->z;
    m[3] = p->w;
}

void mat2point(float m[], struct Point *p) {
    p->x = m[0];
    p->y = m[1];
    p->z = m[2];
    p->w = m[3];
}

void transform(struct Point *p, float m[4][4], float ret[4]) {

    float tmp[4];

    point2mat(p, tmp);
    mm_t_vec(m, tmp, ret);
    mat2point(ret, p);

}

void transform_tri(struct Tri *t, float m[4][4], float ret[4]) {

    transform(&(*t).p1, m, ret);
    transform(&(*t).p2, m, ret);
    transform(&(*t).p3, m, ret);
}

/*
void projection(struct Tri *t) {
    
}
*/

/*
int main() {

    float a[4][4] = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};
    float m[4];
    float ret[4];
    struct Point p = create_point(1, 2, 3, 4);

    transform(&p, a, ret);

    for (int i = 0; i < 4; i++) {
        printf("%d: %f\t", i, ret[i]);
    }
    printf("\n");
}
*/