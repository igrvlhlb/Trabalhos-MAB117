#include "expr.h"
#include <pthread.h>

struct simpson_args {
	double a, b; 	// intervalo
	int n; 		// quantidade de subintervalos
	int thread_id;
	int nthreads; 	// quantidade de threads
};

/* Função que calcula a integral com um número variável de threads
 * utilizando o método de Simpson
 * Args:
 * 	- func: função compilada
 * 	- a, b: intervalo de integração
 * 	- n: 	quantidade de subintervalos
 * 	- nthreads: quantidade de threads
 */
double simpson_method(struct expr func, int a, int b, int n, int nthreads) {
}
