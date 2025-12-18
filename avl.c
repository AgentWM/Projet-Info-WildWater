#include "avl.h"

int max(int a, int b) {
    if (a > b)
        return a;
    else
        return b;
}

int min(int a, int b) {
    if (a < b)
        return a;
    else
        return b;
}

int hauteur(Station* station) {
    if (station == NULL)
        return 0;
    return station->hauteur;
}

Station* creerStation(char* id, long capacite) {
    Station* nouvelleStation = (Station*)malloc(sizeof(Station));
    if (nouvelleStation == NULL) {
        exit(1);
    }
    nouvelleStation->id = strdup(id);
    nouvelleStation->capacite = capacite;
    nouvelleStation->volume = 0;
    nouvelleStation->hauteur = 1;
    nouvelleStation->gauche = NULL;
    nouvelleStation->droit = NULL;
    return nouvelleStation;
}

int equilibre(Station* station) {
    if (station == NULL)
        return 0;
    return hauteur(station->gauche) - hauteur(station->droit);
}

Station* rotationDroite(Station* y) {
    Station* x = y->gauche;
    Station* T2 = x->droit;

    x->droit = y;
    y->gauche = T2;

    y->hauteur = max(hauteur(y->gauche), hauteur(y->droit)) + 1;
    x->hauteur = max(hauteur(x->gauche), hauteur(x->droit)) + 1;

    return x;
}

Station* rotationGauche(Station* x) {
    Station* y = x->droit;
    Station* T2 = y->gauche;

    y->gauche = x;
    x->droit = T2;

    x->hauteur = max(hauteur(x->gauche), hauteur(x->droit)) + 1;
    y->hauteur = max(hauteur(y->gauche), hauteur(y->droit)) + 1;

    return y;
}

Station* insererStation(Station* noeud, char* id, long capacite, long volume) {
    if (noeud == NULL) {
        Station* s = creerStation(id, capacite);
        s->volume = volume;
        return s;
    }

    if (strcmp(id, noeud->id) < 0)
        noeud->gauche = insererStation(noeud->gauche, id, capacite, volume);
    else if (strcmp(id, noeud->id) > 0)
        noeud->droit = insererStation(noeud->droit, id, capacite, volume);
    else {
        noeud->volume += volume;
        if (noeud->capacite == 0) {
            noeud->capacite = capacite;
        }
        return noeud;
    }

    noeud->hauteur = 1 + max(hauteur(noeud->gauche), hauteur(noeud->droit));

    int balance = equilibre(noeud);

    if (balance > 1 && strcmp(id, noeud->gauche->id) < 0)
        return rotationDroite(noeud);

    if (balance < -1 && strcmp(id, noeud->droit->id) > 0)
        return rotationGauche(noeud);

    if (balance > 1 && strcmp(id, noeud->gauche->id) > 0) {
        noeud->gauche = rotationGauche(noeud->gauche);
        return rotationDroite(noeud);
    }

    if (balance < -1 && strcmp(id, noeud->droit->id) < 0) {
        noeud->droit = rotationDroite(noeud->droit);
        return rotationGauche(noeud);
    }

    return noeud;
}

void traiterSortie(Station* station, FILE* fichier) {
    if (station != NULL) {
        traiterSortie(station->droit, fichier);
        fprintf(fichier, "%s;%ld;%ld\n", station->id, station->capacite, station->volume);
        traiterSortie(station->gauche, fichier);
    }
}

void libererArbre(Station* station) {
    if (station != NULL) {
        libererArbre(station->gauche);
        libererArbre(station->droit);
        free(station->id);
        free(station);
    }
}