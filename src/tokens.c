#include "Tokens.h"
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

bool isNumber(char c) {
	return (c >= '0' && c <= '9');
}

bool isVariable(char c) {
	c = tolower(c);
	return (c >= 'a' && c <= 'z');
}

bool isOperation(char c) {
	static const char * operations = "+-u*/^()";
	int opLen = strlen(operations);
	for(size_t i = 0; i < opLen; ++i) 
		if(operations[i] == c)
			return true;
	
	return false;
}

TokenOperator get_operation_from_char(char c) {
    switch (c) {
        case '+':   return TOKEN_OPERATOR_PLUS;
        case '-':   return TOKEN_OPERATOR_MINUS;
        case 'u':   return TOKEN_OPERATOR_UNARY_MINUS;
        case '*':   return TOKEN_OPERATOR_MULT;
        case '/':   return TOKEN_OPERATOR_DIV;
        case '^':   return TOKEN_OPERATOR_POW;
        case '(':   return TOKEN_OPERATOR_BL;
        case ')':   return TOKEN_OPERATOR_BR;
    }
    return TOKEN_OPERATOR_NULL;
}

char get_char_from_operation(TokenOperator c) {
    switch (c) {
        case TOKEN_OPERATOR_PLUS:           return '+';
        case TOKEN_OPERATOR_MINUS:          return '-';
        case TOKEN_OPERATOR_UNARY_MINUS:    return 'u';
        case TOKEN_OPERATOR_MULT:           return '*';
        case TOKEN_OPERATOR_DIV:            return '/';
        case TOKEN_OPERATOR_POW:            return '^';
        case TOKEN_OPERATOR_BL:             return '(';
        case TOKEN_OPERATOR_BR:             return ')';
    }
    return TOKEN_OPERATOR_NULL;
}

/**
 * @brief   Fills in the string with the string value of the token
 * @param   buffer string pointer
 * @param   token
 * @return  void
 **/
void str_token(char *buffer, Token token) {
    switch (token.token_type) {
        case TOKEN_TYPE_OPERATION:
            buffer[0] = get_char_from_operation(token.data.operation);
            buffer[1] = '\0';
            break;
        case TOKEN_TYPE_VAR:
            buffer[0] = token.data.var;
            buffer[1] = '\0';
            break;
        case TOKEN_TYPE_NUM:
            switch (token.data.numeric.type) {
                case NUM_TYPE_INT:
                    snprintf(buffer, 32UL, "%ld", token.data.numeric.value.uint_value);
                    break;
                case NUM_TYPE_UINT:
                    snprintf(buffer, 32UL, "%lu", token.data.numeric.value.uint_value);
                    break;
                case NUM_TYPE_DOUBLE:
                    snprintf(buffer, 64UL, "%lf", token.data.numeric.value.double_value);
                    break;
            }   
            break;
    }
}


tokens_vec * tokenization(const char *source, NumType mode) {
    char        c;
    char        double_buffer[32];
    uint8_t     double_buffer_index = 0;
    uint64_t    uint_buffer = 0; 
    bool        isBufferFilled = false;
    
    Token       token;
    tokens_vec  *result = tokens_vec_create(256);

    for (int i = 0; source[i] != '\0'; i++) {
        c = source[i];

        if (c == ' ') { continue; }

        if (isNumber(c) || (mode == NUM_TYPE_DOUBLE && c == '.')) {
            isBufferFilled = true;
            if (mode == NUM_TYPE_DOUBLE) {
                if (double_buffer_index < sizeof(double_buffer) - 1) {
                    double_buffer[double_buffer_index++] = c;
                }
            } else {
                uint_buffer = uint_buffer * 10 + (c - '0');
            }
        } else {
            if (isBufferFilled) {
                if (mode == NUM_TYPE_DOUBLE) {
                    double_buffer[double_buffer_index] = '\0';
                    char *endptr;
                    double res = strtod(double_buffer, &endptr);

                    token.token_type =          TOKEN_TYPE_NUM;
                    token.data.numeric.type =   NUM_TYPE_DOUBLE;
                    token.data.numeric.value.double_value = res;

                    double_buffer_index = 0;
                } else { 
                    token.token_type =          TOKEN_TYPE_NUM;
                    token.data.numeric.type =   NUM_TYPE_UINT;
                    token.data.numeric.value.uint_value = uint_buffer;
                    uint_buffer = 0;
                }
                isBufferFilled = false;
                tokens_vec_push_back(result, token);
            }
        }

        if (isOperation(c)) {
            Token token_op = {
                .token_type = TOKEN_TYPE_OPERATION,
                .data = get_operation_from_char(c),
            };
            tokens_vec_push_back(result, token_op);
        } else if (isVariable(c)) {
            Token token_var = {
                .token_type = TOKEN_TYPE_VAR,
                .data = c,
            };
            tokens_vec_push_back(result, token_var);
        }
    }

    if (isBufferFilled) {
        if (mode == NUM_TYPE_DOUBLE) {
            double_buffer[double_buffer_index] = '\0';
            char *endptr;
            double res = strtod(double_buffer, &endptr);

            token.token_type =          TOKEN_TYPE_NUM;
            token.data.numeric.type =   NUM_TYPE_DOUBLE;
            token.data.numeric.value.double_value = res;
        } else { 
            token.token_type =          TOKEN_TYPE_NUM;
            token.data.numeric.type =   NUM_TYPE_UINT;
            token.data.numeric.value.uint_value = uint_buffer;
        }
        tokens_vec_push_back(result, token);
    }

    return result;
}

int16_t get_operation_priority(TokenOperator operation) {
    switch (operation) {
        case TOKEN_OPERATOR_PLUS:
        case TOKEN_OPERATOR_MINUS:
            return 2;
        case TOKEN_OPERATOR_MULT:
        case TOKEN_OPERATOR_DIV:
            return 3;
        case TOKEN_OPERATOR_POW:
            return 4;
        case TOKEN_OPERATOR_UNARY_MINUS:
            return 5;
    }
    return 0; 
}

/**
 * @brief
 * @param   operation
 * @return  true if right else false
 **/
TokenAssociativity get_operation_associativity(TokenOperator operation) {
    switch (operation) {
        case TOKEN_OPERATOR_POW:
        case TOKEN_OPERATOR_UNARY_MINUS:
            return TOKEN_ASSOCIATIVITY_RIGHT;
    }
    return TOKEN_ASSOCIATIVITY_LEFT; 
}

/**
 * @brief   Compares the priorities of the operation
 * @param   current operation
 * @param   other operation
 * @return  int16_t 
 **/
bool cmp_operations(TokenOperator current, TokenOperator other) {
    if (get_operation_associativity(current) == TOKEN_ASSOCIATIVITY_LEFT) {
        return get_operation_priority(current) <= get_operation_priority(other);
    }
    return get_operation_priority(current) < get_operation_priority(other);
}
