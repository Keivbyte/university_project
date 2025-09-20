#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <conio.h>

typedef struct {
    int row;
    int col;
} Position;


void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y + 1, x * 5 + 2);
    fflush(stdout);
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void printMatrixWithCursor(int rows, int cols, float **matrix, Position cursor, const char *inputBuffer) {
    clearScreen();

    printf("+");
    for (int j = 0; j < cols; j++) printf("----+");
    printf("\n");

    for (int i = 0; i < rows; i++) {
        printf("|");
        for (int j = 0; j < cols; j++) {
            if (i == cursor.row && j == cursor.col) {
                if (strlen(inputBuffer) > 0) {
                    printf("%*s |", 3, inputBuffer); // show the injected line
                } else {
                    printf("  ? |");
                }
            } else if (matrix[i][j] != 0.0f || (i < cursor.row || (i == cursor.row && j < cursor.col))) {
                printf("%2.1f |", matrix[i][j]);
            } else {
                printf("    |");
            }
        }
        printf("\n+");
        for (int j = 0; j < cols; j++) printf("----+");
        printf("\n");
    }

    gotoxy(cursor.col, cursor.row * 2 + 1);
}

float **createMatrix(int rows, int cols) {
    float **matrix = (float **)malloc(rows * sizeof(float *));
    for (int i = 0; i < rows; i++)
        matrix[i] = (float *)calloc(cols, sizeof(float));
    return matrix;
}

void freeMatrix(float **matrix, int rows) {
    for (int i = 0; i < rows; i++)
        free(matrix[i]);
    free(matrix);
}

void normalizeMatrix(float **matrix, int rows, int cols) {
    float *sumRow = (float *)calloc(rows, sizeof(float));
    float *sumCol = (float *)calloc(cols, sizeof(float));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            sumRow[i] += matrix[i][j];
            sumCol[j] += matrix[i][j];
        }
    }

    float **normalized = (float **)malloc(rows * sizeof(float *));
    for (int i = 0; i < rows; i++) {
        normalized[i] = (float *)malloc(cols * sizeof(float));
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            float rowSumWithout = sumRow[i] - matrix[i][j];
            float colSumWithout = sumCol[j] - matrix[i][j];

            if (colSumWithout == 0.0f) {
                normalized[i][j] = 0.0f;
                fprintf(stderr, "Warning: Division by zero in cell [%d][%d], column sum is zero.\n", i, j);
            } else {
                normalized[i][j] = rowSumWithout / colSumWithout;
            }
        }
    }

    clearScreen();

    printf("Original Matrix:\n");
    printf("+");
    for (int j = 0; j < cols; j++) printf("----+");
    printf("\n");

    for (int i = 0; i < rows; i++) {
        printf("|");
        for (int j = 0; j < cols; j++) {
            printf("%2.1f |", matrix[i][j]);
        }
        printf("\n+");
        for (int j = 0; j < cols; j++) printf("----+");
        printf("\n");
    }

    printf("\nNormalized Matrix:\n");
    printf("+");
    for (int j = 0; j < cols; j++) printf("----+");
    printf("\n");

    for (int i = 0; i < rows; i++) {
        printf("|");
        for (int j = 0; j < cols; j++) {
            printf("%2.1f |", normalized[i][j]);
        }
        printf("\n+");
        for (int j = 0; j < cols; j++) printf("----+");
        printf("\n");
    }

    PressToNext();

    for (int i = 0; i < rows; i++) {
        free(normalized[i]);
    }
    free(normalized);

    free(sumRow);
    free(sumCol);
}

void kr_three_ex_one() {
    int rows, cols;

    printf("Enter number of rows: ");
    scanf("%d", &rows);
    printf("Enter number of columns: ");
    scanf("%d", &cols);

    while (getchar() != '\n');

    float **matrix = createMatrix(rows, cols);
    Position cursor = {0, 0};

    char input[64];

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cursor.row = i;
            cursor.col = j;

            input[0] = '\0';

            while (1) {
                printMatrixWithCursor(rows, cols, matrix, cursor, input);

                char ch = getch();

                if (ch == '\n' || ch == '\r') {
                    if (strlen(input) == 0) continue;
                    sscanf(input, "%f", &matrix[i][j]);
                    break;
                } else if (ch == 127 || ch == 8) {
                    if (strlen(input) > 0)
                        input[strlen(input) - 1] = '\0';
                } else if ((isdigit(ch)) || ch == '.' || ch == '-') {
                    if (strlen(input) < 5) {
                        strncat(input, &ch, 1);
                    }
                }
            }
        }
    }

    clearScreen();
    printf("Completed matrix:\n");
    printf("+");
    for (int j = 0; j < cols; j++) printf("----+");
    printf("\n");

    for (int i = 0; i < rows; i++) {
        printf("|");
        for (int j = 0; j < cols; j++) {
            printf("%2.1f |", matrix[i][j]);
        }
        printf("\n+");
        for (int j = 0; j < cols; j++) printf("----+");
        printf("\n");
    }

    printf("Press Enter to normalize the matrix \n ");
    getchar();
    normalizeMatrix(matrix, rows, cols);
}
