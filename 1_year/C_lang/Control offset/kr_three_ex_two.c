#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printBinary(unsigned char c) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (c >> i) & 1);
    }
}

int toOctal(unsigned char c) {
    return c;
}

void kr_three_ex_two() {
    char sequence[9];
    char transformed[9];

    clearScreen();

    printf("Enter a sequence of exactly 8 characters:\n12345678\n");
    if (fgets(sequence, sizeof(sequence), stdin)) {
        sequence[strcspn(sequence, "\n")] = '\0';
    }
    getchar();
    if (strlen(sequence) != 8) {
        printf("Error: You must enter exactly 8 characters.\n");
        PressToNext();
        return;
    }

    strcpy(transformed, sequence);

    printf("\nOriginal sequence and its binary/octet codes:\n");
    printf("Char | Binary      | Octal\n");
    printf("---------------------------\n");

    for (int i = 0; i < 8; i++) {
        unsigned char c = sequence[i];
        printf("  %c  |  ", c);
        printBinary(c);
        printf("   | %3o\n", toOctal(c));
    }
    printf("---------------------------\n");
    PressToNext();

    for (int i = 0; i < 8; i++) {
        unsigned char c = sequence[i];

        if ((i + 1) % 2 == 1) {
            c |= (1 << 2);
        } else {
            c &= ~(1 << 3);
        }

        transformed[i] = c;
    }

    printf("\nTransformed sequence and its binary/octet codes:\n");
    printf("Char | Binary      | Octal\n");
    printf("---------------------------\n");

    for (int i = 0; i < 8; i++) {
        unsigned char c = transformed[i];
        printf("  %c  |  ", c);
        printBinary(c);
        printf("   | %3o\n", toOctal(c));
    }
    printf("---------------------------\n");
    printf("Transformed: %s\n", transformed);
    printf("---------------------------\n");
    PressToNext();
}
