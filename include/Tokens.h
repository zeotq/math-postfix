#ifndef TOKENS_H
#define TOKENS_H

#include <stdint.h>
#include <Vector.h>

typedef enum {
    TOKEN_TYPE_NULL,
    TOKEN_TYPE_OPERATION, 
    TOKEN_TYPE_VAR,
    TOKEN_TYPE_NUM,
} TokenType;

typedef enum {
    TOKEN_OPERATOR_NULL, // NULL OPERATION
    TOKEN_OPERATOR_PLUS, // +
    TOKEN_OPERATOR_MINUS, // -
    TOKEN_OPERATOR_UNARY_MINUS, // 
    TOKEN_OPERATOR_MULT, // *
    TOKEN_OPERATOR_DIV, // /
    TOKEN_OPERATOR_POW, // ^
    TOKEN_OPERATOR_BL, // /
    TOKEN_OPERATOR_BR, // ^
} TokenOperator;

typedef enum {
    TOKEN_ASSOCIATIVITY_LEFT, // 0
    TOKEN_ASSOCIATIVITY_RIGHT, // 1
} TokenAssociativity;

typedef enum {
    NUM_TYPE_INT,
    NUM_TYPE_UINT,
    NUM_TYPE_DOUBLE,
} NumType;

typedef struct Numeric_ {
    union {
        int64_t int_value;
        uint64_t uint_value;
        double double_value;
    } value;
    NumType type;
} Numeric;

typedef struct Token_
{
    TokenType token_type;
    union {
        Numeric numeric;
        char var;
        TokenOperator operation;
    } data;
} Token;

Z_VECTOR(Token, tokens_vec)

bool isNumber(char c);
bool isVariable(char c);
bool isOperation(char c);

TokenOperator get_operation_from_char(char c);
char get_char_from_operation(TokenOperator c);
void str_token(char *buffer, Token token);

tokens_vec * tokenization(const char *source, NumType mode);

int16_t get_operation_priority(TokenOperator operation);
TokenAssociativity get_operation_associativity(TokenOperator operation);
bool cmp_operations(TokenOperator current, TokenOperator other);

#endif // TOKENS_H