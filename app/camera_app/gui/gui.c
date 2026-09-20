#include "gui.h"
#include "cm_terminal.h"
#include <nuttx/config.h>
#include <unistd.h>
#include <lvgl/lvgl.h>
#include <stdio.h>
#include <string.h>

lv_nuttx_dsc_t info;
lv_nuttx_result_t result;
lv_image_dsc_t img_dsc;

void GUI_Init(void){
    lv_init();
    lv_nuttx_dsc_init(&info);
    info.fb_path = "/dev/lcd0";
    info.input_path = NULL;
    
    lv_nuttx_init(&info, &result);
    lv_display_set_color_format(result.disp, LV_COLOR_FORMAT_RGB565);
    
    img_dsc.header.magic  = LV_IMAGE_HEADER_MAGIC;
    img_dsc.header.cf     = LV_COLOR_FORMAT_RGB565;
    img_dsc.header.w      = 240;
    img_dsc.header.h      = 240;
    img_dsc.header.stride = 240 * 2;
    img_dsc.data_size     = 240 * 240 * 2;

    /*
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    */

    GUI_Clear(0x000000);
    GUI_Log("[INFO] System Start", 0x00ff00);
    GUI_Log("Waiting WIFI Connection...", 0xffffff);
    GUI_Update();
}


void GUI_Update(void){
    uint32_t idle = lv_timer_handler();
    //usleep(1000);
}

void GUI_Destroy(void){
    lv_nuttx_deinit(&result);
    lv_deinit();
}

void GUI_ClearAllText(void){
    terminal_clear_all();
}

void GUI_SetText(const char* text, int x, int y, unsigned int color){
    lv_obj_t * scr = lv_scr_act();
    lv_obj_t* label = lv_label_create(scr);
    //lv_obj_remove_style_all(terminal_container);
    lv_label_set_text(label, text);
    lv_obj_align(label, LV_ALIGN_CENTER, x, y);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_update_layout(scr);
    lv_obj_set_style_transform_pivot_x(label, lv_obj_get_width(label) / 2, 0);
    lv_obj_set_style_transform_pivot_y(label, lv_obj_get_height(label) / 2, 0);
    #ifdef CM_LVGL_VERTICAL_REFLECTION
        lv_obj_set_style_transform_rotation(label, 1800, 0);
    #endif
}

void GUI_Log(const char* text, unsigned int color){
    terminal_log(text, color);
}

void GUI_Clear(unsigned int color){
    lv_obj_clean(lv_screen_active());
    terminal_init(color);
    /*lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);*/
}

void GUI_SetRGB565(const char* pic[]){
    static uint8_t i = 0;
    static lv_obj_t *img;
    if(i == 0){
        img_dsc.data = *pic;
        img = lv_image_create(lv_scr_act());
       
        i = 1;
    }

    lv_image_cache_drop(&img_dsc);
    #ifdef CM_LVGL_VERTICAL_REFLECTION
        lv_image_set_rotation(img, 1800);
    #endif
    //lv_obj_set_style_transform_rotation(img, 1800, 0);
    lv_image_set_src(img, &img_dsc);
}
