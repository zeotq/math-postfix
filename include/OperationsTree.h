#ifndef OPERATIONS_TREE_H
#define OPERATIONS_TREE_H

#include "Tokens.h"
#include "Tree.h"

Z_TREE(Token*, token_tree)

token_tree * create_token_tree(tokens_vec * postfix_tokens);
void print_token_tree(token_tree * tree);
void print_node_recursive(token_tree_node * node, char * buffer, size_t depth);

#endif // OPERATIONS_TREE_H