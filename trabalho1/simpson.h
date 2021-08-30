#include "expr.h"

double sequential_simpson_method(char *math_expr, double a, double b, int n);
double simpson_method(char *math_expr, double a, double b, int n, int nthreads);
