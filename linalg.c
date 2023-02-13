#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

void mm(int m1[][2], int m2[][2], int m3[][2], int rows, int cols) {

    int m = rows;
    int n = cols;

    if (m == n) {
        //printf("hello");
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                m3[i][j] = 0;
                for (int k = 0; k < n; k++) {
                    //printf("%d\n", m1[i][k] * m2[k][j]);
                    m3[i][j] += m1[i][k] * m2[k][j];
                }
            }
        }
    }
}

int main() {

    int a[2][2] = {{1, 2}, {3, 4}};
    int b[2][2] = {{1, 2}, {3, 4}};    
    int c[2][2];

    mm(a, b, c, 2, 2);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            printf("elem at %d, %d: %d\n", i, j, c[i][j]);
        }
    } 
}