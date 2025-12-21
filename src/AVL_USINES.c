#define _POSIX_C_SOURCE 200809L

#include "AVL_USINES.h"

#include <stdlib.h>
#include <string.h>

// Retourne le maximum entre deux entiers
static int max_int(int a, int b) {
    return (a > b) ? a : b;
}

// Retourne la hauteur d'un nœud 
static int hauteur(Usine *u) {
    return u ? u->hauteur : 0;
}

// Crée et initialise un nouveau nœud usine avec les valeurs fournies
static Usine *creer_usine(const char *id,
                          long capacite_delta,
                          double volume_capte_delta,
                          double volume_reel_delta) {
    // Allocation mémoire pour la structure usine
    Usine *u = malloc(sizeof(*u));
    if (!u) {
        fprintf(stderr, "Erreur: malloc Usine\n");
        exit(1);
    }

    // Copie de l'identifiant
    u->id = strdup(id);
    if (!u->id) {
        fprintf(stderr, "Erreur: strdup Usine id\n");
        free(u);
        exit(1);
    }

    // Initialisation des données de l'usine
    u->capacite = capacite_delta;
    u->volume_capte = volume_capte_delta;
    u->volume_reel = volume_reel_delta;
    u->hauteur = 1;  // Hauteur initiale d'une feuille
    u->gauche = NULL;
    u->droit = NULL;

    return u;
}

// Rotation droite pour rééquilibrer l'arbre
static Usine *rotation_droite(Usine *y) {
    Usine *x = y->gauche;
    Usine *T2 = x->droit;

    x->droit = y;
    y->gauche = T2;

    // Mise à jour des hauteurs
    y->hauteur = max_int(hauteur(y->gauche), hauteur(y->droit)) + 1;
    x->hauteur = max_int(hauteur(x->gauche), hauteur(x->droit)) + 1;

    return x;  // Nouvelle racine du sous-arbre
}

// Rotation gauche pour rééquilibrer l'arbre
static Usine *rotation_gauche(Usine *x) {
    Usine *y = x->droit;
    Usine *T2 = y->gauche;

    y->gauche = x;
    x->droit = T2;

    // Mise à jour des hauteurs
    x->hauteur = max_int(hauteur(x->gauche), hauteur(x->droit)) + 1;
    y->hauteur = max_int(hauteur(y->gauche), hauteur(y->droit)) + 1;

    return y;  // Nouvelle racine du sous-arbre
}

// Calcule le facteur d'équilibre d'un nœud 
static int equilibre(Usine *u) {
    return u ? hauteur(u->gauche) - hauteur(u->droit) : 0;
}

// Insère une usine dans l'arbre AVL ou met à jour ses valeurs si elle existe déjà
Usine *usine_inserer(Usine *racine, const char *id,
                     long capacite_delta,
                     double volume_capte_delta,
                     double volume_reel_delta) {
    // Cas de base : arbre vide, on crée un nouveau nœud
    if (!racine) {
        return creer_usine(id, capacite_delta, volume_capte_delta, volume_reel_delta);
    }

    // Insertion récursive selon l'ordre alphabétique
    int cmp = strcmp(id, racine->id);
    if (cmp < 0) {
        // Insertion dans le sous-arbre gauche
        racine->gauche = usine_inserer(racine->gauche, id,
                                       capacite_delta,
                                       volume_capte_delta,
                                       volume_reel_delta);
    } else if (cmp > 0) {
        // Insertion dans le sous-arbre droit
        racine->droit = usine_inserer(racine->droit, id,
                                      capacite_delta,
                                      volume_capte_delta,
                                      volume_reel_delta);
    } else {
        // L'usine existe déjà : on additionne les valeurs
        racine->capacite += capacite_delta;
        racine->volume_capte += volume_capte_delta;
        racine->volume_reel += volume_reel_delta;
        return racine;
    }

    // Mise à jour de la hauteur du nœud actuel
    racine->hauteur = 1 + max_int(hauteur(racine->gauche), hauteur(racine->droit));

    // Calcul du facteur d'équilibre pour vérifier si le nœud est déséquilibré
    int bal = equilibre(racine);

    // Cas 1: Déséquilibre gauche-gauche (rotation droite simple)
    if (bal > 1 && strcmp(id, racine->gauche->id) < 0) {
        return rotation_droite(racine);
    }

    // Cas 2 : Déséquilibre droit-droit donc rotation gauche simple
    if (bal < -1 && strcmp(id, racine->droit->id) > 0) {
        return rotation_gauche(racine);
    }

    // Cas 3 : Déséquilibre gauche-droit donc rotation gauche puis droite
    if (bal > 1 && strcmp(id, racine->gauche->id) > 0) {
        racine->gauche = rotation_gauche(racine->gauche);
        return rotation_droite(racine);
    }

    // Cas 4 : Déséquilibre droit-gauche rotation droite puis gauche
    if (bal < -1 && strcmp(id, racine->droit->id) < 0) {
        racine->droit = rotation_droite(racine->droit);
        return rotation_gauche(racine);
    }

    return racine;
}

// Recherche une usine par son identifiant dans l'arbre
Usine *usine_rechercher(Usine *racine, const char *id) {
    // Cas de base: arbre vide ou usine non trouvée
    if (!racine) {
        return NULL;
    }
    
    int cmp = strcmp(id, racine->id);
    if (cmp == 0) {
        // Usine trouvée
        return racine;
    }
    if (cmp < 0) {
        // Recherche dans le sous-arbre gauche
        return usine_rechercher(racine->gauche, id);
    }
    // Recherche dans le sous-arbre droit
    return usine_rechercher(racine->droit, id);
}

// Écrit les données des usines dans un fichier : parcours infixe inversé pour ordre décroissant
void usine_ecrire_histo(Usine *racine, FILE *fout, int type) {
    if (!racine || !fout) {
        return;
    }

    // Parcourt d'abord le sous-arbre droit : ordre décroissant
    usine_ecrire_histo(racine->droit, fout, type);

    // Conversion des valeurs de kWh en MWh 
    double capacite_Mm3 = (double)racine->capacite / 1000.0;
    double volume_capte_Mm3 = racine->volume_capte / 1000.0;
    double volume_reel_Mm3 = racine->volume_reel / 1000.0;

    // Écriture selon le type demandé
    if (type == 0) {
        fprintf(fout, "%s;%.6f\n", racine->id, capacite_Mm3);
    } else if (type == 1) {
        fprintf(fout, "%s;%.6f\n", racine->id, volume_capte_Mm3);
    } else {
        fprintf(fout, "%s;%.6f\n", racine->id, volume_reel_Mm3);
    }

    // Parcourt ensuite le sous-arbre gauche
    usine_ecrire_histo(racine->gauche, fout, type);
}

// Libère récursivement toute la mémoire de l'arbre
void usine_liberer(Usine *racine) {
    if (!racine) {
        return;
    }
    // Libération post-ordre : enfants puis parent
    usine_liberer(racine->gauche);
    usine_liberer(racine->droit);
    free(racine->id);
    free(racine);
}
