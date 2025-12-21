#include "LEAKS.h"

#include <stdio.h>

static int count_children(Node *n) {
    int count = 0;
    for (Child *c = n ? n->children : NULL; c; c = c->next) {
        count++;
    }
    return count;
}

static double compute_leaks(Node *n, double volume) {
    if (!n || volume <= 0.0) {
        return 0.0;
    }

    double pertes_locales = volume * (n->fuite / 100.0);
    double reste = volume - pertes_locales;

    int nb_enfants = count_children(n);
    if (nb_enfants == 0) {
        return pertes_locales;
    }

    double part = reste / nb_enfants;
    double total = pertes_locales;

    for (Child *c = n->children; c; c = c->next) {
        total += compute_leaks(c->node, part);
    }

    return total;
}

int leaks_compute(const char *datafile, const char *usine_id, const char *outputfile) {
    if (!datafile || !usine_id || !outputfile) {
        return 1;
    }

    Usine *racine_usines = NULL;
    NodeAVL *racine_nodes = NULL;

    if (csv_parse_for_leaks(datafile, &racine_usines, &racine_nodes) != 0) {
        return 1;
    }

    Usine *u = usine_rechercher(racine_usines, usine_id);
    double resultat_Mm3 = -1.0;

    if (u) {
        Node *noeud_usine = network_avl_find(racine_nodes, usine_id);
        if (noeud_usine) {
            double volume_initial = u->volume_reel;
            int nb_stockages = count_children(noeud_usine);

            if (nb_stockages > 0) {
                double volume_par_stockage = volume_initial / nb_stockages;
                double total_pertes_k = 0.0;

                for (Child *c = noeud_usine->children; c; c = c->next) {
                    total_pertes_k += compute_leaks(c->node, volume_par_stockage);
                }

                resultat_Mm3 = total_pertes_k / 1000.0;
            } else {
                resultat_Mm3 = 0.0;
            }
        } else {
            resultat_Mm3 = 0.0;
        }
    }

    FILE *fout = fopen(outputfile, "a+");
    if (!fout) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s en écriture\n", outputfile);
        usine_liberer(racine_usines);
        network_avl_free(racine_nodes);
        return 2;
    }

    int need_header = 0;
    if (fseek(fout, 0, SEEK_END) == 0 && ftell(fout) == 0) {
        need_header = 1;
    }

    if (need_header) {
        fprintf(fout, "identifier;Leak volume(M.m3/year)\n");
    }

    fprintf(fout, "%s;%.6f\n", usine_id, resultat_Mm3);
    fclose(fout);

    usine_liberer(racine_usines);
    network_avl_free(racine_nodes);

    return 0;
}
