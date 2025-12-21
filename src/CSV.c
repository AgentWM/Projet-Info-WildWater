#define _POSIX_C_SOURCE 200809L

#include "CSV.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LONGUEUR_MAX_LIGNE 2048
#define NOMBRE_COLONNES 5

static void supprimer_saut_ligne(char *s) {
    if (!s) {
        return;
    }
    size_t longueur = strlen(s);
    while (longueur > 0 && (s[longueur - 1] == '\n' || s[longueur - 1] == '\r')) {
        s[--longueur] = '\0';
    }
}

static char *nettoyer_champ(char *champ) {
    if (!champ) {
        return NULL;
    }
    while (*champ == ' ' || *champ == '\t') {
        champ++;
    }
    size_t longueur = strlen(champ);
    while (longueur > 0 && (champ[longueur - 1] == ' ' || champ[longueur - 1] == '\t')) {
        champ[--longueur] = '\0';
    }
    return champ;
}

static int decouper_ligne_point_virgule(char *ligne, char **colonnes, int max_colonnes) {
    if (!ligne || !colonnes || max_colonnes <= 0) {
        return 0;
    }

    supprimer_saut_ligne(ligne);
    size_t longueur_totale = strlen(ligne);
    char *emplacement_vide = ligne + longueur_totale;

    int compte = 0;
    char *curseur = ligne;
    while (compte < max_colonnes) {
        colonnes[compte++] = curseur;
        char *separateur = strchr(curseur, ';');
        if (!separateur) {
            break;
        }
        *separateur = '\0';
        curseur = separateur + 1;
    }

    for (int i = 0; i < compte; ++i) {
        colonnes[i] = nettoyer_champ(colonnes[i]);
    }

    while (compte < max_colonnes) {
        colonnes[compte++] = emplacement_vide;
    }

    return compte;
}

static int commence_par(const char *s, const char *prefixe) {
    if (!s || !prefixe) {
        return 0;
    }
    size_t long_p = strlen(prefixe);
    return strncmp(s, prefixe, long_p) == 0;
}

static int est_tiret(const char *s) {
    return !s || s[0] == '\0' || strcmp(s, "-") == 0;
}

static int est_source(const char *s) {
    return commence_par(s, "Spring ") ||
           commence_par(s, "Source ") ||
           commence_par(s, "Well ") ||
           commence_par(s, "Well field ") ||
           commence_par(s, "Fountain ") ||
           commence_par(s, "Resurgence ");
}

static int est_installation(const char *s) {
    return commence_par(s, "Facility complex ") ||
           commence_par(s, "Facility complex #") ||
           commence_par(s, "Plant ") ||
           commence_par(s, "Module ") ||
           commence_par(s, "Unit ");
}

int csv_analyser_pour_histo(const char *nom_fichier, Usine **racine) {
    if (!nom_fichier || !racine) {
        return 1;
    }

    FILE *f = fopen(nom_fichier, "r");
    if (!f) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s\n", nom_fichier);
        return 1;
    }

    char ligne[LONGUEUR_MAX_LIGNE];
    char *colonnes[NOMBRE_COLONNES];

    while (fgets(ligne, sizeof(ligne), f)) {
        int lus = decouper_ligne_point_virgule(ligne, colonnes, NOMBRE_COLONNES);
        if (lus < NOMBRE_COLONNES) {
            continue;
        }

        const char *c2 = colonnes[1];
        const char *c3 = colonnes[2];
        const char *c4 = colonnes[3];
        const char *c5 = colonnes[4];

        if (est_source(c2) && est_installation(c3)) {
            if (!est_tiret(c4) && !est_tiret(c5)) {
                double volume_capte = atof(c4);
                double fuite = atof(c5);
                double volume_reel = volume_capte * (1.0 - fuite / 100.0);
                *racine = usine_inserer(*racine, c3, 0, volume_capte, volume_reel);
            }
        } else if (est_installation(c2) && est_tiret(c3)) {
            if (!est_tiret(c4)) {
                long capacite = atol(c4);
                *racine = usine_inserer(*racine, c2, capacite, 0.0, 0.0);
            }
        }
    }

    fclose(f);
    return 0;
}

int csv_analyser_pour_fuites(const char *nom_fichier, Usine **racine_usines, NoeudAVL **racine_noeuds) {
    if (!nom_fichier || !racine_usines || !racine_noeuds) {
        return 1;
    }

    FILE *f = fopen(nom_fichier, "r");
    if (!f) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s\n", nom_fichier);
        return 1;
    }

    char ligne[LONGUEUR_MAX_LIGNE];
    char *colonnes[NOMBRE_COLONNES];

    while (fgets(ligne, sizeof(ligne), f)) {
        int lus = decouper_ligne_point_virgule(ligne, colonnes, NOMBRE_COLONNES);
        if (lus < NOMBRE_COLONNES) {
            continue;
        }

        const char *c1 = colonnes[0];
        const char *c2 = colonnes[1];
        const char *c3 = colonnes[2];
        const char *c4 = colonnes[3];
        const char *c5 = colonnes[4];

        if (est_source(c2) && est_installation(c3)) {
            if (!est_tiret(c4) && !est_tiret(c5)) {
                double v_capte = atof(c4);
                double v_reel = v_capte * (1.0 - atof(c5) / 100.0);
                *racine_usines = usine_inserer(*racine_usines, c3, 0, v_capte, v_reel);
            }
        } else if (est_installation(c2) && est_tiret(c3)) {
            if (!est_tiret(c4)) {
                *racine_usines = usine_inserer(*racine_usines, c2, atol(c4), 0.0, 0.0);
            }
        }

        if (!est_tiret(c2) && !est_tiret(c3) && !est_tiret(c5) &&
            !est_source(c2) && (est_tiret(c1) || est_installation(c1))) {

            Node *parent = network_avl_find(*racine_noeuds, c2);
            if (!parent) {
                *racine_noeuds = network_avl_insert(*racine_noeuds, c2, &parent);
            }

            Node *enfant = network_avl_find(*racine_noeuds, c3);
            if (!enfant) {
                *racine_noeuds = network_avl_insert(*racine_noeuds, c3, &enfant);
            }

            enfant->fuite = atof(c5);
            network_add_child(parent, enfant);
        }
    }

    fclose(f);
    return 0;
}
