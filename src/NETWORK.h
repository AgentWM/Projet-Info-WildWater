#ifndef NETWORK_H
#define NETWORK_H

typedef struct Noeud Noeud;
typedef struct Enfant Enfant;
typedef struct NoeudAVL NoeudAVL;

struct Enfant {
    Noeud *noeud;
    Enfant *suivant;
};

struct Noeud {
    char *id;
    double fuite;
    Enfant *enfants;
};

struct NoeudAVL {
    char *id;
    Noeud *noeud;
    int hauteur;
    NoeudAVL *gauche;
    NoeudAVL *droite;
};

Noeud *reseau_avl_rechercher(NoeudAVL *racine, const char *id);
NoeudAVL *reseau_avl_inserer(NoeudAVL *racine, const char *id, Noeud **noeud_sortie);
void reseau_ajouter_enfant(Noeud *parent, Noeud *enfant);
void reseau_avl_liberer(NoeudAVL *racine);

#endif
