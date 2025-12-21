#include "AVL.h"

// Fonction utilitaire pour renvoyer le max entre deux entiers
int max(int a, int b) {
    if (a > b)
        return a;
    else
        return b;
}

// Pareil pour le min
int min(int a, int b) {
    if (a < b)
        return a;
    else
        return b;
}

// Récupère la hauteur d'un noeud. gère le cas ou la station est NULL pour éviter les crashs
int hauteur(Station* station) {
    if (station == NULL)
        return 0;
    return station->hauteur;
}

// Création d'une nouvelle station. On alloue la mémoire et on initialise les variables
Station* creerStation(char* id, long capacite) {
    Station* nouvelleStation = (Station*)malloc(sizeof(Station));
    if (nouvelleStation == NULL) {
        exit(1); // Si le malloc échoue on coupe tout
    }
    // strdup est important ici pour bien copier la chaine de caractère et pas juste le pointeur
    nouvelleStation->id = strdup(id);
    nouvelleStation->capacite = capacite;
    nouvelleStation->volume = 0;
    nouvelleStation->hauteur = 1;
    nouvelleStation->gauche = NULL;
    nouvelleStation->droit = NULL;
    return nouvelleStation;
}

// Calcul du facteur d'équilibre (gauche - droite)
int equilibre(Station* station) {
    if (station == NULL)
        return 0;
    return hauteur(station->gauche) - hauteur(station->droit);
}

// Rotation droite classique pour rééquilibrer
Station* rotationDroite(Station* y) {
    Station* x = y->gauche;
    Station* T2 = x->droit;

    // on bouge les pointeurs
    x->droit = y;
    y->gauche = T2;

    // Il ne faut pas oublier de mettre à jour les hauteurs après la rotation
    y->hauteur = max(hauteur(y->gauche), hauteur(y->droit)) + 1;
    x->hauteur = max(hauteur(x->gauche), hauteur(x->droit)) + 1;

    return x; // x devient la nouvelle racine
}

// Rotation gauche, même logique que la droite mais dans l'autre sens
Station* rotationGauche(Station* x) {
    Station* y = x->droit;
    Station* T2 = y->gauche;

    y->gauche = x;
    x->droit = T2;

    x->hauteur = max(hauteur(x->gauche), hauteur(x->droit)) + 1;
    y->hauteur = max(hauteur(y->gauche), hauteur(y->droit)) + 1;

    return y;
}

// Fonction principale d'insertion qui gère aussi l'agrégation des volumes
Station* insererStation(Station* noeud, char* id, long capacite, long volume) {
    // Cas de base : l'arbre est vide ou on est au bout d'une branche
    if (noeud == NULL) {
        Station* s = creerStation(id, capacite);
        s->volume = volume;
        return s;
    }

    // Recherche de la position avec strcmp pour comparer les id (ordre alphabétique)
    if (strcmp(id, noeud->id) < 0)
        noeud->gauche = insererStation(noeud->gauche, id, capacite, volume);
    else if (strcmp(id, noeud->id) > 0)
        noeud->droit = insererStation(noeud->droit, id, capacite, volume);
    else {
        // Si l'id existe déjà (égalité), on additionne le volume (agrégation)
        noeud->volume += volume;
        // petite verif au cas où la capacité n'était pas renseignée avant
        if (noeud->capacite == 0) {
            noeud->capacite = capacite;
        }
        return noeud; // pas besoin d'équilibrer si on a juste modifié une valeur existante
    }

    // Mise à jour de la hauteur du noeud courant
    noeud->hauteur = 1 + max(hauteur(noeud->gauche), hauteur(noeud->droit));

    // on récupère l'équilibre pour voir si ça penche trop
    int balance = equilibre(noeud);

    // 4 cas de rotations possibles selon le déséquilibre
    
    // Cas Gauche Gauche
    if (balance > 1 && strcmp(id, noeud->gauche->id) < 0)
        return rotationDroite(noeud);

    // Cas Droit Droit
    if (balance < -1 && strcmp(id, noeud->droit->id) > 0)
        return rotationGauche(noeud);

    // Cas Gauche Droite
    if (balance > 1 && strcmp(id, noeud->gauche->id) > 0) {
        noeud->gauche = rotationGauche(noeud->gauche);
        return rotationDroite(noeud);
    }

    // Cas Droit Gauche
    if (balance < -1 && strcmp(id, noeud->droit->id) < 0) {
        noeud->droit = rotationDroite(noeud->droit);
        return rotationGauche(noeud);
    }

    return noeud;
}

// Parcours de l'arbre pour écrire dans le fichier csv
// attention : parcours "droit -> racine -> gauche" pour respecter l'ordre décroissant demandé
void traiterSortie(Station* station, FILE* fichier) {
    if (station != NULL) {
        traiterSortie(station->droit, fichier);
        fprintf(fichier, "%s;%ld;%ld\n", station->id, station->capacite, station->volume);
        traiterSortie(station->gauche, fichier);
    }
}

// Libère proprement toute la mémoire de l'arbre
void libererArbre(Station* station) {
    if (station != NULL) {
        libererArbre(station->gauche);
        libererArbre(station->droit);
        free(station->id);
        free(station);
    }
}

