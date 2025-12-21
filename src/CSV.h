#ifndef CSV_H
#define CSV_H
#include "AVL_USINES.h"
#include "NETWORK.h"

typedef enum {
    HISTO_MAX = 0,  // Maximum
    HISTO_SRC = 1,  // Source
    HISTO_REAL = 2  // Réel
} TypeHisto;

int csv_analyser_pour_histo(const char *nom_fichier, Usine **racine);
int csv_analyser_pour_fuites(const char *nom_fichier, Usine **racine_usines,
                              NoeudAVL **racine_noeuds);
#endif
