#ifndef SOCKET_TOOLS_H_
#define SOCKET_TOOLS_H_

#include <arpa/inet.h>
#include <sys/socket.h>

//#define USE_CM_LOG
#define USE_SYS_LOG

#ifdef USE_SYS_LOG
    #include <stdarg.h>
    void log(const char* s, ...);
#endif

#define SOCKET_CONNECT_RETRY_CNT 10
#define WIFI_DATA_NUM 1400

typedef enum{
    SOCKET_UDP,
    SOCKET_TCP
}socket_type_t;

typedef struct{
    int fd;
    unsigned char init_flag;
    struct sockaddr_in skt;
    socket_type_t type;
}socket_t;

socket_t socket_init(socket_type_t type,const char* ip, int port);
char socket_read(socket_t* s, unsigned char* buf, unsigned int len);
char socket_send(socket_t* s, unsigned char* buf, unsigned int len);
void socket_close(socket_t* s);

#endif
