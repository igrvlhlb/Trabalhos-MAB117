#include "expr.h"
#include "funcs.h"
#include "consts.h"
#include <pthread.h>
//#include "expr_debug.h"

extern struct expr_func user_funcs[];

struct simpson_args {
	double h; 	// tamanho do incremento
	int a; 		// limite inferior do intervalo
	int n; 		// quantidade de subintervalos
	int thread_id;
	int nthreads; 	// quantidade de threads
	double res_parc; // resultado calculado por uma thread
	char *math_expr;
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
	char *math_expr = args->math_expr;

	int m = n / 2;

	int nelems = m / nthreads; // quantidade de elementos calculados por thread

	int ini = (nelems * thread_id) + 1;

	int fim_par, fim_impar;
	fim_par = fim_impar = nelems * (thread_id + 1);

	/* trata do caso em que o número de elementos
	 * não é divisível por nthreads */
	if (thread_id == nthreads - 1) { // estamos na ultima thread
		fim_par = m - 1;
		fim_impar = m;
	}

	int i;
	double result = 0;

	// inicializa lista de variáveis
	struct expr_var_list vars = {NULL};
	// compila a expressão
	struct expr *func = expr_create(math_expr, strlen(math_expr), &vars, user_funcs);
	// adiciona à lista constantes pré-definidas
	init_vars(&vars);

	if (func == NULL) {
		fprintf(stderr, "Erro ao compilar a expressão (thread %d)\n", thread_id);
		exit(1);
	}
	// pega o endereço da var x
	struct expr_var *x = expr_var(&vars, "x", 1);

	/* somatorio indices pares */
	for (i = ini; i <= fim_par; i++) {
		int k = 2 * i;
		int xk = a + (h * k);
		x->value = xk;
		/* somatorio par tem peso 2 */
		result += 2 * expr_eval(func);
	}

	/* somatorio indices impares */
	for (i = ini; i <= fim_impar; i++) {
		int k = (2 * i) - 1;
		int xk = a + (h * k);
		x->value = xk;
		/* somatorio impar tem peso 4 */
		result += 4 * expr_eval(func);
	}

	// libara recursos
	expr_destroy(func, &vars);

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
double simpson_method(char *math_expr, double a, double b, int n, int nthreads) {
	double resultado;
	pthread_t threads[nthreads];
	struct simpson_args args[nthreads];
	double *y;
	double h = (b - a) / n;
	int thread;

#ifdef DEBUG
// {{{
	fprintf(stderr, "math_expr: %s\n"
			"[a,b] = [%lf, %lf]\n"
			"n = %d\n"
			"nthreads = %d\n",
			math_expr, a, b,
			n, nthreads);
// }}}
#endif

	// inicializa lista de variáveis
	struct expr_var_list vars = {NULL};
	// compila a expressão
	struct expr *func = expr_create(math_expr, strlen(math_expr), &vars, user_funcs);
	//expr_println(func);
	if (func == NULL) {
		fprintf(stderr, "Erro ao compilar a expressão (simpson_method())\n");
		exit(1);
	}
	// adiciona à lista constantes pré-definidas
	init_vars(&vars);

	/* obtem o endereco de x */
	struct expr_var *x = expr_var(&vars, "x", 1);
	x->value = b;
	double end = expr_eval(func);
	x->value = a;
	double start = expr_eval(func);

	expr_destroy(func, &vars);

	resultado += start + end;

	for (thread = 0; thread < nthreads; thread++){

		args[thread] = (struct simpson_args){
			.h = h,
			.a = a,
			.n = n,
			.nthreads = nthreads,
			.thread_id = thread,
			.math_expr = math_expr,
			.res_parc = 0
		};
		if(pthread_create(&threads[thread], NULL, simpson_method_parc, &args[thread])){
			fprintf(stderr, "Erro ao criar thread\n");
			exit(1);
		}
	}

	for (thread = 0; thread < nthreads; thread++) {
		if(pthread_join(threads[thread], NULL)) {
			fprintf(stderr, "Erro join()\n");
			exit(1);
		}
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
double sequential_simpson_method(char *math_expr, double a, double b, int n) {
	double resultado;

	// tamanho do subintervalo
	double step = (b - a) / n / 2.0;

	// inicializa lista de variáveis
	struct expr_var_list vars = {NULL};
	// compila a expressão
	struct expr *func = expr_create(math_expr, strlen(math_expr), &vars, user_funcs);
	if (func == NULL) {
		fprintf(stderr, "Erro ao compilar a expressão (sequential_simpson_method())\n");
		exit(1);
	}
	// adiciona à lista constantes pré-definidas
	init_vars(&vars);

	/* obtem o endereco de x */
	struct expr_var *x = expr_var(&vars, "x", 1);

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

	expr_destroy(func, &vars);

	return resultado / 3.0 * step;
}
