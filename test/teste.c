#include <stdio.h>
#include "include/sample_driver.h"
#include "include/second_driver.h"
#include "sub_module/sub_module_sample.h"

#define MACRO1 90

int oioi = 10;

int Zapzap(void) {
    return 0;
}

int main() {
    int x = MACRO1;
    char y;

#if MACRO_TESTE == 1
    very_basic_function();
#endif

    printf("Soma: %d\n", soma(x, y));
    printf("Subtração: %d\n", subtrai(x, y));

}
