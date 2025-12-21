#ifndef AVL_USINES_H
#define AVL_USINES_H

#include <stdio.h>

typedef struct Usine {
    char *id;
    long capacite;          /* k.m3.year-1 */
    double volume_capte;    /* k.m3.year-1 */
    double volume_reel;     /* k.m3.year-1 */
    int hauteur;
    struct Usine *gauche;
    struct Usine *droit;
} Usine;

Usine *usine_inserer(Usine *racine, const char *id,
                     long capacite_delta,
                     double volume_capte_delta,
                     double volume_reel_delta);

Usine *usine_rechercher(Usine *racine, const char *id);

void usine_ecrire_histo(Usine *racine, FILE *fout, int type);

void usine_liberer(Usine *racine);

#endif
