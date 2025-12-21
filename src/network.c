#define _POSIX_C_SOURCE 200809L
#include "NETWORK.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Retourne le maximum entre deux entiers
static int max_entier(int a, int b) {
    return (a > b) ? a : b;
}

// Retourne la hauteur d'un nœud AVL 
static int hauteur(NoeudAVL *n) {
    return n ? n->hauteur : 0;
}

// Crée un nouveau nœud du réseau avec un identifiant
static Noeud *creer_noeud(const char *id) {
    Noeud *n = malloc(sizeof(*n));
    if (!n) {
        fprintf(stderr, "Erreur: malloc Noeud\n");
        exit(1);
    }
    // Copie l'identifiant
    n->id = strdup(id);
    if (!n->id) {
        fprintf(stderr, "Erreur: strdup Noeud id\n");
        free(n);
        exit(1);
    }
    // Initialisation des données du nœud
    n->fuite = 0.0;
    n->enfants = NULL;
    return n;
}

// Crée un nœud AVL qui contient un pointeur vers un nœud du réseau
static NoeudAVL *creer_noeud_avl(const char *id, Noeud *noeud) {
    NoeudAVL *a = malloc(sizeof(*a));
    if (!a) {
        fprintf(stderr, "Erreur: malloc NoeudAVL\n");
        exit(1);
    }
    // Copie l'identifiant
    a->id = strdup(id);
    if (!a->id) {
        fprintf(stderr, "Erreur: strdup NoeudAVL id\n");
        free(a);
        exit(1);
    }
    // Initialisation de la structure AVL
    a->noeud = noeud;
    a->gauche = NULL;
    a->droite = NULL;
    a->hauteur = 1;
    return a;
}

// Effectue une rotation droite pour rééquilibrer l'arbre AVL
static NoeudAVL *rotation_droite(NoeudAVL *y) {
    NoeudAVL *x = y->gauche;
    NoeudAVL *T2 = x->droite;
    
    // Rotation: x monte, y descend à droite
    x->droite = y;
    y->gauche = T2;
    
    // Mise à jour des hauteurs
    y->hauteur = max_entier(hauteur(y->gauche), hauteur(y->droite)) + 1;
    x->hauteur = max_entier(hauteur(x->gauche), hauteur(x->droite)) + 1;
    return x;
}

// Effectue une rotation gauche pour rééquilibrer l'arbre AVL
static NoeudAVL *rotation_gauche(NoeudAVL *x) {
    NoeudAVL *y = x->droite;
    NoeudAVL *T2 = y->gauche;
    
    // Rotation: y monte, x descend à gauche
    y->gauche = x;
    x->droite = T2;
    
    // Mise à jour des hauteurs
    x->hauteur = max_entier(hauteur(x->gauche), hauteur(x->droite)) + 1;
    y->hauteur = max_entier(hauteur(y->gauche), hauteur(y->droite)) + 1;
    return y;
}

// Calcule le facteur d'équilibre 
static int equilibre(NoeudAVL *n) {
    return n ? hauteur(n->gauche) - hauteur(n->droite) : 0;
}

// Recherche un nœud par son identifiant dans l'arbre AVL
Noeud *reseau_avl_rechercher(NoeudAVL *racine, const char *id) {
    if (!racine) {
        return NULL;
    }
    // Comparaison alphabétique pour parcourir l'arbre
    int cmp = strcmp(id, racine->id);
    if (cmp == 0) {
        return racine->noeud;  // Nœud trouvé
    }
    if (cmp < 0) {
        return reseau_avl_rechercher(racine->gauche, id);  // Recherche à gauche
    }
    return reseau_avl_rechercher(racine->droite, id);  // Recherche à droite
}

// Insère un nouveau nœud dans l'arbre AVL ou retourne le nœud existant
NoeudAVL *reseau_avl_inserer(NoeudAVL *racine, const char *id, Noeud **noeud_sortie) {
    // Cas de base: arbre vide, création d'un nouveau nœud
    if (!racine) {
        Noeud *n = creer_noeud(id);
        if (noeud_sortie) {
            *noeud_sortie = n;  // Retourne le nœud créé
        }
        return creer_noeud_avl(id, n);
    }
    
    // Insertion récursive selon l'ordre alphabétique
    int cmp = strcmp(id, racine->id);
    if (cmp < 0) {
        racine->gauche = reseau_avl_inserer(racine->gauche, id, noeud_sortie);
    } else if (cmp > 0) {
        racine->droite = reseau_avl_inserer(racine->droite, id, noeud_sortie);
    } else {
        // Le nœud existe déjà, retourne le nœud existant
        if (noeud_sortie) {
            *noeud_sortie = racine->noeud;
        }
        return racine;
    }
    
    // Mise à jour de la hauteur du nœud actuel
    racine->hauteur = 1 + max_entier(hauteur(racine->gauche), hauteur(racine->droite));
    int bal = equilibre(racine);
    
    // Cas 1 : Déséquilibre gauche-gauche donc rotation droite simple
    if (bal > 1 && strcmp(id, racine->gauche->id) < 0) {
        return rotation_droite(racine);
    }
    // Cas 2 : Déséquilibre droit-droit donc rotation gauche simple
    if (bal < -1 && strcmp(id, racine->droite->id) > 0) {
        return rotation_gauche(racine);
    }
    // Cas 3 : Déséquilibre gauche-droit donc rotation gauche puis droite
    if (bal > 1 && strcmp(id, racine->gauche->id) > 0) {
        racine->gauche = rotation_gauche(racine->gauche);
        return rotation_droite(racine);
    }
    // Cas 4 : Déséquilibre droit-gauche donc rotation droite puis gauche
    if (bal < -1 && strcmp(id, racine->droite->id) < 0) {
        racine->droite = rotation_droite(racine->droite);
        return rotation_gauche(racine);
    }
    
    return racine;
}

// Ajoute un nœud enfant à un nœud parent 
void reseau_ajouter_enfant(Noeud *parent, Noeud *enfant) {
    if (!parent || !enfant) {
        return;
    }
    // Crée un nouveau maillon pour la liste chaînée d'enfants
    Enfant *e = malloc(sizeof(*e));
    if (!e) {
        fprintf(stderr, "Erreur: malloc Enfant\n");
        exit(1);
    }
    // Insertion en tête de liste
    e->noeud = enfant;
    e->suivant = parent->enfants;
    parent->enfants = e;
}

// Libère la mémoire d'un nœud du réseau et sa liste d'enfants
static void liberer_noeud(Noeud *n) {
    if (!n) {
        return;
    }
    // Libère toute la liste chaînée d'enfants
    Enfant *e = n->enfants;
    while (e) {
        Enfant *suivant = e->suivant;
        free(e);
        e = suivant;
    }
    free(n->id);
    free(n);
}

// Libère récursivement toute la mémoire de l'arbre AVL et des nœuds du réseau
void reseau_avl_liberer(NoeudAVL *racine) {
    if (!racine) {
        return;
    }
    // Parcours post-ordre pour libérer les enfants avant le parent
    reseau_avl_liberer(racine->gauche);
    reseau_avl_liberer(racine->droite);
    liberer_noeud(racine->noeud);  // Libère le nœud du réseau
    free(racine->id);
    free(racine);  // Libère le nœud AVL
}
