#define _POSIX_C_SOURCE 200809L

#include "NETWORK.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int max_int(int a, int b) {
    return (a > b) ? a : b;
}

static int height(NodeAVL *n) {
    return n ? n->height : 0;
}

static Node *create_node(const char *id) {
    Node *n = malloc(sizeof(*n));
    if (!n) {
        fprintf(stderr, "Erreur: malloc Node\n");
        exit(1);
    }

    n->id = strdup(id);
    if (!n->id) {
        fprintf(stderr, "Erreur: strdup Node id\n");
        free(n);
        exit(1);
    }

    n->fuite = 0.0;
    n->children = NULL;

    return n;
}

static NodeAVL *create_avl_node(const char *id, Node *node) {
    NodeAVL *a = malloc(sizeof(*a));
    if (!a) {
        fprintf(stderr, "Erreur: malloc NodeAVL\n");
        exit(1);
    }

    a->id = strdup(id);
    if (!a->id) {
        fprintf(stderr, "Erreur: strdup NodeAVL id\n");
        free(a);
        exit(1);
    }

    a->node = node;
    a->left = NULL;
    a->right = NULL;
    a->height = 1;

    return a;
}

static NodeAVL *rotate_right(NodeAVL *y) {
    NodeAVL *x = y->left;
    NodeAVL *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max_int(height(y->left), height(y->right)) + 1;
    x->height = max_int(height(x->left), height(x->right)) + 1;

    return x;
}

static NodeAVL *rotate_left(NodeAVL *x) {
    NodeAVL *y = x->right;
    NodeAVL *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max_int(height(x->left), height(x->right)) + 1;
    y->height = max_int(height(y->left), height(y->right)) + 1;

    return y;
}

static int balance(NodeAVL *n) {
    return n ? height(n->left) - height(n->right) : 0;
}

Node *network_avl_find(NodeAVL *root, const char *id) {
    if (!root) {
        return NULL;
    }

    int cmp = strcmp(id, root->id);
    if (cmp == 0) {
        return root->node;
    }
    if (cmp < 0) {
        return network_avl_find(root->left, id);
    }
    return network_avl_find(root->right, id);
}

NodeAVL *network_avl_insert(NodeAVL *root, const char *id, Node **out_node) {
    if (!root) {
        Node *n = create_node(id);
        if (out_node) {
            *out_node = n;
        }
        return create_avl_node(id, n);
    }

    int cmp = strcmp(id, root->id);
    if (cmp < 0) {
        root->left = network_avl_insert(root->left, id, out_node);
    } else if (cmp > 0) {
        root->right = network_avl_insert(root->right, id, out_node);
    } else {
        if (out_node) {
            *out_node = root->node;
        }
        return root;
    }

    root->height = 1 + max_int(height(root->left), height(root->right));
    int bal = balance(root);

    if (bal > 1 && strcmp(id, root->left->id) < 0) {
        return rotate_right(root);
    }

    if (bal < -1 && strcmp(id, root->right->id) > 0) {
        return rotate_left(root);
    }

    if (bal > 1 && strcmp(id, root->left->id) > 0) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }

    if (bal < -1 && strcmp(id, root->right->id) < 0) {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }

    return root;
}

void network_add_child(Node *parent, Node *child) {
    if (!parent || !child) {
        return;
    }

    Child *c = malloc(sizeof(*c));
    if (!c) {
        fprintf(stderr, "Erreur: malloc Child\n");
        exit(1);
    }

    c->node = child;
    c->next = parent->children;
    parent->children = c;
}

static void free_node(Node *n) {
    if (!n) {
        return;
    }

    Child *c = n->children;
    while (c) {
        Child *next = c->next;
        free(c);
        c = next;
    }

    free(n->id);
    free(n);
}

void network_avl_free(NodeAVL *root) {
    if (!root) {
        return;
    }

    network_avl_free(root->left);
    network_avl_free(root->right);
    free_node(root->node);
    free(root->id);
    free(root);
}
