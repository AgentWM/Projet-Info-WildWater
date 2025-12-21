#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVL_USINES.h"

// Parcours infixe inverse (Droit, Racine, Gauche) pour l'ordre alphabétique inverse
void afficher_inverse_simple(Usine *racine) {
    if (!racine) return;

    afficher_inverse_simple(racine->droit);
    // On affiche directement volume_reel.
    printf("%s;%.6f\n", racine->id, racine->volume_reel);

    afficher_inverse_simple(racine->gauche);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("Erreur ouverture fichier");
        return 1;
    }

    Usine *racine = NULL;
    char buffer[500];
    char id[500];
    double valeur;

    while (fgets(buffer, sizeof(buffer), f)) {
        //On découpe la ligne avec ";"
        char *token = strtok(buffer, ";");
        if (!token) continue;
        strcpy(id, token);

        token = strtok(NULL, "\n"); 
        if (!token) continue;
        valeur = atof(token);
        racine = usine_inserer(racine, id, 0, 0, valeur); // On insère dans l'AVL
    }

    fclose(f);

    //Affichage trié et récupéré par le shell
    afficher_inverse_simple(racine);
    //Clean
    usine_liberer(racine);

    return 0;
}
