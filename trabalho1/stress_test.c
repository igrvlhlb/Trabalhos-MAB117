#include<stdio.h>
#include<math.h>
// #include"expr.h"
// #include"funcs.h"
#include"simpson.h"
#include<assert.h>

#define MAX_EXPR 128 // tamanho maximo da expressao matematica

const double eps = 1e-3;

// retorna -1 se a < b, 0 se a = b e 1 se a > b
int cmp_double(double a, double b) {
    return a + eps > b ? b + eps > a ? 0 : 1 : -1;
}

void unit_test_0() {
    double a = 0;
    double b = acos(-1.0) / 2.0;
    double expected = sequential_simpson_method("sen(x)", a, b, 10000);
    double approximation = simpson_method("sen(x)", a, b, 10000, 2);
    printf("%lf -------- %lf\n", approximation, expected);
    assert(cmp_double(approximation, expected) == 0);
}


void unit_test_1() {
    double a = acos(-1.0);
    double b = 5.0 * acos(-1.0) / 3.0;
    double expected = sequential_simpson_method("cos(x)", a, b, 10000);
    double approximation = simpson_method("cos(x)", a, b, 10000, 2);
    printf("%lf -------- %lf\n", approximation, expected);
    assert(cmp_double(approximation, expected) == 0);
}

int main() {
    char math_expr[MAX_EXPR];
    // unit_test_0();
    unit_test_1();

    printf("Passou nos testes\n");
}
