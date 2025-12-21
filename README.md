# 🌊 Projet-Info-WildWater

## 📝 Description

Ce projet met en place un pipeline **Shell + C** pour analyser un fichier CSV qui décrit un réseau de distribution d’eau. L’outil extrait :

- **Des histogrammes** résumant, pour chaque usine de traitement, la capacité annuelle maximale, le volume capté par les sources et le volume réellement traité (données en M.m³/an).
- **Le volume total perdu** par une usine sur tout son réseau aval, en tenant compte des fuites à chaque tronçon du graphe.

L’algorithme C repose sur **deux AVL** : l’un pour unir les usines et leurs volumes, l’autre pour indexer les nœuds du réseau (stockages, jonctions, raccordements, usagers) et calculer les pertes.

Le script Shell `Projet.sh` gère la compilation avec `make`, valide les arguments et s'occupe de 2 exécutables selon les résultats voulus : `exec_AVL` ou `exec_fuites`.

## ⚙️ Prérequis

- **Système :** macOS ou Linux avec `clang` (ou un compilateur C compatible).
- **Outil :** `make` (utilisé par le script).
- **Données :** Un fichier CSV conforme au format décrit dans `Sujet.pdf` (exemple : `c-wildwater_v0.dat` ou la v3).

## 📂 Fichiers du projet

Le projet comporte un total de **16 fichiers** nécessaires au bon fonctionnement :

- **C / Headers :** `AVL.c`, `AVL.h`, `AVL_USINES.c`, `AVL_USINES.h`, `CSV.c`, `CSV.h`, `HISTO.c`, `HISTO.h`, `leaks.c`, `LEAKS.h`, `network.c`, `NETWORK.h`, `main_avl.c`, `main_fuites.c`.
- **Automatisation :** `Makefile`, `Projet.sh`.

## 🔨 Compilation

Pour compiler le projet, lancez simplement :

```bash
make

```

> **Note :** Les 2 exécutables sont produits au début du script. La commande `clean` supprime les exécutables et les objets intermédiaires.

## 🚀 Utilisation du script

Toutes les commandes passent par `Projet.sh`. Il vérifie les arguments, compile si nécessaire, et appelle l'exécutable approprié (`./exec_AVL` ou `./exec_fuites`) avant d'afficher la durée d’exécution.

### 📊 Génération d’un histogramme

```bash
./Projet.sh <fichier.csv> histo <max|src|real>

```

**Exemples :**

```bash
./Projet.sh c-wildwater_v0.dat histo max
./Projet.sh c-wildwater_v0.dat histo src
./Projet.sh c-wildwater_v0.dat histo real

```

Chaque commande crée un fichier `vol_<type>.dat` à deux colonnes :

1. **Station (ID)** : L'identifiant de l'usine.
2. **Mesure** : La valeur en M.m³/an correspondant au mode demandé.

Les usines sont triées par identifiant décroissant. Ces fichiers sont prêts pour Gnuplot.

### 💧 Calcul des fuites d’une usine

```bash
./Projet.sh <fichier.csv> leaks "<Facility ID>"

```

**Exemple :**

```bash
./Projet.sh c-wildwater_v0.dat leaks "Plant #JA200000I"

```

Le volume des pertes (en M.m³/an) est ajouté au fichier `leaks.dat`. Si l’usine n’existe pas, la valeur `-1` est ajoutée.

## 🖼️ Génération des graphiques (Images)

Le sujet demande de produire des images (PNG) présentant les **50 plus petites** et les **10 plus grandes** usines selon la capacité maximale.

À partir du fichier `vol_max.dat`, le script utilise Gnuplot pour créer ces visuels (automatiquement via la commande `histo max`). Si Gnuplot n'est pas installé, un message d'erreur s'affiche.

**Installation de Gnuplot :**

```bash
sudo apt update
sudo install gnuplot

```

Les fichiers générés seront : `vol_max10.png` et `vol_min50.png`.

## 🧹 Nettoyage

Pour supprimer l’exécutable et les objets générés par la compilation :

```bash
make clean

```

```

```
