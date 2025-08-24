#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "kr_two_ex_one.h"

#define WIDTH 50

double K(double a1, double a2) {
    double temp = pow(a1, 2) + pow(a2, 2);
    return sqrt(temp);
}

void draw_border() {
    printf("|");
    for (int i = 0; i < WIDTH; i++) printf("-");
    printf("|\n");
}

void draw_progress_bar(int current, int total) {
    int progress = (current * WIDTH) / total;
    printf("|");
    for (int i = 0; i < progress; i++) printf("%c", 176);
    for (int i = progress; i < WIDTH; i++) printf(" ");
    printf("| %3d%%\n", (current * 100) / total);
}

void sum(double (*K)(double, double), int n, double *sum) {
    double ai_1 = 3.14;
    double ai_2 = 1.57;
    *sum = ai_1 + ai_2;

    draw_border();
    printf("| Starting values:                              |\n");
    printf("|   a1 = %8.4lf                                 |\n", ai_1);
    printf("|   a2 = %8.4lf                                 |\n", ai_2);
    draw_border();

    double ai = 0;
    double k = K(ai_1, ai_2);

    for (int i = 0; i < n; i++) {
        ai = 2 * sin(k * ai_1) + 3 * cos(k * ai_2);
        ai_1 = ai_2;
        ai_2 = ai;
        *sum += ai;

        system("cls");

        draw_border();
        printf("|  a_1=3.14 a_2=1.57  k=( (a_1)^2 + (a_2)^2 )^1/2  |\n");
        printf("|         k = 2*sin(k*a_i-1) + 3*cos(k*a_i-2)      |\n");
        printf("|--------------------------------------------------|\n");
        printf("| Iteration %3d of %-3d                             |\n", i + 1, n);
        printf("| Current value: %10.4lf                        |\n", ai);
        printf("| Accumulated sum: %10.4lf                      |\n", *sum);
        draw_border();

        draw_progress_bar(i + 1, n);

        usleep(50000);
    }
}

void kr_two_ex_one(){
    double total_sum = 0;
    sum(K, 150, &total_sum);

    draw_border();
    printf("| Final result:                                    |\n");
    printf("|   Total sum = %10.4lf                         |\n", total_sum);
    draw_border();

    printf("Press q for exit\n");
    while(getchar() != 'q') ;
}
