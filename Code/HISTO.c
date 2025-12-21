#include "HISTO.h"

#include <stdio.h>

int histo_generate(const char *datafile, HistoType type, const char *outputfile) {
    if (!datafile || !outputfile) {
        return 1;
    }

    Usine *racine = NULL;

    if (csv_parse_for_histo(datafile, &racine) != 0) {
        return 1;
    }

    FILE *fout = fopen(outputfile, "w");
    if (!fout) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s en écriture\n", outputfile);
        usine_liberer(racine);
        return 2;
    }

    if (type == HISTO_MAX) {
        fprintf(fout, "identifier;max volume(M.m3/year)\n");
    } else if (type == HISTO_SRC) {
        fprintf(fout, "identifier;source volume(M.m3/year)\n");
    } else {
        fprintf(fout, "identifier;real volume(M.m3/year)\n");
    }

    usine_ecrire_histo(racine, fout, (int)type);

    fclose(fout);
    usine_liberer(racine);

    return 0;
}
