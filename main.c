#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csv.h"
#include "histo.h"
#include "leaks.h"

static void usage(const char *prog) {
    fprintf(stderr,
            "Usage:\n"
            "  %s <datafile> histo <max|src|real> <outputfile>\n"
            "  %s <datafile> leaks \"<Facility ID>\" <outputfile>\n",
            prog, prog);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    if (argc == 5 && strcmp(argv[2], "histo") == 0) {
        const char *datafile = argv[1];
        const char *mode = argv[3];
        const char *outputfile = argv[4];

        HistoType type;
        if (strcmp(mode, "max") == 0) {
            type = HISTO_MAX;
        } else if (strcmp(mode, "src") == 0) {
            type = HISTO_SRC;
        } else if (strcmp(mode, "real") == 0) {
            type = HISTO_REAL;
        } else {
            fprintf(stderr, "Erreur: type histo inconnu '%s'\n", mode);
            fprintf(stderr, "Types valides: max, src, real\n");
            return 2;
        }

        return histo_generate(datafile, type, outputfile);
    }

    if (argc == 5 && strcmp(argv[2], "leaks") == 0) {
        const char *datafile = argv[1];
        const char *usine_id = argv[3];
        const char *outputfile = argv[4];

        return leaks_compute(datafile, usine_id, outputfile);
    }

    fprintf(stderr, "Erreur: arguments invalides\n");
    usage(argv[0]);
    return 1;
}
