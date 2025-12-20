#!/bin/bash

debut_chrono=$(date +%s%3N)
trap 'echo "La durée totale du traitement a été de $(($(date +%s%3N) - $debut_chrono)) secondes."' EXIT

if [ $# -ne 2 ]; then
	echo "Erreur  : il faut impérativement 2 arguments."
	exit 1
fi

if [ "$1" != "histo" ] && [ "$1" != "leaks" ]; then
	echo "Erreur : Les arguments donnés n'existent pas."
	exit 1
fi

verif_exec_AVL="exec_AVL"
if [ ! -f "$verif_exec_AVL" ]; then
	make
	if [ $? -ne 0 ]; then
		echo "Echec de la compilation : AVL."
		exit 1
	fi
fi

verif_exec_fuites="exec_fuites"
if [ ! -f "$verif_exec_fuites" ]; then
	make
	if [ $? -ne 0 ]; then
		echo "Echec de la compilation : fuites."
		exit 1
	fi
fi

if [ "$1" = "histo" ]; then
	if [ "$2" = "max" ]; then
		awk -F ';' '$2=="SOURCE" && NR>1 {print $1 ";" $4}' "$INPUT_CSV" > input_c.txt
		./exec_AVL input_c.txt > output.txt
	elif [ "$2" = "src" ]; then
	# à compléter

	elif [ "$2" = "real" ]; then
	# à compléter

	else; echo "Erreur : Le 2ème argument n'est pas valide."
fi

if [ "$1" =  "leaks" ]; then 
	# à compléter

fi


