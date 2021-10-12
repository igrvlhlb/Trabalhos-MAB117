import numpy as np

dict_arqs = {}

with open('tempos.txt', 'r') as infos:
    for linha in infos.readlines():
        arq, C, seq, par = linha.split()
        if (arq, C) in dict_arqs.keys():
            dict_arqs[(arq, C)].append((seq, par))
        else:
            dict_arqs[(arq, C)] = [(seq, par)]

dict_arqs_min = {}

for ent, t in dict_arqs.items():
    arq, C = ent
    if arq in dict_arqs_min.keys():
        dict_arqs_min[arq][C] = min(t, key=lambda x: x[-1])
    else:
        dict_arqs_min[arq] = {C: min(t, key=lambda x: x[-1])}

acel = {}

for arq, dict_C in dict_arqs_min.items():
    base = dict_C['1']
    tseq = sum(map(float, base))
    acel[arq] = {}
    acel[arq]['1'] = 1
    for C, vals in dict_C.items():
        if int(C) > 1:
            acel[arq][C] = tseq / sum(map(float, vals))

acels = {}
for arq, dict_C in acel.items():
    for C, acc in dict_C.items():
        if C in acels.keys():
            acels[C].append(acc)
        else:
            acels[C] = [acc]

medias = {C: np.mean(accs) for (C, accs) in acels.items()}
print(medias)
