#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "kr_two_ex_three.h"

#define ROWS 4
#define COLS 5


void PressToNext(){
    printf("Press Enter for next step -> ");
    getchar();
}
void drawHorizontalLine(int width) {
    printf("+");
    for (int i = 0; i < width - 2; i++) {
        printf("-");
    }
    printf("+\n");
}

void drawBottomLine(int width) {
    printf("+");
    for (int i = 0; i < width - 2; i++) {
        printf("-");
    }
    printf("+\n");
}

void drawTitle(const char *title, int width) {
    int padding = (width - 2 - strlen(title)) / 2;
    printf("|");
    for (int i = 0; i < padding; i++) {
        printf(" ");
    }
    printf("%s", title);
    for (int i = 0; i < padding + (width - 2 - strlen(title)) % 2; i++) {
        printf(" ");
    }
    printf("|\n");
}

void drawMatrixRow(float *row, int cols, int width) {
    printf("| ");
    for (int j = 0; j < cols; j++) {
        printf("%6.2f ", row[j]);
    }
    printf("|\n");
}

void drawMatrix(float matrix[ROWS][COLS], const char *title, int width) {
    drawHorizontalLine(width);
    drawTitle(title, width);
    drawHorizontalLine(width);

    for (int i = 0; i < ROWS; i++) {
        drawMatrixRow(matrix[i], COLS, width);
    }

    drawBottomLine(width);
}

void drawArray(float array[], int size, const char *title, int width) {
    drawHorizontalLine(width);
    drawTitle(title, width);
    drawHorizontalLine(width);

    printf("| ");
    for (int i = 0; i < size; i++) {
        if (array[i] != 0)
            printf("%6.2f ", array[i]);
    }
    printf("|\n");

    drawBottomLine(width);
}

void drawProcessing(float matrix[ROWS][COLS], int currentRow, float max, int maxCol, float firstElement) {
    printf("\n");
    printf("+------------------------------------+\n");
    printf("| String processing %d                |\n", currentRow + 1);
    printf("|------------------------------------|\n");
    printf("| Current line:                      |\n");
    printf("| ");
    for (int j = 0; j < COLS; j++) {
        printf("%6.2f ", matrix[currentRow][j]);
    }
    printf("|\n");
    printf("| Maximum element: %6.2f (column %d) |\n", max, maxCol + 1);
    printf("| First element: %6.2f              |\n", firstElement);
    if (firstElement != 0) {
        printf("| First element is saved to the array|\n");
    } else {
        printf("| First element is 0, do not save     |\n");
    }
    printf("| Replace first element with max one |\n");
    printf("+------------------------------------+\n");

    PressToNext();
}

void kr_two_ex_three(){
    float matrix[ROWS][COLS];
    float originalMatrix[ROWS][COLS];
    float firstElements[ROWS] = {0};
    int firstElementCount = 0;

    FILE *file = fopen("matrix.txt", "r");

    if (file == NULL) {
        printf("Error: failed to open the file matrix.txt!\n");
        return 1;
    }

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (j < COLS - 1) {
                fscanf(file, "%f;", &matrix[i][j]);
            } else {
                fscanf(file, "%f\n", &matrix[i][j]);
            }
            originalMatrix[i][j] = matrix[i][j];
        }
    }

    fclose(file);


    system("cls");
    drawMatrix(matrix, "Original matrix", 38);


    for (int i = 0; i < ROWS; i++){
        float max = matrix[i][0];
        int maxCol = 0;

        for (int j = 1; j < COLS; j++){
            if (matrix[i][j] > max){
                max = matrix[i][j];
                maxCol = j;
            }
        }


        float firstElement = matrix[i][0];
        if (firstElement != 0) {
            firstElements[firstElementCount++] = firstElement;
        }

        drawProcessing(matrix, i, max, maxCol, firstElement);

        matrix[i][0] = max;
    }

    system("cls");

    drawMatrix(originalMatrix, "Original matrix", 38);
    printf("\n");

    drawMatrix(matrix, "Transformed matrix", 38);
    printf("\n");

    PressToNext();

    drawArray(firstElements, firstElementCount, "Array of first elements (not equal to zero)", 50);

    PressToNext();
}
