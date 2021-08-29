/* compilar com flag -DDEBUG se desejar a impressao de informacoes adicionais */

#include "expr.h"
#include "funcs.h"
#include "consts.h"
#include <math.h>

#define MAX_EXPR 128 // tamanho maximo da expressao matematica

extern struct expr_func user_funcs[];

double a, b; 	// intervalo de integração
int nthreads; 	// numero de threads
int subint; 	// subintervalos


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

	a = atof(argv[1]);
	b = atof(argv[2]);
	subint = atoi(argv[3]);
	nthreads = atoi(argv[4]);

	fprintf(stderr, "Expressao lida: %s\n", math_expr);

	expr = expr_create(math_expr, strlen(math_expr), &user_vars, user_funcs);

	if (expr == NULL) {
		printf("Syntax error");
		return 1;
	}

	init_vars(&user_vars);

#ifdef DEBUG
	fprintf(stderr, "Intervalo: [%lf, %lf]\n"
			"Subintervalos: %d\n"
			"nthreads: %d\n"
			"Expressão: %s\n",
			a, b, subint, nthreads, math_expr);
	fputs("======\nVariáveis definidas\n======", stderr);
	struct expr_var *var_atual = user_vars.head;
	int cnt = 0;
	while(var_atual != NULL) {
		fprintf(stderr, "Variavel: %s\n"
				"Valor: %lf\n"
				"Next: %p\n",
				var_atual->name,
				var_atual->value,
				var_atual->next);
		var_atual = var_atual->next;
		cnt++;
		if (var_atual != NULL) {
			fputs("------", stderr);
		}
	}
#endif

	expr_destroy(expr, &user_vars);
	return 0;
}
