#include <stdio.h>
#include <stdlib.h>

#define LETTER 256

int symbol, yylval;

// Structure of a tree node
typedef struct _node {
    char key;
    struct _node *l, *r;
} node;

// Function to get the next symbol
void next_symbol() {
    symbol = yylex();
}

// Forward declarations for recursive-descent functions
node* E();
node* T();
node* F();
node* P();

// Function to create a new node
node* create_node(char key, node* l, node* r) {
    node *new_node = (node *)malloc(sizeof(node));
    if (!new_node) {
        printf("Memory allocation error!\n");
        exit(1);
    }
    new_node->key = key;
    new_node->l = l;
    new_node->r = r;
    return new_node;
}

// E -> T '|' E | T
node* E() {
    node *t = T();
    if (symbol == '|') {
        next_symbol();  // Move past '|'
        node *e = E();
        return create_node('|', t, e);  // Create a node for '|'
    }
    return t;
}

// T -> F T | F
node* T() {
    node *f = F();
    while (symbol == LETTER || symbol == '(' ) {
        node *t = T();
        f = create_node('&', f, t);  // Concatenation using '&'
    }
    return f;
}

// F -> P '*' | P
node* F() {
    node *p = P();
    if (symbol == '*') {
        next_symbol();  // Move past '*'
        return create_node('*', p, NULL);  // Create a node for '' (unary operator)
    }
    return p;
}

// P -> '(' E ')' | LETTER
node* P() { 
    node *p = NULL;
    if (symbol == '(' ) {
        next_symbol(); 
        p = E();       
        if (symbol != ')' ) {
            printf("Error: Expected ') '\n");
            exit(1);
        }
        next_symbol();
    } else if (symbol == LETTER) {
        p = create_node(yylval, NULL, NULL);  // Create a node for the letter
        next_symbol();  // Move to the next symbol
    } else {
        printf("Error: Unexpected symbol '%c'\n", symbol);
        exit(1);
    }
    return p;
}

// Print the tree in a readable format
void print_tree(node *root, int depth) {
    if (root == NULL) return;
    print_tree(root->r, depth + 1);
    for (int i = 0; i < depth; i++) printf("  ");
    printf("%c\n", root->key);
    print_tree(root->l, depth + 1);
}

int main() {
    next_symbol();  // Get the first symbol
    node *root = E();  // Parse the input
    print_tree(root, 0);  // Print the expression tree
    return 0;
}