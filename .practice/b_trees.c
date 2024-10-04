#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>

#define MAX_ELEMENTS 5
#define MAX_CHILDREN 6

typedef struct bnode
{
    int n;
    int keys[MAX_ELEMENTS];
    bool leaf;
    struct bnode **children;
} BTreeNode;

typedef struct
{
    int t;
    BTreeNode *rootNode;
} BTree;

int main(void)
{

}

BTree *init_btree()
{
    BTree *root = malloc(sizeof(BTree));
    if (root == NULL)
    {
        printf("Root malloc failed\n");
        return NULL;
    }

    root->t = ceil(MAX_ELEMENTS / (float) 2);
    root->rootNode = NULL;

    return root;
}

BTreeNode *create_node(bool leaf)
{
    BTreeNode *node = malloc(sizeof(BTreeNode));

    if (node == NULL)
    {
        printf("Memory allocation fialed at create_node()");
        return NULL;
    }

    node->n = 0;
    node->leaf = leaf;
    node-> children = malloc((MAX_ELEMENTS + 1) * sizeof(BTreeNode*));
    for (int i = 0; i < MAX_ELEMENTS; i++)
    {
        node->keys[i] = INT_MIN;
    }
    for (int i = 0; i <= MAX_ELEMENTS; i++)
    {
        node->children[i] = NULL;
    }
    return node;
}
