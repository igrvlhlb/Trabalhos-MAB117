#include "expr.h"
#include "funcs.h"
#include "consts.h"
#include <math.h>

#define MAX_EXPR 128 // tamanho maximo da expressao matematica

extern struct expr_func user_funcs[];

int main(int argc, char *argv[]) {

	char math_expr[MAX_EXPR];
	struct expr *expr;// = expr_create(s, strlen(s), &user_vars, user_funcs);
	struct expr_var_list user_vars = {NULL};

	/* menos argumentos que o necessario */
	if (argc < 6) {
		fprintf(stderr, "uso: %s <lim_inferior> <lim_superior>"
			"<subintervalos> <nthreads> <expressao matematica>\n",
			argv[0]);
		return 1;
	}
	/* a expressao esta em mais de um argumento */
	else if (argc > 6) {
		 /* maximo de caracteres que podem estar em uma parte da expressao */
		size_t parc = MAX_EXPR / (argc - 5);
		strncpy(math_expr, argv[5], parc);
		for (int i = 6; i < argc; i++) {
			strncat(math_expr, argv[i], parc);
		}
	}
	else {
		strncpy(math_expr, argv[5], MAX_EXPR);
	}

	fprintf(stderr, "Expressao lida: %s\n", math_expr);

	expr = expr_create(math_expr, strlen(math_expr), &user_vars, user_funcs);

	if (expr == NULL) {
		printf("Syntax error");
		return 1;
	}

	init_vars(&user_vars);

	double result = expr_eval(expr);
	printf("result: %f\n", result);

	expr_destroy(expr, &user_vars);
	return 0;
}
