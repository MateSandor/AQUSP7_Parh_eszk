#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>



void solve_cubic_equation(float a, float b, float c, float d);

int main() {
    srand(time(0));
    int N = 10;
    printf("START\n");
    float a, b, c, d;
	
    while (N != 100000000) {
        int i = 0;
        clock_t start, end, temp;
        start = clock();
        while (i < N) {
            do {
                a = (float)(rand() % 2000 - 1000) / 100;
                b = (float)(rand() % 2000 - 1000) / 100;
                c = (float)(rand() % 2000 - 1000) / 100;
                d = (float)(rand() % 2000 - 1000) / 100;
            } while (a == 0 || b == 0 || c == 0 || d == 0);
            solve_cubic_equation(a, b, c, d);
            i++;
			
        }
        end = clock();
        printf("\n\n%d:  Overall runtime:\t%lf\n", N, ((float)(end - start)) / CLOCKS_PER_SEC);
        N *= 10;
    }
    return 0;
}

void solve_cubic_equation(float a, float b, float c, float d) {
    float p, q, r, x1, x2, x3;

    p = b / a;
    q = c / a;
    r = d / a;

    float Q = (pow(p, 2) - 3 * q) / 9;
    float R = (2 * pow(p, 3) - 9 * p * q + 27 * r) / 54;
    float D = pow(Q, 3) - pow(R, 2);

    if (D > 0) {
        float theta = acos(R / sqrt(pow(Q, 3)));
        x1 = -2 * sqrt(Q) * cos(theta / 3) - p / 3;
        x2 = -2 * sqrt(Q) * cos((theta + 2 * M_PI) / 3) - p / 3;
        x3 = -2 * sqrt(Q) * cos((theta - 2 * M_PI) / 3) - p / 3;
        //printf("x1 = %lf\nx2 = %lf\nx3 = %lf\n", x1, x2, x3);
    } else if (D == 0) {
        x1 = -2 * cbrt(R) - p / 3;
        x2 = cbrt(R) - p / 3;
        //printf("x1 = %lf (triple root)\nx2 = %lf\n", x1, x2);
    } else {
        float alpha = cbrt(sqrt(pow(R, 2) - pow(Q, 3)) + fabs(R));
        float beta = Q / alpha;
        if (R > 0) {
            x1 = -p / 3 + alpha + beta;
            x2 = -p / 3 - 0.5 * (alpha + beta);
            x3 = x2;
            //printf("x1 = %lf\nx2 = %lf (float root)\nx3 = %lf (float root)\n", x1, x2, x3);
        } else {
            x1 = -p / 3 + alpha + beta;
            x2 = -p / 3 - alpha - beta;
            x3 = x2;
            //printf("x1 = %lf\nx2 = %lf\nx3 = %lf (float root)\n", x1, x2, x3);
        }
    }
}