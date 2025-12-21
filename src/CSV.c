#define _POSIX_C_SOURCE 200809L

#include "CSV.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_MAX_LEN 2048
#define COL_COUNT 5

static void chomp(char *s) {
    if (!s) {
        return;
    }
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
    }
}

static char *trim_field(char *field) {
    if (!field) {
        return NULL;
    }
    while (*field == ' ' || *field == '\t') {
        field++;
    }
    size_t len = strlen(field);
    while (len > 0 && (field[len - 1] == ' ' || field[len - 1] == '\t')) {
        field[--len] = '\0';
    }
    return field;
}

static int split_semicolon_line(char *line, char **cols, int max_cols) {
    if (!line || !cols || max_cols <= 0) {
        return 0;
    }

    chomp(line);
    size_t total_len = strlen(line);
    char *empty_slot = line + total_len;

    int count = 0;
    char *cursor = line;
    while (count < max_cols) {
        cols[count++] = cursor;
        char *sep = strchr(cursor, ';');
        if (!sep) {
            break;
        }
        *sep = '\0';
        cursor = sep + 1;
    }

    for (int i = 0; i < count; ++i) {
        cols[i] = trim_field(cols[i]);
    }

    while (count < max_cols) {
        cols[count++] = empty_slot;
    }

    return count;
}

static int starts_with(const char *s, const char *prefix) {
    if (!s || !prefix) {
        return 0;
    }
    size_t lp = strlen(prefix);
    return strncmp(s, prefix, lp) == 0;
}

static int is_dash(const char *s) {
    return !s || s[0] == '\0' || strcmp(s, "-") == 0;
}

static int is_source(const char *s) {
    return starts_with(s, "Spring ") ||
           starts_with(s, "Source ") ||
           starts_with(s, "Well ") ||
           starts_with(s, "Well field ") ||
           starts_with(s, "Fountain ") ||
           starts_with(s, "Resurgence ");
}

static int is_facility(const char *s) {
    return starts_with(s, "Facility complex ") ||
           starts_with(s, "Facility complex #") ||
           starts_with(s, "Plant ") ||
           starts_with(s, "Module ") ||
           starts_with(s, "Unit ");
}

int csv_parse_for_histo(const char *filename, Usine **racine) {
    if (!filename || !racine) {
        return 1;
    }

    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s\n", filename);
        return 1;
    }

    char line[LINE_MAX_LEN];
    char *cols[COL_COUNT];

    while (fgets(line, sizeof(line), f)) {
        int read = split_semicolon_line(line, cols, COL_COUNT);
        if (read < COL_COUNT) {
            continue;
        }

        const char *c2 = cols[1];
        const char *c3 = cols[2];
        const char *c4 = cols[3];
        const char *c5 = cols[4];

        if (is_source(c2) && is_facility(c3)) {
            if (!is_dash(c4) && !is_dash(c5)) {
                double volume_capte = atof(c4);
                double fuite = atof(c5);
                double volume_reel = volume_capte * (1.0 - fuite / 100.0);
                *racine = usine_inserer(*racine, c3, 0, volume_capte, volume_reel);
            }
        } else if (is_facility(c2) && is_dash(c3)) {
            if (!is_dash(c4)) {
                long capacite = atol(c4);
                *racine = usine_inserer(*racine, c2, capacite, 0.0, 0.0);
            }
        }
    }

    fclose(f);
    return 0;
}

int csv_parse_for_leaks(const char *filename, Usine **racine_usines, NodeAVL **racine_nodes) {
    if (!filename || !racine_usines || !racine_nodes) {
        return 1;
    }

    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s\n", filename);
        return 1;
    }

    char line[LINE_MAX_LEN];
    char *cols[COL_COUNT];

    while (fgets(line, sizeof(line), f)) {
        int read = split_semicolon_line(line, cols, COL_COUNT);
        if (read < COL_COUNT) {
            continue;
        }

        const char *c1 = cols[0];
        const char *c2 = cols[1];
        const char *c3 = cols[2];
        const char *c4 = cols[3];
        const char *c5 = cols[4];

        if (is_source(c2) && is_facility(c3)) {
            if (!is_dash(c4) && !is_dash(c5)) {
                double v_capte = atof(c4);
                double v_reel = v_capte * (1.0 - atof(c5) / 100.0);
                *racine_usines = usine_inserer(*racine_usines, c3, 0, v_capte, v_reel);
            }
        } else if (is_facility(c2) && is_dash(c3)) {
            if (!is_dash(c4)) {
                *racine_usines = usine_inserer(*racine_usines, c2, atol(c4), 0.0, 0.0);
            }
        }

        if (!is_dash(c2) && !is_dash(c3) && !is_dash(c5) &&
            !is_source(c2) && (is_dash(c1) || is_facility(c1))) {

            Node *parent = network_avl_find(*racine_nodes, c2);
            if (!parent) {
                *racine_nodes = network_avl_insert(*racine_nodes, c2, &parent);
            }

            Node *child = network_avl_find(*racine_nodes, c3);
            if (!child) {
                *racine_nodes = network_avl_insert(*racine_nodes, c3, &child);
            }

            child->fuite = atof(c5);
            network_add_child(parent, child);
        }
    }

    fclose(f);
    return 0;
}
