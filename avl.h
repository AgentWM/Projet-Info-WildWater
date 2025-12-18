#ifndef AVL_H
#define AVL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Station {
    char* id;
    long capacite;
    long volume;
    int hauteur;
    struct Station* gauche;
    struct Station* droit;
} Station;

int max(int a, int b);
int min(int a, int b);
int hauteur(Station* station);
Station* creerStation(char* id, long capacite);
int equilibre(Station* station);
Station* rotationDroite(Station* y);
Station* rotationGauche(Station* x);
Station* insererStation(Station* noeud, char* id, long capacite, long volume);
void traiterSortie(Station* station, FILE* fichier);
void libererArbre(Station* station);

#endif