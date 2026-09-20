#ifndef CM_TERMINAL_H_
#define CM_TERMINAL_H_

#include <unistd.h>

#define MAX_LINES 30

typedef struct {
    char text[128];
    uint32_t color;
} log_line_t;

void terminal_clear_all(void);
void terminal_str(const char *text, uint32_t color);
void terminal_log(const char *text, uint32_t color);
void terminal_init(unsigned int color);

#endif
