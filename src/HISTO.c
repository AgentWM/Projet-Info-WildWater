#include "HISTO.h"
#include <stdio.h>

// Génère un fichier CSV d'histogramme à partir des données d'entrée
int histo_generer(const char *fichier_donnees, TypeHisto type, const char *fichier_sortie) {
    // Vérification des paramètres
    if (!fichier_donnees || !fichier_sortie) {
        return 1;
    }
    
    // Initialise l'arbre AVL des usines
    Usine *racine = NULL;
    
    // Parse le fichier CSV et construit l'arbre
    if (csv_analyser_pour_histo(fichier_donnees, &racine) != 0) {
        return 1;  // Erreur lors de la lecture du fichier
    }
    
    // Ouvre le fichier de sortie en écriture
    FILE *fsortie = fopen(fichier_sortie, "w");
    if (!fsortie) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s en écriture\n", fichier_sortie);
        usine_liberer(racine);
        return 2;
    }
    
    // Écrit l'en-tête CSV selon le type d'histogramme demandé
    if (type == HISTO_MAX) {
        fprintf(fsortie, "identifier;max volume(M.m3/an)\n");
    } else if (type == HISTO_SRC) {
        fprintf(fsortie, "identifier;source volume(M.m3/an)\n");
    } else {
        fprintf(fsortie, "identifier;real volume(M.m3/an)\n");
    }
    
    // Écrit toutes les données de l'arbre dans le fichier daans l'ordre décroissant
    usine_ecrire_histo(racine, fsortie, (int)type);
    
    // Ferme le fichier et libère la mémoire
    fclose(fsortie);
    usine_liberer(racine);
    return 0;  // Succès
}
