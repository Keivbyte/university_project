#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <math.h>
#include "kr_one_ex_one.h"

#define WIDTH 41
#define HEIGHT 11

char map[HEIGHT][WIDTH + 1];
char *equation = "   b=(c*(y^0.5+x^2))^0.5*(cos(x)-|c-y|)";

char values[4][20] = {
    "  c = ",
    "  y = ",
    "  x = ",
    "  b = " };

const char *input_prompts[4] = {
    "  Enter:c, enter->next, out->q",
    "  Enter:y, enter->next, out->q",
    "  Enter:x, enter->next, out->q",
    "  write:b, enter->next, out->q" };

void init_map() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if( i == 0 || i == HEIGHT - 1 )
                map[i][j] = '-';
            else
                map[i][j] = ' ';
        }
        map[i][WIDTH] = '\0';
    }
}

void put_ex(int current_row) {
    strncpy(map[1], equation, strlen(equation));
    strncpy(map[3], input_prompts[current_row], WIDTH);
    for (int i = 0; i < 4; i++) {
        strncpy(map[5 + i], values[i], WIDTH);
    }
}

void show_map() {
    system("cls");
    for (int i = 0; i < HEIGHT; i++) {
        printf("%s\n", map[i]);
    }
}

void calculate_b() {
    double c, y, x, b;

    if (sscanf(values[0] + 5, "%lf", &c) != 1 ||
        sscanf(values[1] + 5, "%lf", &y) != 1 ||
        sscanf(values[2] + 5, "%lf", &x) != 1) {
        printf("Error: Invalid input detected.\n");
        return;
    }

    if (y < 0) {
        printf("Error: y cannot be negative for square root.\n");
        snprintf(values[3] + 5, 15, "ERROR");
        return;
    }

    double sqrt_y = sqrt(y);
    double x_squared = x * x;
    double numerator = sqrt(c * (sqrt_y + x_squared));
    double denominator = cos(x) - fabs(c - y);

    b = numerator * denominator;

    snprintf(values[3] + 5, 15, "%.2f", b);
}

void kr_one_ex_one() {
    init_map();
    int current_row = 0;
    int position = 5;
    char ch;

    while (1) {
        put_ex(current_row);
        show_map();

        char input[6] = {'\0'};
        int input_len = 0;
        int is_negative = 0;

        while (1) {
            ch = getch();

            if (ch == '-' && input_len == 0) {
                is_negative = 1;
                input[input_len++] = ch;
                values[current_row][position + input_len - 1] = ch;
                values[current_row][position + input_len] = '\0';
                put_ex(current_row);
                show_map();
            } else if (ch >= '0' && ch <= '9') {
                if (input_len < 5) {
                    input[input_len++] = ch;
                    values[current_row][position + input_len - 1] = ch;
                    values[current_row][position + input_len] = '\0';
                    put_ex(current_row);
                    show_map();
                }
            } else if (ch == '.' && strchr(input, '.') == NULL) {
                if (input_len < 5) {
                    input[input_len++] = ch;
                    values[current_row][position + input_len - 1] = ch;
                    values[current_row][position + input_len] = '\0';
                    put_ex(current_row);
                    show_map();
                }
            } else if (ch == '\b' && input_len > 0) {
                if (input[--input_len] == '-') {
                    is_negative = 0;
                }
                input[input_len] = '\0';
                values[current_row][position + input_len] = ' ';
                put_ex(current_row);
                show_map();
            } else if (ch == '\r') {
                if (input_len > 0)
                    break;
            } else if (ch == 'q') {
                printf("Exiting program.\n");
                return;
            }
        }

        current_row = (current_row + 1) % 3;

        if (current_row == 0)
            calculate_b();
    }
}
