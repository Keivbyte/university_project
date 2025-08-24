#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "kr_two_ex_two.h"

#define N 50

void drawLine(char symbol, int length) {
    for(int i = 0; i < length; i++) printf("%c", symbol);
    printf("\n");
}

void kr_two_ex_two() {
    char filename[256];
    double data[N], sum = 0.0, avg, deviation[N];

    drawLine('=', 60);
    printf("|      Calculation of average humidity and deviations      |\n");
    drawLine('=', 60);

    printf("Enter the path to the data file: ");
    scanf("%255s", filename);

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Failed to open file '%s'.\n", filename);
        Sleep(2000);
        return;
    }

    for(int i = 0; i < N; i++) {
        if(fscanf(file, "%lf", &data[i]) != 1) {
            printf("Error: Failed to open file.\n");
            fclose(file);
            return;
        }
        sum += data[i];
    }
    fclose(file);

    avg = sum / N;

    for(int i = 0; i < N; i++) {
        deviation[i] = data[i] - avg;
    }

    drawLine('-', 60);
    printf("| # |   Value    | Deviation from the mean |\n");
    drawLine('-', 60);

    for(int i = 0; i < N; i++) {
        printf("| %2d|%10.2f  | %21.2f   |\n", i+1, data[i], deviation[i]);
        Sleep(400);
    }

    drawLine('=', 60);
    printf("Average humidity value: %.2f\n", avg);
    drawLine('=', 60);

    printf("Press Enter for exit...");
    getchar(); getchar();
}
