#define _POSIX_C_SOURCE 200809L

#include "AVL_USINES.h"

#include <stdlib.h>
#include <string.h>

static int max_int(int a, int b) {
    return (a > b) ? a : b;
}

static int hauteur(Usine *u) {
    return u ? u->hauteur : 0;
}

static Usine *creer_usine(const char *id,
                          long capacite_delta,
                          double volume_capte_delta,
                          double volume_reel_delta) {
    Usine *u = malloc(sizeof(*u));
    if (!u) {
        fprintf(stderr, "Erreur: malloc Usine\n");
        exit(1);
    }

    u->id = strdup(id);
    if (!u->id) {
        fprintf(stderr, "Erreur: strdup Usine id\n");
        free(u);
        exit(1);
    }

    u->capacite = capacite_delta;
    u->volume_capte = volume_capte_delta;
    u->volume_reel = volume_reel_delta;
    u->hauteur = 1;
    u->gauche = NULL;
    u->droit = NULL;

    return u;
}

static Usine *rotation_droite(Usine *y) {
    Usine *x = y->gauche;
    Usine *T2 = x->droit;

    x->droit = y;
    y->gauche = T2;

    y->hauteur = max_int(hauteur(y->gauche), hauteur(y->droit)) + 1;
    x->hauteur = max_int(hauteur(x->gauche), hauteur(x->droit)) + 1;

    return x;
}

static Usine *rotation_gauche(Usine *x) {
    Usine *y = x->droit;
    Usine *T2 = y->gauche;

    y->gauche = x;
    x->droit = T2;

    x->hauteur = max_int(hauteur(x->gauche), hauteur(x->droit)) + 1;
    y->hauteur = max_int(hauteur(y->gauche), hauteur(y->droit)) + 1;

    return y;
}

static int equilibre(Usine *u) {
    return u ? hauteur(u->gauche) - hauteur(u->droit) : 0;
}

Usine *usine_inserer(Usine *racine, const char *id,
                     long capacite_delta,
                     double volume_capte_delta,
                     double volume_reel_delta) {
    if (!racine) {
        return creer_usine(id, capacite_delta, volume_capte_delta, volume_reel_delta);
    }

    int cmp = strcmp(id, racine->id);
    if (cmp < 0) {
        racine->gauche = usine_inserer(racine->gauche, id,
                                       capacite_delta,
                                       volume_capte_delta,
                                       volume_reel_delta);
    } else if (cmp > 0) {
        racine->droit = usine_inserer(racine->droit, id,
                                      capacite_delta,
                                      volume_capte_delta,
                                      volume_reel_delta);
    } else {
        racine->capacite += capacite_delta;
        racine->volume_capte += volume_capte_delta;
        racine->volume_reel += volume_reel_delta;
        return racine;
    }

    racine->hauteur = 1 + max_int(hauteur(racine->gauche), hauteur(racine->droit));

    int bal = equilibre(racine);

    if (bal > 1 && strcmp(id, racine->gauche->id) < 0) {
        return rotation_droite(racine);
    }

    if (bal < -1 && strcmp(id, racine->droit->id) > 0) {
        return rotation_gauche(racine);
    }

    if (bal > 1 && strcmp(id, racine->gauche->id) > 0) {
        racine->gauche = rotation_gauche(racine->gauche);
        return rotation_droite(racine);
    }

    if (bal < -1 && strcmp(id, racine->droit->id) < 0) {
        racine->droit = rotation_droite(racine->droit);
        return rotation_gauche(racine);
    }

    return racine;
}

Usine *usine_rechercher(Usine *racine, const char *id) {
    if (!racine) {
        return NULL;
    }
    int cmp = strcmp(id, racine->id);
    if (cmp == 0) {
        return racine;
    }
    if (cmp < 0) {
        return usine_rechercher(racine->gauche, id);
    }
    return usine_rechercher(racine->droit, id);
}

void usine_ecrire_histo(Usine *racine, FILE *fout, int type) {
    if (!racine || !fout) {
        return;
    }

    usine_ecrire_histo(racine->droit, fout, type);

    double capacite_Mm3 = (double)racine->capacite / 1000.0;
    double volume_capte_Mm3 = racine->volume_capte / 1000.0;
    double volume_reel_Mm3 = racine->volume_reel / 1000.0;

    if (type == 0) {
        fprintf(fout, "%s;%.6f\n", racine->id, capacite_Mm3);
    } else if (type == 1) {
        fprintf(fout, "%s;%.6f\n", racine->id, volume_capte_Mm3);
    } else {
        fprintf(fout, "%s;%.6f\n", racine->id, volume_reel_Mm3);
    }

    usine_ecrire_histo(racine->gauche, fout, type);
}

void usine_liberer(Usine *racine) {
    if (!racine) {
        return;
    }
    usine_liberer(racine->gauche);
    usine_liberer(racine->droit);
    free(racine->id);
    free(racine);
}
