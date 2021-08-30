#include "expr.h"

double sequential_simpson_method(struct expr *func, struct expr_var_list *vars, double a, double b, int n);
double simpson_method(char *math_expr, double a, double b, int n, int nthreads);
