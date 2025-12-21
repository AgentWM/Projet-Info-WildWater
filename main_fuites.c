#include <stdio.h>
#include <stdlib.h>
#include "LEAKS.h"

int main(int argc, char *argv[]) {
    if (argc != 3) { //Les 3 arguments demandés dans le shell
        fprintf(stderr, "Usage: %s <input_file> <usine_id>\n", argv[0]);
        return 1;
    }

    const char *datafile = argv[1];
    const char *usine_id = argv[2];
    const char *outputfile = "/dev/stdout"; //Pour éviter tout problème avec >> dans le script du shell
    // On appelle la fonction du groupe
    int r = leaks_compute(datafile, usine_id, outputfile);
    return r;
}
