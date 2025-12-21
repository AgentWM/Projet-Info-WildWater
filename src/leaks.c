#include "LEAKS.h"
#include <stdio.h>

static int compter_enfants(Node *n) {
    int compte = 0;
    for (Child *c = n ? n->children : NULL; c; c = c->next) {
        compte++;
    }
    return compte;
}

static double calculer_fuites(Node *n, double volume) {
    if (!n || volume <= 0.0) {
        return 0.0;
    }
    
    double pertes_locales = volume * (n->fuite / 100.0);
    double reste = volume - pertes_locales;
    
    int nb_enfants = compter_enfants(n);
    if (nb_enfants == 0) {
        return pertes_locales;
    }
    
    double part = reste / nb_enfants;
    double total = pertes_locales;
    
    for (Child *c = n->children; c; c = c->next) {
        total += calculer_fuites(c->node, part);
    }
    
    return total;
}

int fuites_calculer(const char *fichier_donnees, const char *id_usine, const char *fichier_sortie) {
    if (!fichier_donnees || !id_usine || !fichier_sortie) {
        return 1;
    }
    
    Usine *racine_usines = NULL;
    NodeAVL *racine_noeuds = NULL;
    
    if (csv_analyser_pour_fuites(fichier_donnees, &racine_usines, &racine_noeuds) != 0) {
        return 1;
    }
    
    Usine *u = usine_rechercher(racine_usines, id_usine);
    double resultat_Mm3 = -1.0;
    
    if (u) {
        Node *noeud_usine = network_avl_find(racine_noeuds, id_usine);
        if (noeud_usine) {
            double volume_initial = u->volume_reel;
            int nb_stockages = compter_enfants(noeud_usine);
            
            if (nb_stockages > 0) {
                double volume_par_stockage = volume_initial / nb_stockages;
                double total_pertes_k = 0.0;
                
                for (Child *c = noeud_usine->children; c; c = c->next) {
                    total_pertes_k += calculer_fuites(c->node, volume_par_stockage);
                }
                
                resultat_Mm3 = total_pertes_k / 1000.0;
            } else {
                resultat_Mm3 = 0.0;
            }
        } else {
            resultat_Mm3 = 0.0;
        }
    }
    
    FILE *fsortie = fopen(fichier_sortie, "a+");
    if (!fsortie) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s en écriture\n", fichier_sortie);
        usine_liberer(racine_usines);
        network_avl_free(racine_noeuds);
        return 2;
    }
    
    int besoin_entete = 0;
    if (fseek(fsortie, 0, SEEK_END) == 0 && ftell(fsortie) == 0) {
        besoin_entete = 1;
    }
    
    if (besoin_entete) {
        fprintf(fsortie, "identifier;Leak volume(M.m3/year)\n");
    }
    
    fprintf(fsortie, "%s;%.6f\n", id_usine, resultat_Mm3);
    
    fclose(fsortie);
    usine_liberer(racine_usines);
    network_avl_free(racine_noeuds);
    
    return 0;
}
