#include "cm_app_config.h"
#include "gui/gui.h"
#include "wifi_client.h"
#include "log.h"
#include <string.h>
#include <fcntl.h>
#include "lvgl.h"

int main(int argc, char *argv[]) {
    #ifdef CM_GUI
    GUI_Init();
    #endif

    #ifdef CM_WIFI
    if(!Wifi_Client_Init(argv[1], argv[2], argv[3])){
        #ifdef CM_GUI
        GUI_Clear(0xff0000);
        GUI_ClearAllText();
        GUI_SetText("WIFI ERR", 0, 0, 0xffffff);
        #endif

        while(1){
        #ifdef CM_GUI
            GUI_Update();
            usleep(1000);
        #endif
        }
    }
    #endif

    #ifdef CM_GUI
    GUI_Clear(0xff0000);
    log("camera app start\n");
    #endif

    #ifdef CM_WIFI
    char s[25];
    #endif

    while(1){
        #ifdef CM_GUI
        GUI_Update();
        #endif

        #ifdef CM_WIFI
        int fd = open("/dev/ttyACM0", O_RDONLY | O_NONBLOCK);
        read(fd, s, sizeof(s));
        if(strstr(s,"exit") != NULL){
            log("exit\n");
            close(fd);
            break;
        }
        close(fd);
        #endif
    }

    #ifdef CM_GUI
    GUI_Destroy();
    #endif

    return 0;
}
