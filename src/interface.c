#include "Interface.h"

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#include "Postfix.h"
#include "Tokens.h"
#include "OperationsTree.h"
#include "Task.h"


char token_buffer[64];
ProgramMode num_mode = MODE_INT64;
InterfaceMode interface_mode = INTERFACE_MAIN;


void enable_raw_mode(struct termios *orig) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, orig);
    raw = *orig;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode(const struct termios *orig) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig);
}

void terminal_clear() {
    printf("\033[H\033[J");
}

void printf_info() {
    printf(ANSI_COLOR_CYAN "Program mode <use arrorws>: " ANSI_COLOR_GREEN);
    switch (interface_mode) {
        case INTERFACE_MAIN:    printf("INTERFACE_MAIN" ANSI_COLOR_RESET); break;
        case INTERFACE_TREE:    printf("INTERFACE_TREE" ANSI_COLOR_RESET); break;
        case INTERFACE_TASK:    printf("INTERFACE_TASK" ANSI_COLOR_RESET); break;
        case INTERFACE_HELP:    printf("INTERFACE_HELP" ANSI_COLOR_RESET); break;
        default:                printf(ANSI_COLOR_RED "ERROR %d" ANSI_COLOR_RESET, interface_mode); break;
    }
    putchar('\n');
    printf(ANSI_COLOR_CYAN "Numeric mode <use arrorws>: " ANSI_COLOR_GREEN);
    switch (num_mode) {
        case MODE_INT64:    printf("MODE_INT64" ANSI_COLOR_RESET); break;
        case MODE_UINT64:   printf("MODE_UINT64" ANSI_COLOR_RESET); break;
        case MODE_DOUBLE:   printf("MODE_DOUBLE" ANSI_COLOR_RESET); break;
        default:            printf(ANSI_COLOR_RED "ERROR %d" ANSI_COLOR_RESET, num_mode); break;
    }
    putchar('\n');
}

void main_redraw(const char *input_buffer) {
    printf("> %s", input_buffer);
    printf("\n= length: %zu ", strlen(input_buffer));

    tokens_vec          *vec = tokenization(input_buffer, num_mode);
    tokens_vec          *postfix_vec = convert_to_postfix_form(vec);
    tokens_vec_iterator it = tokens_vec_iterator_begin(vec);
    
    printf("\n= tokens: ");
    while (tokens_vec_iterator_has_next(&it)) {
        str_token(token_buffer, *tokens_vec_iterator_next(&it));
        printf("%s ", token_buffer);
    }
    
    it = tokens_vec_iterator_begin(postfix_vec);
    printf("\n= postfix: ");
    while (tokens_vec_iterator_has_next(&it)) {
        str_token(token_buffer, *tokens_vec_iterator_next(&it));
        printf("%s ", token_buffer);
    }

    printf("\n= result: ");
    switch (num_mode) {
        case MODE_INT64:    printf("%ld\n", calc_int64_t_postfix_form(postfix_vec)); break;
        case MODE_UINT64:   printf("%lu\n", calc_uint64_t_postfix_form(postfix_vec)); break;
        case MODE_DOUBLE:   double res = calc_double_postfix_form(postfix_vec); printf("%lf " "%.17g\n", res, res); break;
    }

    tokens_vec_destroy(vec);
    tokens_vec_destroy(postfix_vec);
    fflush(stdout);
}

void tree_redraw(const char *input_buffer) {
    tokens_vec *vec =           tokenization(input_buffer, num_mode);
    tokens_vec *postfix_vec =   convert_to_postfix_form(vec);
    token_tree *tree =          create_token_tree(postfix_vec);
    if (tree) {
        print_token_tree(tree);
        token_tree_destroy(tree);
    }
    tokens_vec_destroy(vec);
    tokens_vec_destroy(postfix_vec);
}

void help_redraw() {
    printf(
        ANSI_COLOR_GREEN "----------CONTROL----------\n" ANSI_COLOR_RESET
        "> UP < - Next numeric mode\n"
        "> DOWN < - Prev numeric mode\n"
        "> RIGHT < - Next program mode\n"
        "> LEFT < - Prev program mode\n"
        "> Ctrl + C < - Exit\n"
        "> Backspace < - Clear sign\n"
        "> Ctrl + Backspace < - Clear word\n"
        ANSI_COLOR_RED "----------ERRORS----------\n" ANSI_COLOR_RESET
        "> ZD < - Zero division Error\n"
        "> OE < - Operator Error\n"
        "> UE < - Unary Operator Error\n"
        "> BE < - Binart Operator Error\n"
        ANSI_COLOR_YELLOW "----------WARNS----------\n" ANSI_COLOR_RESET
        "> UT < - Unexpected token\n"
        "> RW < - Result Warn\n"
    );
}

void task_redraw(const char *input_buffer) {
    tokens_vec *vec =           tokenization(input_buffer, num_mode);
    tokens_vec *postfix_vec =   convert_to_postfix_form(vec);
    token_tree *tree =          create_token_tree(postfix_vec);
    if (tree) {
        remove_the_multipliers(tree);
        print_token_tree(tree);
        token_tree_destroy(tree);
    }
    tokens_vec_destroy(vec);
    tokens_vec_destroy(postfix_vec);
}

void frame(const char *input_buffer) {
    terminal_clear();
    printf_info();
    switch (interface_mode) {
        case INTERFACE_MAIN: main_redraw(input_buffer); break;
        case INTERFACE_TREE: tree_redraw(input_buffer); break;
        case INTERFACE_TASK: task_redraw(input_buffer); break;
        case INTERFACE_HELP: help_redraw();             break;
    }
    fflush(stdout);
}

void input_wait() {
    struct termios orig_term;
    enable_raw_mode(&orig_term);

    char    input_buffer[MAX_INPUT] = {0};
    size_t  len = 0;

    frame(input_buffer);

    while (1) {
        char c;
        if (read(STDIN_FILENO, &c, 1) != 1) break;

        if (c == 3) {  // Ctrl-C
            break;
        } else if (c == 27) {  // Escape-последовательность
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) != 1) break;
            if (read(STDIN_FILENO, &seq[1], 1) != 1) break;

            if (seq[0] == '[') {
                if (seq[1] == 'A') {  // Стрелка вверх
                    num_mode = (++num_mode) % 3;
                } else if (seq[1] == 'B') { // Стрелка вниз
                    num_mode = (num_mode + 2) % 3;
                } else if (seq[1] == 'C') {
                    interface_mode = (++interface_mode) % 4;
                } else if (seq[1] == 'D') {
                    interface_mode = (interface_mode + 3) % 4;
                }
            }
        } 
        
        if (interface_mode == INTERFACE_MAIN) {
            if (c == 127 || c == '\b') {  // Backspace
                if (len > 0) {
                    input_buffer[--len] = '\0';
                }
            } else if (c == 8 || c == 23) {  // Ctrl+Backspace
                while (len > 0 && input_buffer[len-1] == ' ') {
                    input_buffer[--len] = '\0';
                }
                while (len > 0 && input_buffer[len-1] != ' ') {
                    input_buffer[--len] = '\0';
                }
            } else if (c >= 32 && c < 127) {
                if (len < MAX_INPUT - 1) {
                    input_buffer[len++] = c;
                    input_buffer[len] = '\0';
                }
            }
        }
        
        frame(input_buffer);
    }

    disable_raw_mode(&orig_term);
}