#!/usr/bin/python
import sys

if __name__ == '__main__':
    valido = True
    argv = sys.argv
    if len(argv) < 2:
        print(f'Uso: {argv[0]} <arquivo>')

    arq = argv[1]
    with open(arq, 'r') as entrada:
        for linha in entrada.readlines():
            valores = map(int, linha.split())
            anterior = next(valores)
            for valor in valores:
                if valor < anterior:
                    print(f'# [{arq}] Falhou')
                    sys.exit(1)
                anterior = valor
    print(f"* [{arq}] Passou!")
