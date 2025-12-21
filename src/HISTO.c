#include "HISTO.h"
#include <stdio.h>

int histo_generer(const char *fichier_donnees, TypeHisto type, const char *fichier_sortie) {
    if (!fichier_donnees || !fichier_sortie) {
        return 1;
    }
    
    Usine *racine = NULL;
    if (csv_analyser_pour_histo(fichier_donnees, &racine) != 0) {
        return 1;
    }
    
    FILE *fsortie = fopen(fichier_sortie, "w");
    if (!fsortie) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s en écriture\n", fichier_sortie);
        usine_liberer(racine);
        return 2;
    }
    
    if (type == HISTO_MAX) {
        fprintf(fsortie, "identifier;max volume(M.m3/an)\n");
    } else if (type == HISTO_SRC) {
        fprintf(fsortie, "identifier;source volume(M.m3/an)\n");
    } else {
        fprintf(fsortie, "identifier;real volume(M.m3/an)\n");
    }
    
    usine_ecrire_histo(racine, fsortie, (int)type);
    
    fclose(fsortie);
    usine_liberer(racine);
    return 0;
}
