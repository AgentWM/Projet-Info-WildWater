# Projet-Info-WildWater
Projet Info 3 à finir avant le 21/12/25

# Projet-Info-WildWater

## Description

Ce projet met en place un pipeline Shell + C pour analyser un fichier CSV qui décrit un réseau de distribution d’eau. L’outil extrait :
- des histogrammes résumant, pour chaque usine de traitement, la capacité annuelle maximale, le volume capté par les sources et le volume réellement traité (données en M.m³/an).
- le volume total perdu par une usine sur tout son réseau aval, en tenant compte des fuites à chaque tronçon du graphe.

L’algorithme C repose sur deux AVL : l’un pour unir les usines et leurs volumes, l’autre pour indexer les nœuds du réseau (stockages, jonctions, raccordements, usagers) et calculer les pertes. Le script Shell 'Projet.sh' gère la compilation avec 'make', fait la validation des arguments et s'occupe de 2 exécutables selon le résultats voulus 'exec_AVL' ou 'exec_fuites'.

## Prérequis

- macOS ou Linux avec `clang` (ou un compilateur C compatible) ;
- `make` (utilisé par le script) ;
- un fichier CSV conforme au format décrit dans `Sujet.pdf` (exemple fourni : `c-wildwater_v0.dat` ou la v3).

## Fichier à télécharger
AVL.c ; AVL.h ; AVL_USINES.c ; AVL_USINES.h ; CSV.c ; CSV.h ; HISTO.c ; HISTO.h ; leaks.c ; LEAKS.h ; network.c ; NETWORK.h ; main_avl.c ; main_fuites.c ; Makefile, 
soit un total de 15 fichiers.

## Compilation

```bash
make
```

Les 2 exécutables sont produits au début du script et `clean` supprime les exécutables et les objets intermédiaires.

## Utilisation du script

Toutes les commandes passent par 'Projet.sh', qui vérifie les arguments, compile si nécessaire, appelle './exec_AVL' si le 2ème argument est histo, ou appelle './exec_fuites' si le 2ème argument est leaks puis affiche la durée d’exécution.

### Génération d’un histogramme

```bash
./Projet.sh <fichier.csv> histo <max|src|real>
```

Exemples :

```bash
./Projet.sh c-wildwater_v0.dat histo max
./Projet.sh c-wildwater_v0.dat histo src
./Projet.sh c-wildwater_v0.dat histo real
```

Chaque commande crée un fichier `vol_<max|captation|traitement>.dat` à deux colonnes :

- Station(ID) qui correspond à "identifier" dans le pdf mais un simple verbe pour définir une colonne n'est pas le plus intéressant à notre avis.
- la mesure correspondant au mode demandé, déjà convertie en M.m³/an.

Les usines sont triées par identifiant décroissant. Ces fichiers peuvent ensuite être exploités par Gnuplot pour tracer les barres demandées (top 50 plus petites, top 10 plus grandes).

### Calcul des fuites d’une usine

```bash
./Projet.sh <fichier.csv> leaks "<Facility ID>"
```

Exemple :

```bash
./Projet.sh c-wildwater_v0.dat leaks "Plant #JA200000I"
```

Le volume des pertes (en M.m³/an) est ajouté au fichier 'leaks.dat'. Si l’usine n’existe pas dans les données, la valeur '-1' est ajouté.

## Génération des histogrammes en image

Le sujet demande de produire des images (PNG) présentant les 50 plus petites et les 10 plus grandes usines selon la capacité maximale. À partir du fichier 'vol_max.dat', on utilise Gnuplot pour créer ces visuels (elles se créaient directement avec la commande './Projet.sh <fichier.csv> histo max', ou on renvoie un message d'erreur si gnuplot n'est pas installé). L'installation de Gnuplot :

```bash
sudo apt update
sudo install gnuplot
```

Les 2 images (PNG) auront pour nom 'vol_max10.png' et 'vol_min50.png'.

## Nettoyage

```bash
make clean
```

Supprime l’exécutable et les objets générés par la compilation.
