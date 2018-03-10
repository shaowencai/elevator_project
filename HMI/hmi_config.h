#ifndef HMI_CONFIG_H
#define HMI_CONFIG_H

#define CRC16_ENABLE 0         /*!< 如果需要CRC16校验功能，修改此宏为1(此时需要在VisualTFT工程中配CRC校验)*/
#define CMD_MAX_SIZE 20        /*!<单条指令大小，根据需要调整，尽量设置大一些*/
#define QUEUE_MAX_SIZE 80      /*!< 指令接收缓冲区大小，根据需要调整，尽量设置大一些*/


#define uchar   unsigned char
#define uint8   unsigned char
#define uint16  unsigned short int
#define uint32  unsigned long
#define int16   short int
#define int32   long
    

#endif

