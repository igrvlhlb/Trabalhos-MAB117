#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <err.h>
#include <stdbool.h>

#define BUFFSIZE 10 // tamanho do buffer

/* Variáveis de sincronizacao:
 * 	- slots_vazios, slots_cheios: dao informacoes sobre o buffer
 * 		(o buffer tem capacidade BUFFSIZE, entao slots_vazios comeca com o valor
 * 		BUFFSIZE e slots_vazios comeca com o valor 0)
 * 	- buff_lock: exclusao mutua para leitura/escrita do buffer
 * 	- f_lock: exclusao mutua para escrita no arquivo de saida */
sem_t slots_vazios, slots_cheios;
pthread_mutex_t buff_lock, f_lock;

/* alocaremos dinamicamente memoria para o buffer */
int *buffer;
FILE *arq_entrada, *arq_saida;
int in, out, C, N;
bool continua = true;

int my_cmp(const void* a, const void* b) {
	return (*(int*)a - *(int*)b);
}

/* Funcao que eh chamada pela funcao main() para que leia o conteudo do
 * arquivo de entrada e coloca-lo no buffer */
void produz(void) {
	int total; // primeiro valor do arquivo é a quantidade de números
	int lidos[N];
	fscanf(arq_entrada, " %d", &total);

	while (total > 0) {
		for (int i = 0; i < N; i++) {
			fscanf(arq_entrada, " %d", &lidos[i]);
		}
		sem_wait(&slots_vazios);
		pthread_mutex_lock(&buff_lock);
		/* posicao para escrever no buffer */
		int buffpos = in * N;
		/* escreve no buffer */
		for (int i = 0; i < N; i++) {
			buffer[buffpos + i] = lidos[i];
		}
		in = (in + 1) % BUFFSIZE;
		pthread_mutex_unlock(&buff_lock);
		sem_post(&slots_cheios);
		total -= N;
	}
	/* caso haja alguma thread parada no sem_wait(), a libera */
	sem_post(&slots_cheios);
	/* indica que nao ha mais entrada */
	continua = false;
}

/* Funcao que eh executada pelas threads consumidoras/escritoras */
void consome(void *args) {
	int total; // primeiro valor do arquivo é a quantidade de números
	int copia[N]; // copia do buffer

	while (1) {
		sem_wait(&slots_cheios);

		/* se a entrada acabar, encerra a thread */
		if (continua == false && in == out){
			/* permite que outra thread seja desbloqueada cajo esteja bloqueada*/
			sem_post(&slots_cheios);
			pthread_exit(0);
		}

		pthread_mutex_lock(&buff_lock);

		/* posicao para ler do buffer */
		int buffpos = out * N;
		/* escreve no buffer local */
		for (int i = 0; i < N; i++) {
			copia[i] = buffer[buffpos + i];
		}
		out = (out + 1) % BUFFSIZE;
		pthread_mutex_unlock(&buff_lock);
		sem_post(&slots_vazios);

		/* ordena o bloco */ 
		qsort(copia, N, sizeof(int), my_cmp);

		pthread_mutex_lock(&f_lock);
		/* neste ponto o vetor `copia[]` esta ordenado */
		for (int i = 0; i < N; i++) {
			fprintf(arq_saida, "%d ", copia[i]);
		}
		putc('\n', arq_saida);
		pthread_mutex_unlock(&f_lock);
	}
}

/* Argumentos de linha de comando:
 * 	- arq_entrada: arquivo com os valores de entrada
 * 	- arq_saida: arquivo onde escreveremos os valores ordenados
 * 	- N: Número de valores por linha
 * 	- C: número de threads consumidoras/escritoras */
int main(int argc, char *argv[]) {
	in = out = 0;

	if (argc < 5) {
		fprintf(stderr, "Uso: %s <arq_entrada> <arq_saida> <N> <C>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* se entrar no if, ocorreu erro */
	if ((arq_entrada = fopen(argv[1], "r")) == NULL) {
		err(EXIT_FAILURE, "Erro ao abrir o arquivo '%s'", argv[1]);
	}
	/* idem */
	if ((arq_saida = fopen(argv[2], "w")) == NULL) {
		err(EXIT_FAILURE, "Erro ao abrir o arquivo '%s'", argv[2]);
	}

	N = atoi(argv[3]);
	C = atoi(argv[4]);

	/* aloca memoria para o buffer */
	buffer = (int *)malloc(sizeof(int) * BUFFSIZE * N);
	if (buffer == NULL) {
		err(EXIT_FAILURE, "Erro ao alocar memoria para o buffer");
	}

	/* inicializacao das variaveis de sincronizacao */
	sem_init(&slots_vazios, 0, BUFFSIZE);
	sem_init(&slots_cheios, 0, 0);
	pthread_mutex_init(&buff_lock, NULL);
	pthread_mutex_init(&f_lock, NULL);

	/* cria threads */
	pthread_t tid_prod;

	pthread_t* tid_cons;
	tid_cons = (pthread_t *)malloc(sizeof(int) * C);

	for (int id = 0; id < C; ++id) {
		if (pthread_create(&tid_cons[id], NULL, consome, NULL)) {
			err(EXIT_FAILURE, "Erro -- thread create");
		}
	}

	/* executa o produtor na thread principal */
	produz();

	for (int id = 0; id < C; ++id) {
		if (pthread_join(tid_cons[id], NULL)) {
			err(EXIT_FAILURE, "Erro -- join thread");
		}
	}

	/* fecha os arquivos */
	fclose(arq_entrada);
	fclose(arq_saida);

	/* destruicao das variaveis de sincronizacao */
	pthread_mutex_destroy(&buff_lock);
	pthread_mutex_destroy(&f_lock);
	sem_destroy(&slots_vazios);
	sem_destroy(&slots_cheios);

	free(tid_cons);
	free(buffer);
	return 0;
}
