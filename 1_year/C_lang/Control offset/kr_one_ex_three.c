#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <windows.h>
#include "kr_one_ex_three.h"

#define WIDTH 40
#define HEIGHT 20
#define PI 3.141592

char map[HEIGHT][WIDTH + 1];

typedef struct SObject {
    int x, y;
    char *name_object;
} TObject;

void setting_an_object_by_coordinates(TObject obj) {
    int k = 0;
    while (obj.name_object[k] != '\0' && obj.x + k < WIDTH - 1) {
        map[obj.y][obj.x + k] = obj.name_object[k];
        k++;
    }
}

void init(int num_objects, ...) {
    for (int j = 0; j < HEIGHT; j++) {
        for (int i = 0; i < WIDTH; i++) {
            if (j == 0 && (i == 0 || i == WIDTH - 1)) {
                map[j][i] = '+';
            } else if (j == HEIGHT - 1 && (i == 0 || i == WIDTH - 1)) {
                map[j][i] = '+';
            } else if (j == 0 || j == HEIGHT - 1) {
                map[j][i] = '-';
            } else if (i == 0 || i == WIDTH - 1) {
                map[j][i] = '|';
            } else {
                map[j][i] = ' ';
            }
        }
        map[j][WIDTH] = '\0';
    }

    va_list args;
    va_start(args, num_objects);

    for (int i = 0; i < num_objects; i++) {
        TObject obj = va_arg(args, TObject);
        setting_an_object_by_coordinates(obj);
    }
    va_end(args);
}

void show() {
    system("cls");
    for (int j = 0; j < HEIGHT; j++)
        printf("%s\n", map[j]);
}

double upper_function(double x) {
    return (exp(x) * (1 + sin(x))) / (1 + cos(x));
}

double down_function(double x) {
    return exp(x) * tan(x / 2);
}

double resolve_equations(double x) {
    return (x < 0) ? upper_function(x) : down_function(x);
}

void kr_one_ex_three() {
    char *welcome = "Welcome to Exercise 3";
    TObject greetings = {(WIDTH - strlen(welcome)) / 2, 1, welcome};
    TObject equation_up = {2, 3, "if  x<0: e^x*(1+sin x)/(1+cos x)"};
    TObject equation_down = {2, 4, "if x>=0: e^x*tan(x/2)"};
    TObject line = {1, 5, "__________________________"};
    TObject text = {1, 6, "count|   x    |     y    |"};

    init(5, greetings, equation_up, equation_down, line, text);
    show();
    int y_position = 7;
    int row_number = 1;

    for (double x = -PI / 4; x <= PI / 4; x += PI / 20) {
        double y = resolve_equations(x);

        char result[50];
        sprintf(result, "%3d | %6.3f | %8.3f |", row_number, x, y);

        TObject current_result = {2, y_position, result};
        setting_an_object_by_coordinates(current_result);

        show();
        y_position++;
        row_number++;
        Sleep(500);
    }
    printf("Press anything for exit\n");
        getchar();
        getchar();
}
