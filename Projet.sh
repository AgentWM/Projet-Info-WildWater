#!/bin/bash

#Timeur demandé peu importe que le programme aboutisse ou fasse une erreur
debut_chrono=$(date +%s%3N)
trap 'echo "La durée totale du traitement a été de $(($(date +%s%3N) - $debut_chrono)) secondes."' EXIT

#Vérifications des conditions de base, nb d arg
if [ $# -ne 3 ]; then
	echo "Erreur  : il faut impérativement 3 arguments."
	exit 1
fi
#les args dans l'ordre demandé
if [ ! -f "$1" ]; then
	echo "Erreur : Le fichier '$1' n'existe pas."
	exit 1
fi

if [ "$2" != "histo" ] && [ "$2" != "leaks" ]; then
	echo "Erreur : Le 2ème argument donné n'est ni "histo" ni "leaks"."
	exit 1
fi
#les exécutables C
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

#Les cas selon ce qu'on nous demande en argument
if [ "$2" = "histo" ]; then
	if [ "$3" = "max" ]; then #On prend juste les lignes usines
		awk -F ';' '$1=="-" && $3=='-' {print $2 ";" $4/1000}' "$1" > tri_max.txt
		(echo "Station(ID);Capacité maximale de traitement(M.m³/an)"; ./exec_AVL tri_max.txt) > vol_max.dat
	elif [ "$3" = "src" ]; then #On prend les lignes sources -> usines
		awk -F ';' '$1=="-" && $3!="-" {print $3 ";" $4/1000}' "$1" > tri_capt.txt
		(echo "Station(ID);Volume total capté depuis les sources(M.m³/an)"; ./exec_AVL tri_capt.txt) > vol_captation.txt
	elif [ "$3" = "real" ]; then #On prend les lignes sources -> usines mais on enlève le volume de fuite du volume total
		awk -F ';' '$1=="-" && $3!="-" {print $3 ";" ($4 * (1 - $5/100)) / 1000}' "$1" > tri_reel.txt
		(echo "Station(ID);Volume total traité(M.m³/an)"; ./exec_AVL tri_reel.txt) > vol_traitement.tmp
	else
		echo "Erreur : Le 3ème argument n'est pas valide."
		exit 1
	fi
fi

if [ "$2" =  "leaks" ]; then
	if [ -z "$3" ]; then
		echo "Erreur : Leak a besoin d'un identifiant d'usine pour son 3ème argument."
		exit 1
	fi
	awk -F ';' -v id="$3" '($1=="-" && $2==id) || ($1==id) {print $0}' "$1" > liste_leaks.txt
	if [ $(./exec_fuites liste_leaks.txt) e -1 ]; then
		echo "Erreur : L'identifiant d'usine n'existe pas."
		exit 1
	fi
	(echo "Station(ID);Volume fuité(M.m^3/an)"; cat liste_leaks.txt; echo "$(./exec_fuites listes_leaks.txt) M.m^3 sont perdues par an.") > vol_fuites.dat
fi
