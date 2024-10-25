#include <stdio.h>

#define MACRO1 90

int soma(int a, int b) {
    return a + b;
}

int subtrai(int a, int b) {
    return a - b;
}

int main() {
    int x = 10;
    int y = 5;

    printf("Soma: %d\n", soma(x, y));
    printf("Subtração: %d\n", subtrai(x, y));

    return 0;
}
