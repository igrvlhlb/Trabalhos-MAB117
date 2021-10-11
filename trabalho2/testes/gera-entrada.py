#!/usr/bin/python

import sys
import time
import random
import os

# intervalo para geracao de numeros aleatorios
limite = 10**9


def gera_arquivos(n_entradas, N_arq):
    for sufixo in range(n_entradas):
        with open(f'entradas/entrada{sufixo:02d}.txt', 'w') as saida:
            cols = random.randint(5, 30)
            lins = random.randint(1, 50) * 10
            saida.write(str(cols * lins) + '\n')
            N_arq[sufixo] = cols
            
            for lin in range(lins):
                rand_nums = map(str, [random.randint(-limite, limite) for n in range(cols)])
                saida.write(' '.join(rand_nums) + '\n')

if __name__ == '__main__':
    argv = sys.argv
    random.seed(time.time())

    if len(argv) < 2:
        print(f'Uso: {argv[0]} <quantidade-de-entradas>')

    n_entradas = int(argv[1])

    # numero de numeros por linha em cada arquivo
    N_arq = {}
    gera_arquivos(n_entradas, N_arq)

    with open('infos.txt', 'w') as infos:
        for sufixo, N in N_arq.items():
            infos.write(f'{sufixo:02d} {N}\n')
