#define _POSIX_C_SOURCE 200809L
#include "NETWORK.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int max_entier(int a, int b) {
    return (a > b) ? a : b;
}

static int hauteur(NoeudAVL *n) {
    return n ? n->hauteur : 0;
}

static Noeud *creer_noeud(const char *id) {
    Noeud *n = malloc(sizeof(*n));
    if (!n) {
        fprintf(stderr, "Erreur: malloc Noeud\n");
        exit(1);
    }
    n->id = strdup(id);
    if (!n->id) {
        fprintf(stderr, "Erreur: strdup Noeud id\n");
        free(n);
        exit(1);
    }
    n->fuite = 0.0;
    n->enfants = NULL;
    return n;
}

static NoeudAVL *creer_noeud_avl(const char *id, Noeud *noeud) {
    NoeudAVL *a = malloc(sizeof(*a));
    if (!a) {
        fprintf(stderr, "Erreur: malloc NoeudAVL\n");
        exit(1);
    }
    a->id = strdup(id);
    if (!a->id) {
        fprintf(stderr, "Erreur: strdup NoeudAVL id\n");
        free(a);
        exit(1);
    }
    a->noeud = noeud;
    a->gauche = NULL;
    a->droite = NULL;
    a->hauteur = 1;
    return a;
}

static NoeudAVL *rotation_droite(NoeudAVL *y) {
    NoeudAVL *x = y->gauche;
    NoeudAVL *T2 = x->droite;
    x->droite = y;
    y->gauche = T2;
    y->hauteur = max_entier(hauteur(y->gauche), hauteur(y->droite)) + 1;
    x->hauteur = max_entier(hauteur(x->gauche), hauteur(x->droite)) + 1;
    return x;
}

static NoeudAVL *rotation_gauche(NoeudAVL *x) {
    NoeudAVL *y = x->droite;
    NoeudAVL *T2 = y->gauche;
    y->gauche = x;
    x->droite = T2;
    x->hauteur = max_entier(hauteur(x->gauche), hauteur(x->droite)) + 1;
    y->hauteur = max_entier(hauteur(y->gauche), hauteur(y->droite)) + 1;
    return y;
}

static int equilibre(NoeudAVL *n) {
    return n ? hauteur(n->gauche) - hauteur(n->droite) : 0;
}

Noeud *reseau_avl_rechercher(NoeudAVL *racine, const char *id) {
    if (!racine) {
        return NULL;
    }
    int cmp = strcmp(id, racine->id);
    if (cmp == 0) {
        return racine->noeud;
    }
    if (cmp < 0) {
        return reseau_avl_rechercher(racine->gauche, id);
    }
    return reseau_avl_rechercher(racine->droite, id);
}

NoeudAVL *reseau_avl_inserer(NoeudAVL *racine, const char *id, Noeud **noeud_sortie) {
    if (!racine) {
        Noeud *n = creer_noeud(id);
        if (noeud_sortie) {
            *noeud_sortie = n;
        }
        return creer_noeud_avl(id, n);
    }
    
    int cmp = strcmp(id, racine->id);
    if (cmp < 0) {
        racine->gauche = reseau_avl_inserer(racine->gauche, id, noeud_sortie);
    } else if (cmp > 0) {
        racine->droite = reseau_avl_inserer(racine->droite, id, noeud_sortie);
    } else {
        if (noeud_sortie) {
            *noeud_sortie = racine->noeud;
        }
        return racine;
    }
    
    racine->hauteur = 1 + max_entier(hauteur(racine->gauche), hauteur(racine->droite));
    int bal = equilibre(racine);
    
    if (bal > 1 && strcmp(id, racine->gauche->id) < 0) {
        return rotation_droite(racine);
    }
    if (bal < -1 && strcmp(id, racine->droite->id) > 0) {
        return rotation_gauche(racine);
    }
    if (bal > 1 && strcmp(id, racine->gauche->id) > 0) {
        racine->gauche = rotation_gauche(racine->gauche);
        return rotation_droite(racine);
    }
    if (bal < -1 && strcmp(id, racine->droite->id) < 0) {
        racine->droite = rotation_droite(racine->droite);
        return rotation_gauche(racine);
    }
    
    return racine;
}

void reseau_ajouter_enfant(Noeud *parent, Noeud *enfant) {
    if (!parent || !enfant) {
        return;
    }
    Enfant *e = malloc(sizeof(*e));
    if (!e) {
        fprintf(stderr, "Erreur: malloc Enfant\n");
        exit(1);
    }
    e->noeud = enfant;
    e->suivant = parent->enfants;
    parent->enfants = e;
}

static void liberer_noeud(Noeud *n) {
    if (!n) {
        return;
    }
    Enfant *e = n->enfants;
    while (e) {
        Enfant *suivant = e->suivant;
        free(e);
        e = suivant;
    }
    free(n->id);
    free(n);
}

void reseau_avl_liberer(NoeudAVL *racine) {
    if (!racine) {
        return;
    }
    reseau_avl_liberer(racine->gauche);
    reseau_avl_liberer(racine->droite);
    liberer_noeud(racine->noeud);
    free(racine->id);
    free(racine);
}
