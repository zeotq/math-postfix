#ifndef CALC_CONSOLE_INTERFACE_H
#define CALC_CONSOLE_INTERFACE_H

#define MAX_INPUT 256

#include <termios.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef enum {
    MODE_INT64,
    MODE_UINT64,
    MODE_DOUBLE,
} ProgramMode;

typedef enum {
    INTERFACE_MAIN,
    INTERFACE_TREE,
    INTERFACE_TASK,
    INTERFACE_HELP,
} InterfaceMode;

void enable_raw_mode(struct termios *orig);
void disable_raw_mode(const struct termios *orig);

void terminal_clear();
void input_wait();

#endif // CALC_CONSOLE_INTERFACE_H