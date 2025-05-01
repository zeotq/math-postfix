#include "OperationsTree.h"
#include "Tokens.h"

token_tree * create_token_tree(tokens_vec * postfix_tokens) {
    if (tokens_vec_size(postfix_tokens) == 0) {
        printf("Empty input...\n");
        return NULL;
    }

    token_tree *tree = token_tree_create_empty();
    token_tree_node_vector *stack = token_tree_node_vector_create();

    tokens_vec_iterator it = tokens_vec_iterator_begin(postfix_tokens);
    while (tokens_vec_iterator_has_next(&it)) {
        Token token = *tokens_vec_iterator_next(&it);
        Token *copy = malloc(sizeof(Token));
        *copy = token;
        token_tree_node *node = token_tree_create_node(copy);

        if (token.token_type == TOKEN_TYPE_NUM || token.token_type == TOKEN_TYPE_VAR) {
            token_tree_node_vector_push_back(stack, node);

        } else if (token.token_type == TOKEN_TYPE_OPERATION) {
            if (token.data.operation == TOKEN_OPERATOR_UNARY_MINUS) {
                if (token_tree_node_vector_size(stack) < 1) {
                    printf("<UE> ");  // Unary Operator Error
                    free(copy);
                    token_tree_node_vector_destroy(stack);
                    token_tree_destroy(tree);
                    return NULL;
                }
                token_tree_node *child = token_tree_node_vector_pop_back(stack);
                token_tree_node_vector_push_back(node->children, child);
                child->parent = node;

            } else {
                if (token_tree_node_vector_size(stack) < 2) {
                    printf("<BE> "); // Binart Operator Error
                    free(copy);
                    token_tree_node_vector_destroy(stack);
                    token_tree_destroy(tree);
                    return NULL;
                }
                token_tree_node *right = token_tree_node_vector_pop_back(stack);
                token_tree_node *left  = token_tree_node_vector_pop_back(stack);

                token_tree_node_vector_push_back(node->children, right);
                right->parent = node;
                token_tree_node_vector_push_back(node->children, left);
                left->parent = node;
            }
            token_tree_node_vector_push_back(stack, node);
        }
        else {
            free(copy);
            free(node);
        }
        continue;
    }

    if (token_tree_node_vector_size(stack) != 1) {
        printf("<RW> ");
        token_tree_node_vector_destroy(stack);
        token_tree_destroy(tree);
        return NULL;
    }

    tree->root_node = token_tree_node_vector_pop_back(stack);
    token_tree_node_vector_destroy(stack);
    return tree;
}


void print_node_recursive(token_tree_node * node, char * buffer, size_t depth) {
    for (size_t d = 0; d <= depth; ++d) printf("->");
    str_token(buffer, *node->data);
    printf(" %s\n", buffer);
    token_tree_node_vector_iterator it = token_tree_node_vector_iterator_begin(node->children);
    while (token_tree_node_vector_iterator_has_next(&it)) {
        token_tree_node * next = *token_tree_node_vector_iterator_next(&it);
        print_node_recursive(next, buffer, depth + 1);
    }
}

void print_token_tree(token_tree * tree) {
    char buffer[64];
    if (tree->root_node) {
        print_node_recursive(tree->root_node, buffer, 0);
    } else {
        printf("Empty!\n");
    }
}