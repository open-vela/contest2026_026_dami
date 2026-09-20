#include "log.h"
#include "cm_app_config.h"
#include "gui/gui.h"
#include <stdio.h>
#include <stdarg.h>

void log(const char* format, ...){
    char out[100];
    va_list args;
    va_start(args, format);
    vsprintf(out, format, args);
    #ifdef CM_LOG_UART_OUT
    printf("[%s] %s\n", LOG_N, out);
    #endif
    #ifdef CM_GUI
    gui_log(out);
    #endif
    va_end(args);
}

void gui_log(const char* format){
    char tmp[50];
    sprintf(tmp, "[%s] %s", LOG_N, format);
    GUI_Log(tmp,0xf2ff00);
    GUI_Update();
}