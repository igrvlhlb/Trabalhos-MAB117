#include "expr.h"
#include <pthread.h>

struct simpson_args {
	double h; 	// tamanho do incremento
	int a; 		// limite inferior do intervalo
	int n; 		// quantidade de subintervalos
	int thread_id;
	int nthreads; 	// quantidade de threads
	double res_parc; // resultado calculado por uma thread
	struct expr_var_list *vars;
	struct expr *func;
};

/*{{{*/
/* Calcula y a partir de func (uma função de x), a struct de variáveis associadas a func,
 * os limites de integração e quantidade de subintervalos
 * Retorna um vetor dinamicamente alocado */
double *calc_func(struct expr *func, struct expr_var_list *vars, double a, double b, int n) {
	double *y = (double *)malloc(sizeof(double) * (n + 1));
	double step = (b - a) / n / 2.0;
	/* obtem o endereco de x */
	struct expr_var *x = expr_var(vars, "x", 1);
	x->value = a;
	for (int i = 0; i < (n << 1); i++) {
		y[i] = expr_eval(func);
		x->value += step;
	}
	return y;
}
/*}}}*/

/* Função chamada por cada thread
 * para ela passamos (via pthread_create()
 * a estrutura struct simpson_args)
 */
void *simpson_method_parc(void *arg) {
	struct simpson_args *args = (struct simpson_args *)arg;
	double h = args->h;
	int n = args->n,
	    thread_id = args->thread_id,
	    nthreads = args->nthreads;
	double a = args->a;

	int m = n / 2;

	int nelems_par = (m - 1) / nthreads; // quantidade de elementos (indice par) calculados por thread
	int nelems_impar = m / nthreads; // quantidade de elementos (indice impar) calculados por thread

	int ini_par = (nelems_par * thread_id) + 1;
	int fim_par = nelems_par * (thread_id + 1);

	int ini_impar = (nelems_impar * thread_id) + 1;
	int fim_impar = nelems_impar * (thread_id + 1);

	struct expr_var *x = expr_var(args->vars, "x", 1);

	/* trata do caso em que o número de elementos
	 * não é divisível por nthreads */
	if (thread_id == nthreads - 1) { // estamos na ultima thread
		fim_par = m - 1;
		fim_impar = m;
	}

	int i;
	double result = 0;

	/* somatorio indices pares */
	for (i = ini_par; i <= fim_par; i++) {
		int k = 2 * i;
		int xk = a + (h * k);
		x->value = xk;
		result += expr_eval(args->func);
	}

	/* somatorio indices impares */
	for (i = ini_impar; i <= fim_impar; i++) {
		int k = (2 * i) - 1;
		int xk = a + (h * k);
		x->value = xk;
		result += expr_eval(args->func);
	}

	args->res_parc = result;

	pthread_exit(NULL);
}

/* Função que aproxima a integral com um número variável de threads
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
	struct simpson_args args[nthreads];
	double *y;
	double h = (b - a) / n;
	int thread;

	/* obtem o endereco de x */
	struct expr_var *x = expr_var(vars, "x", 1);
	x->value = b;
	double end = expr_eval(func);
	x->value = a;
	double start = expr_eval(func);

	resultado += start + end;

	for (thread = 0; thread < nthreads; thread++){
		args[thread] = (struct simpson_args){
			.h = h,
			.a = a,
			.n = n,
			.nthreads = nthreads,
			.thread_id = thread,
			.vars = vars
		};
		pthread_create(&threads[thread], NULL, simpson_method_parc, &args[thread]);
	}

	for (thread = 0; thread < nthreads; thread++) {
		pthread_join(threads[thread], NULL);
		resultado += args[thread].res_parc; // soma resultado obtido pela thread
	}

	return (h * resultado) / 3;
}

/* Função que aproxima a integral
 * utilizando o método de Simpson
 * Args:
 * 	- func: função compilada
 * 	- vars: variáveis associadas a `func`
 * 	- a, b: intervalo de integração
 * 	- n: 	quantidade de subintervalos
 */
double sequential_simpson_method(struct expr *func, struct expr_var_list *vars, double a, double b, int n) {
	double resultado;

	// tamanho do subintervalo
	double step = (b - a) / n / 2.0;

	/* obtem o endereco de x */
	struct expr_var *x = expr_var(vars, "x", 1);

	x->value = b;
	double end = expr_eval(func);
	
	x->value = a;
	double start = expr_eval(func);

	resultado = start + end;

	for (int i = 1; i < (n << 1); ++i) {
		if ((i & 1) == 1) {
			resultado += expr_eval(func) * 4.0;
		} else { // termo par tem peso 2
			resultado += expr_eval(func) * 2.0;
		}
		x->value += step;
	}

	return resultado / 3.0 * step;
}
