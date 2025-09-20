#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "kr_one_ex_two.h"

#define False 0
#define True 1

const char *first_equation = "  ln(x/y) + (x^2 + y)^3, x/y > 0";
const char *second_equation = "  ln|x/y| + (x^2 + y)^3, x/y < 0";
const char *third_equation = "  (x^2 + y)^3, y != 0, x = 0";

double first_equation_count(double x, double y, double temp_x_div_y) {
    return log(temp_x_div_y) + pow((x * x + y), 3);
}

double second_equation_count(double x, double y, double temp_x_div_y) {
    return log(fabs(temp_x_div_y)) + pow((x * x + y), 3);
}

double third_equation_count(double x, double y) {
    return pow((x * x + y), 3);
}

double double_input(const char *prompt) {
    printf("Please input %s: ", prompt);
    double value;
    while (scanf("%lf", &value) != 1) {
        printf("You input an invalid value! Try again:\n");
        while (getchar() != '\n') ;
    }
    return value;
}

void explain_answer(char flag, double answer) {
    if ((flag & 0b1000) == 0b1000)
        printf("Answer is %.2lf, because x/y > 0\n", answer);
    else if ((flag & 0b0100) == 0b0100)
        printf("Answer is %.2lf, because x/y < 0\n", answer);
    else if ((flag & 0b0010) == 0b0010)
        printf("Answer is %.2lf, because y != 0 and x = 0\n", answer);
    else if ((flag & 0b0001) == 0b0001)
        printf("Answer is %.2lf, because y = 0\n", answer);
    else
        printf("Error: No conditions met.\n");
}

void kr_one_ex_two() {
    printf("kr_one_ex_two function called!\n"); // film printing
    double input_x, input_y;
    double answer;
    char flag, exit;
    system("cls");
    while (True) {
        flag = False;
        printf("This is task number 2 on solving the equation\n");
        printf("%s\n%s\n%s\n", first_equation, second_equation, third_equation);

        input_y = double_input("y");
        if (input_y == 0) {
            answer = 0;
            flag |= 0b0001;
        } else {
            input_x = double_input("x");
            double temp_x_div_y = input_x / input_y;

            if (input_x == 0) {
                answer = third_equation_count(input_x, input_y);
                flag |= 0b0010;
            } else if (temp_x_div_y > 0) {
                answer = first_equation_count(input_x, input_y, temp_x_div_y);
                flag |= 0b1000;
            } else if (temp_x_div_y < 0) {
                answer = second_equation_count(input_x, input_y, temp_x_div_y);
                flag |= 0b0100;
            }
        }

        explain_answer(flag, answer);

        printf("Press Enter to continue...or 'q' for exit\n");
        while (getchar() != '\n'); // cleaning input buffer
        if (getchar() == 'q') break;
        system("cls");
    }
    printf("Bye-bye ;)");
}
