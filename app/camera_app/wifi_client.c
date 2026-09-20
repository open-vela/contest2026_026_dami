#include "wifi_client.h"
#include "cm_app_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <jpeglib.h>

#include "log.h"
#include "gui/gui.h"

#include "socket_tools/socket_tools.h"

static void wifi_connect(const char* wifi_name,const char* wifi_password){
    char w_n[20];
    sprintf(w_n,"wapi psk wlan0 %s 3 2",wifi_password);
    system(w_n);
    sprintf(w_n,"wapi essid wlan0 %s 1",wifi_name);
    system(w_n);
    system("renew wlan0");
}

static char Wifi_Init(const char* wifi_name, const char* wifi_password)
{
    struct in_addr addr = {0};
    if (netlib_get_ipv4addr("wlan0", &addr) < 0 || addr.s_addr == 0) {
        wifi_connect(wifi_name, wifi_password);
        netlib_get_ipv4addr("wlan0", &addr);
        if (netlib_get_ipv4addr("wlan0", &addr) < 0 || addr.s_addr == 0){
            log("WIFI ERR\n");
            return 0;
        }
    }

    log("%s", inet_ntoa(addr));
    printf("%s\n", inet_ntoa(addr));

    return 1;
    
}

char Wifi_Client_Init(const char* wifi_name, const char* wifi_password, const char* ip)
{
    if(!Wifi_Init(wifi_name, wifi_password)){
        return 0;
    }

    log("wifi connected");
    log("creating socket link");
    
    socket_t s = socket_init(SOCKET_TCP, ip, 8080);
    if(!s.init_flag){
        return 0;
    }

    unsigned char pic_ask = 0x80;
    unsigned char pic_info[2];
    unsigned char* pic_data;

    if(!socket_send(&s, (unsigned char*)&pic_ask, 1)){
        log("SEND ASKCODE ERR");
        socket_close(&s);
        return 0;
    }

    log("it has sent askcode");
    
    if(!socket_read(&s, (unsigned char*)&pic_info, 2)){
        log("READ PICINFO ERR");
        socket_close(&s);
        return 0;
    }

    log("recv ok");
    log("picW: %d",pic_info[0]);
    log("picH: %d",pic_info[1]);
    
    pic_data = (unsigned char*)malloc(pic_info[0] * pic_info[1] * 2);

    while(1){
        if(socket_read(&s, (unsigned char*)pic_data,pic_info[0]*pic_info[1] * 2)){
            
        }
        GUI_SetRGB565(&pic_data);
        GUI_Update();
    }

    socket_close(&s);
    return 1;
}
