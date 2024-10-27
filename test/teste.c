#include <stdio.h>
#include "include/sample_driver.h"
#include "include/second_driver.h"
#include "sub_module/sub_module_sample.h"

#define MACRO1 90

int zapzap(void) {
    return 1;
}

int main() {
    int x = 10;
    int y = 5;

#if MACRO_TESTE == 1
    very_basic_function();
#endif

    printf("Soma: %d\n", soma(x, y));
    printf("Subtração: %d\n", subtrai(x, y));

    return 0;
}
