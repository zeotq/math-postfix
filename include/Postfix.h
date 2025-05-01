#ifndef CONVERT_TO_POSTFIX_H
#define CONVERT_TO_POSTFIX_H

#include "Tokens.h"
#include "Vector.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


tokens_vec * convert_to_postfix_form(tokens_vec * source_tokens);

static int64_t  get_value_int64(        Token token                                 );
static uint64_t get_value_uint64(       Token token                                 );
static double   get_value_double(       Token token                                 );
static int64_t  apply_operator_int64(   int64_t a,      int64_t b,  TokenOperator op);
static uint64_t apply_operator_uint64(  uint64_t a,     uint64_t b, TokenOperator op);
static double   apply_operator_double(  double a,       double b,   TokenOperator op);


Z_VECTOR(int64_t,   postfix_stack_int64_t)
Z_VECTOR(uint64_t,  postfix_stack_uint64_t)
Z_VECTOR(double,    postfix_stack_double)


#define POSTFIX_CALC_TEMPLATE(TYPE, CUSTOM_NAME, GET_FN, APPLY_FN)                        \
    TYPE calc_##CUSTOM_NAME##_postfix_form(tokens_vec *postfix_tokens) {                  \
        postfix_stack_##CUSTOM_NAME *stack = postfix_stack_##CUSTOM_NAME##_create();      \
        tokens_vec_iterator it = tokens_vec_iterator_begin(postfix_tokens);               \
                                                                                          \
        while (tokens_vec_iterator_has_next(&it)) {                                       \
            Token token = *tokens_vec_iterator_next(&it);                                 \
                                                                                          \
            if (token.token_type == TOKEN_TYPE_NUM) {                                     \
                TYPE val = GET_FN(token);                                                 \
                postfix_stack_##CUSTOM_NAME##_push_back(stack, val);                      \
                                                                                          \
            } else if (token.token_type == TOKEN_TYPE_OPERATION) {                        \
                if (postfix_stack_##CUSTOM_NAME##_size(stack) >= 1                        \
                    && token.data.operation == TOKEN_OPERATOR_UNARY_MINUS) {              \
                    TYPE a = postfix_stack_##CUSTOM_NAME##_pop_back(stack);               \
                    TYPE res = APPLY_FN(a, (TYPE)0, token.data.operation);                \
                    postfix_stack_##CUSTOM_NAME##_push_back(stack, res);                  \
                                                                                          \
                } else if (postfix_stack_##CUSTOM_NAME##_size(stack) >= 2) {              \
                    TYPE b = postfix_stack_##CUSTOM_NAME##_pop_back(stack);               \
                    TYPE a = postfix_stack_##CUSTOM_NAME##_pop_back(stack);               \
                    if (b == (TYPE)0 && token.data.operation == TOKEN_OPERATOR_DIV) {     \
                        printf(ANSI_COLOR_RED "<ZD> " ANSI_COLOR_RESET); /*Zero division*/\
                        postfix_stack_##CUSTOM_NAME##_destroy(stack);                     \
                        return (TYPE)0;                                                   \
                    }                                                                     \
                    TYPE res = APPLY_FN(a, b, token.data.operation);                      \
                    postfix_stack_##CUSTOM_NAME##_push_back(stack, res);                  \
                                                                                          \
                } else {                                                                  \
                    printf(ANSI_COLOR_RED "<%c OE> " ANSI_COLOR_RESET, /*Operator Error*/ \
                           get_char_from_operation(token.data.operation));                \
                }                                                                         \
                                                                                          \
            } else {                                                                      \
                printf(ANSI_COLOR_YELLOW "<UT> " ANSI_COLOR_RESET); /* Unexpected token*/ \
            }                                                                             \
        }                                                                                 \
                                                                                          \
        if (postfix_stack_##CUSTOM_NAME##_size(stack) != 1) {                             \
            printf(ANSI_COLOR_YELLOW "<RW> " ANSI_COLOR_RESET);           /*Result Warn*/ \
            postfix_stack_##CUSTOM_NAME##_destroy(stack);                                 \
            return (TYPE)0;                                                               \
        }                                                                                 \
                                                                                          \
        TYPE result = postfix_stack_##CUSTOM_NAME##_pop_back(stack);                      \
        postfix_stack_##CUSTOM_NAME##_destroy(stack);                                     \
        return result;                                                                    \
    }

    int64_t  calc_int64_t_postfix_form(tokens_vec *postfix_tokens);
    uint64_t calc_uint64_t_postfix_form(tokens_vec *postfix_tokens);
    double   calc_double_postfix_form(tokens_vec *postfix_tokens);

#endif // CONVERT_TO_POSTFIX_H