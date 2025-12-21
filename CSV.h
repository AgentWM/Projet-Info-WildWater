#ifndef CSV_H
#define CSV_H

#include "AVL_USINES.h"
#include "network.h"

typedef enum {
    HISTO_MAX = 0,
    HISTO_SRC = 1,
    HISTO_REAL = 2
} HistoType;

int csv_parse_for_histo(const char *filename, Usine **racine);

int csv_parse_for_leaks(const char *filename, Usine **racine_usines,
                        NodeAVL **racine_nodes);

#endif
