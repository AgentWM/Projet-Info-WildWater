#ifndef NETWORK_H
#define NETWORK_H

typedef struct Node Node;
typedef struct Child Child;
typedef struct NodeAVL NodeAVL;

struct Child {
    Node *node;
    Child *next;
};

struct Node {
    char *id;
    double fuite;
    Child *children;
};

struct NodeAVL {
    char *id;
    Node *node;
    int height;
    NodeAVL *left;
    NodeAVL *right;
};

Node *network_avl_find(NodeAVL *root, const char *id);

NodeAVL *network_avl_insert(NodeAVL *root, const char *id, Node **out_node);

void network_add_child(Node *parent, Node *child);

void network_avl_free(NodeAVL *root);

#endif
