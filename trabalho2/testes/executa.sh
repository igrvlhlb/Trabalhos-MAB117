while IFS=" " read -ra infos; do
	sufix="${infos[0]}"
	N="${infos[1]}"
	for C in {1..4}; do
		for reps in {1..5}; do
			../main "entradas/entrada${sufix}.txt" "saidas/saida${sufix}.txt" "$N" "$C"
		done
	done
done < infos.txt
