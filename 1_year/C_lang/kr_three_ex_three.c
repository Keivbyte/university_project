#include <stdio.h>
#include <stdlib.h>

typedef int (*SumFunc)(int, int);

int add(int a, int b) {
    return a + b;
}

int fold(const int *arr, int size, SumFunc fn) {
    if (size <= 0) {
        fprintf(stderr, "Error: Array size must be greater than zero.\n");
        return 0;
    }

    int acc = arr[0];
    printf("+-------- fold visualization ---------+\n");
    printf("| Start: %d\n", acc);

    for (int i = 1; i < size; ++i) {
        int prev = acc;
        acc = fn(acc, arr[i]);
        printf("| Step %3d: | %3d | + | %3d | = | %3d |\n", i, prev, arr[i], acc);
        Sleep(200);
    }

    printf("+-------------------------------------+\n");
    return acc;
}

void kr_three_ex_three() {
    system("cls");

    int array[] = {2, 5, 8, 1, 4, 5, 3};
    int size = sizeof(array) / sizeof(array[0]);

    printf("Array: [ ");
    for (int i = 0; i < size; ++i) {
        printf("%d%s", array[i], (i < size - 1) ? ", " : "");
    }
    printf(" ]\n\n");

    int result = fold(array, size, add);
    printf("\nTotal sum of array elements: %d\n\n", result);

    PressToNext();
}
