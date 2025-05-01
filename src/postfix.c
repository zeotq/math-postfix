#include "Postfix.h"
#include "Tokens.h"
#include "Deque.h"
#include "Vector.h"

#include <stdint.h>
#include <math.h>
#include <assert.h>


// int64_t calc_int64_t_postfix_form(tokens_vec * postfix_tokens);
POSTFIX_CALC_TEMPLATE(int64_t,   int64_t,   get_value_int64,    apply_operator_int64)
// uint64_t calc_uint64_t_postfix_form(tokens_vec * postfix_tokens);
POSTFIX_CALC_TEMPLATE(uint64_t,  uint64_t,  get_value_uint64,   apply_operator_uint64)
// double calc_double_postfix_form(tokens_vec * postfix_tokens);
POSTFIX_CALC_TEMPLATE(double,    double,    get_value_double,   apply_operator_double)


tokens_vec * convert_to_postfix_form(tokens_vec * source_tokens) {
    tokens_vec * q_tokens = tokens_vec_create();
    tokens_vec * stack = tokens_vec_create();
    tokens_vec_iterator source_it = tokens_vec_iterator_begin(source_tokens);

    Token token;
    Token prev_token = {
        .token_type = TOKEN_TYPE_NULL
    };

    while (tokens_vec_iterator_has_next(&source_it)) {
        token = *tokens_vec_iterator_next(&source_it);
        if (token.token_type == TOKEN_TYPE_NUM || token.token_type == TOKEN_TYPE_VAR) {
            tokens_vec_push_back(q_tokens, token);
        } else if (token.token_type == TOKEN_TYPE_OPERATION) {

            if (token.data.operation == TOKEN_OPERATOR_MINUS) {
                if ( prev_token.token_type == 0 || (prev_token.token_type == TOKEN_TYPE_OPERATION &&
                     prev_token.data.operation != TOKEN_OPERATOR_BR)
                ) {
                    token.data.operation = TOKEN_OPERATOR_UNARY_MINUS;
                }
            }

            if (token.data.operation == TOKEN_OPERATOR_BL) {
                tokens_vec_push_back(stack, token);

            } else if (token.data.operation == TOKEN_OPERATOR_BR) {
                bool isFoundedBL = false;
                while (tokens_vec_size(stack)) {
                    Token top = tokens_vec_pop_back(stack);
                    if (top.data.operation == TOKEN_OPERATOR_BL) {
                        isFoundedBL = true;
                        break;
                    } else {
                        tokens_vec_push_back(q_tokens, top);
                    }
                }
                if (!isFoundedBL) {
                    printf(ANSI_COLOR_YELLOW "<can't find TOKEN_OPERATOR_BL> " ANSI_COLOR_RESET);
                }
            } else {
                while (tokens_vec_size(stack)) {
                    Token top = *tokens_vec_at(stack, stack->size - 1);
                    if (cmp_operations(token.data.operation, top.data.operation)) {
                        tokens_vec_push_back(q_tokens, top);
                        tokens_vec_pop_back(stack);
                    } else {
                        break;
                    }
                }
                tokens_vec_push_back(stack, token);
            }
        }
        prev_token = token;
    }

    while (tokens_vec_size(stack)) {
        tokens_vec_push_back(q_tokens, tokens_vec_pop_back(stack));
    }

    tokens_vec_destroy(stack);

    return q_tokens;
}

static int64_t get_value_int64(Token token) {
    return (int64_t)token.data.numeric.value.int_value;
}

static int64_t apply_operator_int64(int64_t a, int64_t b, TokenOperator op) {
    switch (op) {
        case TOKEN_OPERATOR_PLUS:           return (int64_t)a + (int64_t)b;
        case TOKEN_OPERATOR_MINUS:          return (int64_t)a - (int64_t)b;
        case TOKEN_OPERATOR_UNARY_MINUS:    return (int64_t)-a;
        case TOKEN_OPERATOR_MULT:           return (int64_t)a * (int64_t)b;
        case TOKEN_OPERATOR_DIV:            return (int64_t)a / (int64_t)b;
        case TOKEN_OPERATOR_POW:            return pow((int64_t)a, (int64_t)b);

        default: printf(ANSI_COLOR_YELLOW "<Unknown operation> " ANSI_COLOR_RED); return 0;
    }
}

static uint64_t get_value_uint64(Token token) {
    return (uint64_t)token.data.numeric.value.uint_value;
}

static uint64_t apply_operator_uint64(uint64_t a, uint64_t b, TokenOperator op) {
    switch (op) {
        case TOKEN_OPERATOR_PLUS:           return (uint64_t)a + (uint64_t)b;
        case TOKEN_OPERATOR_MINUS:          return (uint64_t)a - (uint64_t)b;
        case TOKEN_OPERATOR_UNARY_MINUS:    return (uint64_t)-a;
        case TOKEN_OPERATOR_MULT:           return (uint64_t)a * (uint64_t)b;
        case TOKEN_OPERATOR_DIV:            return (uint64_t)a / (uint64_t)b;
        case TOKEN_OPERATOR_POW:            return pow((uint64_t)a, (uint64_t)b);

        default: printf(ANSI_COLOR_YELLOW "<Unknown operation> " ANSI_COLOR_RESET); return 0;
    }
}

static double get_value_double(Token token) {
    return (double)token.data.numeric.value.double_value;
}

static double apply_operator_double(double a, double b, TokenOperator op) {
    switch (op) {
        case TOKEN_OPERATOR_PLUS:           return (double)a + (double)b;
        case TOKEN_OPERATOR_MINUS:          return (double)a - (double)b;
        case TOKEN_OPERATOR_UNARY_MINUS:    return -(double)a;
        case TOKEN_OPERATOR_MULT:           return (double)a * (double)b;
        case TOKEN_OPERATOR_DIV:            return (double)a / (double)b;
        case TOKEN_OPERATOR_POW:            return pow((double)a, (double)b);

        default: printf(ANSI_COLOR_YELLOW "<Unknown operation> " ANSI_COLOR_RESET); return 0;
    }
}
