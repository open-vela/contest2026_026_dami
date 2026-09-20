#include "cm_terminal.h"
#include "../cm_app_config.h"
#include <nuttx/config.h>
#include <lvgl/lvgl.h>
#include <stdio.h>
#include <string.h>

static lv_obj_t *terminal_container = NULL;
static int line_count = 0;

static log_line_t log_lines[MAX_LINES];

void terminal_clear_all(void){
    line_count = 0;
}

void terminal_str(const char *text, uint32_t color){
     if (!terminal_container) return;
    char clean_text[256];
    strncpy(clean_text, text, sizeof(clean_text) - 1);
    clean_text[sizeof(clean_text) - 1] = '\0';
    
    size_t len = strlen(clean_text);
    if (len > 0 && clean_text[len - 1] == '\n') {
        clean_text[len - 1] = '\0';
    }
    if (len > 1 && clean_text[len - 2] == '\r') {
        clean_text[len - 2] = '\0';
    }
    
    if (line_count < MAX_LINES) {
        strncpy(log_lines[line_count].text, clean_text, sizeof(log_lines[line_count].text) - 1);
        log_lines[line_count].text[sizeof(log_lines[line_count].text) - 1] = '\0';
        log_lines[line_count].color = color;
        line_count++;
    } else {
        for (int i = 1; i < MAX_LINES; i++) {
            strcpy(log_lines[i - 1].text, log_lines[i].text);
            log_lines[i - 1].color = log_lines[i].color;
        }
        strncpy(log_lines[MAX_LINES - 1].text, clean_text, sizeof(log_lines[MAX_LINES - 1].text) - 1);
        log_lines[MAX_LINES - 1].text[sizeof(log_lines[MAX_LINES - 1].text) - 1] = '\0';
        log_lines[MAX_LINES - 1].color = color;
    }
}
void terminal_log(const char *text, uint32_t color) {
    if (!terminal_container) return;
    terminal_str(text, color);
    lv_obj_clean(terminal_container);
    
    for (int i = 0; i < line_count; i++) {
        lv_obj_t *line = lv_label_create(terminal_container);
        lv_label_set_text(line, log_lines[i].text);
        lv_obj_set_style_text_color(line, lv_color_hex(log_lines[i].color), 0);
        lv_obj_set_style_text_font(line, &lv_font_montserrat_14, 0);
        lv_obj_set_width(line, lv_obj_get_width(terminal_container) - 10);
        lv_label_set_long_mode(line, LV_LABEL_LONG_WRAP);
        
        if (i > 0) {
            lv_obj_align_to(line, lv_obj_get_child(terminal_container, i - 1), 
                           LV_ALIGN_OUT_BOTTOM_LEFT, 0, 2);
        } else {
            lv_obj_align(line, LV_ALIGN_TOP_LEFT, 5, 5);
        }
    }
    
    lv_obj_scroll_to_y(terminal_container, LV_COORD_MAX, LV_ANIM_OFF);
    lv_obj_update_layout(terminal_container);
}

void terminal_init(unsigned int color){
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    terminal_container = lv_obj_create(scr);
    lv_obj_remove_style_all(terminal_container);

    lv_obj_set_size(terminal_container, CM_SCREEN_W, CM_SCREEN_H);
    lv_obj_align(terminal_container, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_set_style_transform_pivot_x(terminal_container, CM_SCREEN_W/2, 0);
    lv_obj_set_style_transform_pivot_y(terminal_container, CM_SCREEN_H/2, 0);
    #ifdef CM_LVGL_VERTICAL_REFLECTION
    //    lv_obj_set_style_transform_rotation(terminal_container, 1800, 0);
    #endif
    lv_obj_set_style_bg_color(terminal_container, lv_color_hex(color), 0);
    //lv_obj_set_style_border_width(terminal_container, 1, 0);
    //lv_obj_set_style_border_color(terminal_container, lv_color_hex(0x00ff00), 0);
    //lv_obj_set_style_pad_all(terminal_container, 5, 0);
    lv_obj_set_scrollbar_mode(terminal_container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(terminal_container, LV_DIR_VER);
    lv_obj_update_layout(terminal_container);
}
