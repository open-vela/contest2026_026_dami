#ifndef EXEC_CMD_H
#define EXEC_CMD_H

#include "cmd.h"
#include <string.h>
#include <ctype.h>

uint8_t cmd_mode = 0;
uint32_t color = 0x000000;
#define COLOR_INFO 0x00ff00

typedef struct {
    const char *name;
    uint8_t cmd_id;
} cmd_entry_t;

static const cmd_entry_t cmd_table[] = {
    {"update",  CMD_UPDATE_MODE},
    {"red",     CMD_RED},
    {"clear",   CMD_CLEAR_ALL},
    {"time",    CMD_TIME},
    {"help",    CMD_HELP},
};

#define CMD_COUNT (sizeof(cmd_table) / sizeof(cmd_table[0]))

static void strip_newline(char *str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

uint8_t if_cmd(const char *c) {
    if (!c || c[0] == '\0') return 0;
    
    for (size_t i = 0; c[i] != '\0'; i++) {
        if (!isspace(c[i])) {
            return 1;
        }
    }
    return 0;
}

uint8_t cmd_to_type(const char *c) {
    if (!c) return CMD_UNKNOWN;
    
    char cmd_copy[32];
    strncpy(cmd_copy, c, sizeof(cmd_copy) - 1);
    cmd_copy[sizeof(cmd_copy) - 1] = '\0';
    
    strip_newline(cmd_copy);
    
    char *start = cmd_copy;
    while (isspace(*start)) start++;
    
    char *end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) {
        *end = '\0';
        end--;
    }
    
    for (size_t i = 0; i < CMD_COUNT; i++) {
        if (strcmp(start, cmd_table[i].name) == 0) {
            return cmd_table[i].cmd_id;
        }
    }
    
    return CMD_UNKNOWN;
}

void exec_cmd(const char *c) {
    if (!if_cmd(c)) return;
    
    uint8_t cmd_id = cmd_to_type(c);
    
    switch (cmd_id) {
        case CMD_UPDATE_MODE:
            cmd_mode = (cmd_mode == 0) ? 1 : 0;
            if(cmd_mode){
                lv_obj_clean(lv_scr_act());
                Cm_draw();
            }
            terminal_log("[CMD] Update mode changed", COLOR_INFO);
            break;
            
        case CMD_RED:
            color = 0xff0000;
            terminal_log("[CMD] Color set to RED", COLOR_INFO);
            break;
            
        case CMD_CLEAR_ALL:
            lv_obj_clean(lv_scr_act());
            Cm_draw();
            terminal_log("[CMD] Screen cleared", COLOR_INFO);
            break;
            
        case CMD_TIME:
            cmd_mode = 0;
            break;
            
        case CMD_HELP:
            terminal_log("[CMD] Available: update, red, clear, time, help", COLOR_INFO);
            break;
            
        /*default:
            terminal_log("[CMD] Unknown command", COLOR_INFO);
            break;*/
    }
}

#endif
