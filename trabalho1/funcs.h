#include "expr.h"
#include <math.h>

#define MAX_NAME 32

static double cosseno(struct expr_func *f, vec_expr_t *args, void *c) {
	double a = expr_eval(&vec_nth(args, 0));
	return cos(a);
}

static double seno(struct expr_func *f, vec_expr_t *args, void *c) {
	double a = expr_eval(&vec_nth(args, 0));
	return sin(a);
}

static double exponencial(struct expr_func *f, vec_expr_t *args, void *c) {
	double a = expr_eval(&vec_nth(args, 0));
	return cos(a);
}

static double logn(struct expr_func *f, vec_expr_t *args, void *c) {
	double a = expr_eval(&vec_nth(args, 0));
	return log(a);
}

/* aqui definimos funcoes uteis */
static struct expr_func user_funcs[] = {
    {"cos", cosseno, NULL, 0},
    {"sen", seno, NULL, 0},
    {"exp", exponencial, NULL, 0},
    {"ln", logn, NULL, 0},
    {NULL, NULL, NULL, 0}
};

