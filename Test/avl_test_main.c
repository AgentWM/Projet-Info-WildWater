#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

#define BUFFER_SIZE 2048

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Erreur : Nombre d'arguments incorrect.\n");
        printf("Usage : %s <fichier_entree.csv> <fichier_sortie.csv>\n", argv[0]);
        return 1;
    }

    FILE* fichierEntree = fopen(argv[1], "r");
    if (fichierEntree == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier d'entrée %s\n", argv[1]);
        return 1;
    }

    FILE* fichierSortie = fopen(argv[2], "w");
    if (fichierSortie == NULL) {
        printf("Erreur : Impossible de créer le fichier de sortie %s\n", argv[2]);
        fclose(fichierEntree);
        return 1;
    }

    Station* racine = NULL;
    char buffer[BUFFER_SIZE];
    
    char *col2, *col3, *col4;
    long capacite = 0;
    long volume = 0;

    while (fgets(buffer, BUFFER_SIZE, fichierEntree) != NULL) {
        strtok(buffer, ";"); 

        col2 = strtok(NULL, ";");
        col3 = strtok(NULL, ";");
        col4 = strtok(NULL, ";");

        if (col2 == NULL || col3 == NULL || col4 == NULL) {
            continue;
        }

        if (strstr(col3, "-") != NULL) {
            capacite = atol(col4);
            volume = 0;
            racine = insererStation(racine, col2, capacite, volume);
        }
        else if (strstr(col3, "Unit") != NULL || 
                 strstr(col3, "Plant") != NULL || 
                 strstr(col3, "Facility") != NULL ||
                 strstr(col3, "Module") != NULL) {
            
            capacite = 0;
            volume = atol(col4);
            racine = insererStation(racine, col3, capacite, volume);
        }
    }

    traiterSortie(racine, fichierSortie);

    libererArbre(racine);
    fclose(fichierEntree);
    fclose(fichierSortie);

    return 0;
}