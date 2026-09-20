通讯数据结构
图片请求
客户端 send 0x80 & recv pic_data
服务器 recv 0x80 & send pic_data

pic_data结构:
[PIC_INFO][PIC_DATA]
PIC_INFO为uint8_t数组
索引数    定义
i = 0    PIC_W
i = 1    PIC_H
最大只支持240*240屏幕
PIC_DATA为uint8_t数组
长度为PIC_W*PIC_H*2
数据结构为[R*5+G*3][G*3+B*5]
