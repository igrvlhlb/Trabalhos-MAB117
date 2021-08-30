#include<stdio.h>
#include<math.h>
// #include"expr.h"
// #include"funcs.h"
#include"simpson.c"
#include<assert.h>

#define MAX_EXPR 128 // tamanho maximo da expressao matematica

const double eps = 1e-4;

// retorna -1 se a < b, 0 se a = b e 1 se a > b
int cmp_double(double a, double b) {
    return a + eps > b ? b + eps > a ? 0 : 1 : -1;
}

void unit_test_0() {
    double a = 0;
    double b = acos(-1.0) / 2.0;
    double approximation = simpson_method("sin(x)", a, b, 100, 1);
    assert(cmp_double(approximation, 1.0) == 0);
}


void unit_test_1() {
    double a = acos(-1.0);
    double b = 5.0 * acos(-1.0) / 3.0;
    double approximation = simpson_method("cos(x)", a, b, 100, 1);
    assert(cmp_double(approximation, .0) == 0);
}

int main() {
    unit_test_0();
    // unit_test_1();

    printf("Passou nos testes\n");
}
