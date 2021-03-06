/* compilar com flag -DDEBUG se desejar a impressao de informacoes adicionais */
#include <stdio.h>
#include "simpson.h"
#include <string.h>
#include <stdlib.h>

#define MAX_EXPR 128 // tamanho maximo da expressao matematica


double a, b; 	// intervalo de integração
int nthreads; 	// numero de threads
int subint; 	// subintervalos


int main(int argc, char *argv[]) {

	char math_expr[MAX_EXPR];

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
	/* subint deve ser par */
	if (subint % 2 == 1) subint += 1;
	nthreads = atoi(argv[4]);

	fprintf(stderr, "Expressao lida: %s\n", math_expr);

	double resultado = sequential_simpson_method(math_expr, a, b, subint);
	printf("Resultado = %lf\n", resultado);

	double resultado_conc = simpson_method(math_expr, a, b, subint, nthreads);
	printf("Resultado (concorrente)= %lf\n", resultado_conc);

	return 0;
}
