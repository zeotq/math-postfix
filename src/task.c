#include "Task.h"

bool eq_num_token_value_and_int(Numeric n, int m) {
    switch (n.type) {
        case NUM_TYPE_INT:      return n.value.int_value == m;              break;
        case NUM_TYPE_UINT:     return n.value.uint_value == (uint64_t) m;  break;
        case NUM_TYPE_DOUBLE:   return n.value.double_value == (double) m;  break;
    }
    return false;
}

bool is_numeric_token_equal_to_one(token_tree_node * node) {
    return node->data->token_type == TOKEN_TYPE_NUM &&
           eq_num_token_value_and_int(node->data->data.numeric, 1);
}

bool remove_the_multipliers_equal_to_one(token_tree_node * node) {
    if (node != NULL && node->data->token_type == TOKEN_TYPE_OPERATION && node->data->data.operation == TOKEN_OPERATOR_MULT) {
        if (node->children == NULL || node->children->size != 2) return false;
        token_tree_node * node_1 = *token_tree_node_vector_at(node->children, 0);
        token_tree_node * node_2 = *token_tree_node_vector_at(node->children, 1);
    
        if (is_numeric_token_equal_to_one(node_1)) {
            node->children = node_2->children;
            node->data = node_2->data;
            token_tree_destroy_node(node_1);
            free(node_2);
            remove_the_multipliers_equal_to_one(node);
            return true;
        }
        else if (is_numeric_token_equal_to_one(node_2)) {
            node->children = node_1->children;
            node->data = node_1->data;
            token_tree_destroy_node(node_2);
            free(node_1);
            remove_the_multipliers_equal_to_one(node);
            return true;
        }
    }
    return false;
}

bool find_the_multipliers_equal_to_one(token_tree_node * node) {
    if (node == NULL || node->children == NULL) return false;

    bool someDeleted = false;
    someDeleted |= remove_the_multipliers_equal_to_one(node);

    token_tree_node_vector_iterator it = token_tree_node_vector_iterator_begin(node->children);
    while (token_tree_node_vector_iterator_has_next(&it)) {
        someDeleted |= find_the_multipliers_equal_to_one(*token_tree_node_vector_iterator_next(&it));
    }
    return someDeleted;
}

void remove_the_multipliers(token_tree * tree) {
    if (token_tree_is_empty(tree)) return;
    while (find_the_multipliers_equal_to_one(tree->root_node)) {

    }
}