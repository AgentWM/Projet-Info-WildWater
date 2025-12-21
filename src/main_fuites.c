#include <stdio.h>
#include <stdlib.h>
#include "LEAKS.h"

int main(int argc, char *argv[]) {
    if (argc != 3) { // Les 3 arguments demandés dans le shell
        fprintf(stderr, "Usage: %s <fichier_entree> <id_usine>\n", argv[0]);
        return 1;
    }
    
    const char *fichier_donnees = argv[1];
    const char *id_usine = argv[2];
    const char *fichier_sortie = "/dev/stdout"; // Pour éviter tout problème avec >> dans le script du shell
    
    // On appelle la fonction du groupe
    int r = fuites_calculer(fichier_donnees, id_usine, fichier_sortie);
    return r;
}
