#include "socket_tools/socket_tools.h"
#include "server_config.h"
#include <stdio.h>
#include <string.h>
unsigned char pic_data[PIC_W*PIC_H * 2];


int main(void){
    unsigned char pic_ask_code;
    unsigned char pic_info[2]={/*W*/PIC_W,/*H*/PIC_H};
    // for (int i = 0; i < PIC_W * PIC_H; i++) {
    //     pic_data[i * 2]     = 0x00;
    //     pic_data[i * 2 + 1] = 0xf8;
    // }

    socket_t s = socket_init(SOCKET_TCP, NULL, 8080);
    if(s.fd < 0){
        return 1;
    }
    if(!socket_read(&s, &pic_ask_code, 1)){
        printf("error\n");
        socket_close(&s);
        return 1;
    }

    switch(pic_ask_code){
        case 0x80:
            if(socket_send(&s, pic_info, sizeof(pic_info))){
                printf("send info successfully\n");
                uint8_t i = 0;
                while(1){
                    if(capture_screen_to_pic_data() == 0)
                    {
                        socket_send(&s, pic_data, sizeof(pic_data));
                    }
                }
            }
        break;
        case 0x00:
            printf("test\n");
        break;
    }
    
    socket_close(&s);
    return 0;
}