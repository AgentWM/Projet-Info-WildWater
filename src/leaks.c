#include "LEAKS.h"
#include <stdio.h>

// Compte le nombre d'enfants directs d'un nœud dans le réseau
static int compter_enfants(Node *n) {
    int compte = 0;
    // Parcourt la liste chaînée des enfants
    for (Child *c = n ? n->children : NULL; c; c = c->next) {
        compte++;
    }
    return compte;
}

// Calcule récursivement les fuites totales dans l'arbre à partir d'un nœud
static double calculer_fuites(Node *n, double volume) {
    if (!n || volume <= 0.0) {
        return 0.0;
    }
    
    // Calcul des pertes locales au nœud actuel (pourcentage de fuite)
    double pertes_locales = volume * (n->fuite / 100.0);
    double reste = volume - pertes_locales;
    
    // Si c'est une feuille (pas d'enfants), retourne uniquement les pertes locales
    int nb_enfants = compter_enfants(n);
    if (nb_enfants == 0) {
        return pertes_locales;
    }
    
    // Répartit équitablement le volume restant entre tous les enfants
    double part = reste / nb_enfants;
    double total = pertes_locales;
    
    // Calcul récursif des fuites pour chaque enfant
    for (Child *c = n->children; c; c = c->next) {
        total += calculer_fuites(c->node, part);
    }
    
    return total;
}

// Calcule les fuites d'une usine et écrit le résultat dans un fichier CSV
int fuites_calculer(const char *fichier_donnees, const char *id_usine, const char *fichier_sortie) {
    // Vérification des paramètres
    if (!fichier_donnees || !id_usine || !fichier_sortie) {
        return 1;
    }
    
    // Initialisation des structures de données
    Usine *racine_usines = NULL;
    NodeAVL *racine_noeuds = NULL;
    
    // Parse le CSV et construit l'arbre d'usines + le réseau de nœuds
    if (csv_analyser_pour_fuites(fichier_donnees, &racine_usines, &racine_noeuds) != 0) {
        return 1;
    }
    
    // Recherche l'usine demandée dans l'arbre
    Usine *u = usine_rechercher(racine_usines, id_usine);
    double resultat_Mm3 = -1.0;
    
    if (u) {
        // Recherche le nœud correspondant dans le réseau
        Node *noeud_usine = network_avl_find(racine_noeuds, id_usine);
        if (noeud_usine) {
            // Récupère le volume réel produit par l'usine
            double volume_initial = u->volume_reel;
            int nb_stockages = compter_enfants(noeud_usine);
            
            if (nb_stockages > 0) {
                // Répartit le volume entre les stockages connectés
                double volume_par_stockage = volume_initial / nb_stockages;
                double total_pertes_k = 0.0;
                
                // Calcule les fuites totales pour chaque branche du réseau
                for (Child *c = noeud_usine->children; c; c = c->next) {
                    total_pertes_k += calculer_fuites(c->node, volume_par_stockage);
                }
                
                // Conversion de kWh en MWh (division par 1000)
                resultat_Mm3 = total_pertes_k / 1000.0;
            } else {
                // Pas de stockage connecté = pas de fuites
                resultat_Mm3 = 0.0;
            }
        } else {
            // Nœud non trouvé dans le réseau = pas de fuites
            resultat_Mm3 = 0.0;
        }
    }
    
    // Ouvre le fichier en mode ajout 
    FILE *fsortie = fopen(fichier_sortie, "a+");
    if (!fsortie) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s en écriture\n", fichier_sortie);
        usine_liberer(racine_usines);
        network_avl_free(racine_noeuds);
        return 2;
    }
    
    // Vérifie si le fichier est vide pour ajouter l'en-tête
    int besoin_entete = 0;
    if (fseek(fsortie, 0, SEEK_END) == 0 && ftell(fsortie) == 0) {
        besoin_entete = 1;
    }
    
    // Écrit l'en-tête CSV si nécessaire
    if (besoin_entete) {
        fprintf(fsortie, "identifier;Leak volume(M.m3/year)\n");
    }
    
    // Écrit le résultat pour l'usine demandée
    fprintf(fsortie, "%s;%.6f\n", id_usine, resultat_Mm3);
    
    // Fermeture et libération de la mémoire
    fclose(fsortie);
    usine_liberer(racine_usines);
    network_avl_free(racine_noeuds);
    
    return 0;  // Succès
}
