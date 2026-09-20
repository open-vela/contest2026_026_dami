#include "socket_tools.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#ifdef USE_CM_LOG
#include "../log.h"
#endif

#ifdef USE_SYS_LOG
void log(const char* format, ...){
    char out[20];
    va_list args;
    va_start(args, format);
    vsprintf(out, format, args);
    printf("[SYS]: %s",out);
    va_end(args);
}
#endif

socket_t socket_init(socket_type_t type, const char* ip, int port){
    socket_t s={.fd = -1};
    switch(type){
        case SOCKET_UDP:
            s.fd = socket(AF_INET, SOCK_DGRAM, 0);
            break;
        case SOCKET_TCP:
            s.fd = socket(AF_INET, SOCK_STREAM, 0);
            break;
    }
    if(s.fd < 0){
        perror("socket");
        return s;
    }
    s.skt.sin_family = AF_INET;
    s.skt.sin_port = htons(port);
    if(ip == NULL){
        s.skt.sin_addr.s_addr = htonl(INADDR_ANY);
        int reuse = 1;
        if(setsockopt(s.fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0){
            perror("setsockopt");
            close(s.fd);
            s.fd = -1;
            return s;
        }
        if(bind(s.fd, (struct sockaddr*)&s.skt, sizeof(s.skt)) < 0){
            perror("bind");
            close(s.fd);
            s.fd = -1;
            return s;
        }
        if(type == SOCKET_TCP){
            int ls_fd = listen(s.fd, 1);
            if(ls_fd < 0){
                perror("listen");
                close(s.fd);
                s.fd = -1;
                return s;
            }

            int conn_fd = accept(s.fd, NULL, NULL);
            if(conn_fd < 0){
                perror("accept");
                close(s.fd);
                s.fd = -1;
                return s;
            }
            close(s.fd);
            s.fd = conn_fd;
        }
    }else{
        s.skt.sin_addr.s_addr = inet_addr(ip);
        if(type == SOCKET_TCP){
            int cnfd = connect(s.fd, (struct sockaddr*)&s.skt, sizeof(s.skt));
            int retry = SOCKET_CONNECT_RETRY_CNT;
            if (cnfd == -1) {
                while(retry--){
                    close(s.fd);
                    s.fd = socket(AF_INET, SOCK_STREAM, 0);
                    cnfd = connect(s.fd, (struct sockaddr*)&s.skt, sizeof(s.skt));
                    if(cnfd != -1) break;
                    log("retry %d seconds...",SOCKET_CONNECT_RETRY_CNT-retry);
                    sleep(1);
                }
                if(cnfd == -1){
                    s.init_flag = 0;
                    log("connect failed: errno=%d", errno);
                    log("please reset your board and check the server is running");
                    close(s.fd);
                    return s;
                }else{
                    log("connect successfully!");
                }
            }
        }
    }
    s.init_flag = 1;
    s.type = type;
    return s;
}

char socket_read(socket_t* s, unsigned char* buf, unsigned int len){
    unsigned int r_i = 0;
    unsigned int read_total_n = len;
    int r_r = 0;
    unsigned int read_buf_size = 0;

    while(r_i < read_total_n){
        if((read_total_n - r_i) > WIFI_DATA_NUM){
            read_buf_size = WIFI_DATA_NUM;
        }else{
            read_buf_size = read_total_n - r_i;
        }
        if(s->type == SOCKET_TCP){
            r_r = recv(s->fd, buf + r_i, read_buf_size, 0);
        }else{
            r_r = recvfrom(s->fd, buf + r_i, read_buf_size, 0, NULL, 0);
        }
        if(r_r == 0) break;
        if(r_r < 0){
            perror("recv");
            printf("recv error\n");
            return 0;
        }
        r_i += r_r;
    }

    return 1;
}

char socket_send(socket_t* s, unsigned char* buf, unsigned int len){
    unsigned int s_i = 0;
    unsigned int send_total_n = len;
    int s_r = 0;
    unsigned int send_buf_size = 0;

    while(s_i < send_total_n){
        if((send_total_n - s_i) > WIFI_DATA_NUM){
            send_buf_size = WIFI_DATA_NUM;
        }else{
            send_buf_size = send_total_n - s_i;
        }
        if(s->type == SOCKET_TCP){
            s_r = send(s->fd, buf + s_i, send_buf_size, 0);
        }else{
            s_r = sendto(s->fd, buf + s_i, send_buf_size, 0, (struct sockaddr*)&s->skt, sizeof(s->skt));
        }
        if(s_r < 0){
            perror("send");
            printf("send error\n");
            return 0;
        }
        s_i += s_r;
    }

    return 1;
}

void socket_close(socket_t* s){
    if (s == NULL || s->fd < 0) {
        return;
    }
    close(s->fd);
    s->fd = -1;
}