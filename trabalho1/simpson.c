#include "expr.h"
#include <pthread.h>

struct simpson_args {
	double a, b; 	// intervalo
	int n; 		// quantidade de subintervalos
	int thread_id;
	int nthreads; 	// quantidade de threads
};

/* Calcula y a partir de func (uma função de x), a struct de variáveis associadas a func,
 * os limites de integração e quantidade de subintervalos
 * Retorna um vetor dinamicamente alocado */
double *calc_func(struct expr *func, struct expr_var_list *vars, double a, double b, int n) {
	double *y = (double *)malloc(sizeof(double) * (n + 1));
	double step = (b - a) / n;
	/* obtem o endereco de x */
	struct expr_var *x = expr_var(vars, "x", 1);
	x->value = a;
	for (int i = 0; i <= n; i++) {
		y[i] = expr_eval(func);
		x->value += step;
	}
	return y;
}

/* Função que calcula a integral com um número variável de threads
 * utilizando o método de Simpson
 * Args:
 * 	- func: função compilada
 * 	- vars: variáveis associadas a `func`
 * 	- a, b: intervalo de integração
 * 	- n: 	quantidade de subintervalos
 * 	- nthreads: quantidade de threads
 */
double simpson_method(struct expr *func, struct expr_var_list *vars, double a, double b, int n, int nthreads) {
	double resultado;
	pthread_t threads[nthreads];
	double *y;
	return resultado;
}
