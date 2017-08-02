

#ifndef COMMON_H
#define COMMON_H

#include "stm8s.h"
#include "main.h"

/*----------------------------------------------------------------------------*/
//UART1发送接收转换宏定义
#define  UART_485TX   GPIO_WriteHigh(GPIOD, GPIO_PIN_7)    //发送切换宏定义
#define  UART_485RX   GPIO_WriteLow(GPIOD, GPIO_PIN_7)     //接收切换宏定义


#define  DEVICE_TYPE   0x000C //仪表类型
#define  DEVICE_ADDR   0x01   //设备地址
#define  DATA_DOT      0x1000 //静态小数点位
#define  NONE_DOT      0xFFFF //动态小数点位（未用）

#define  TIME_RECEIVE  200    //200ms
#define  TIME_DEAL     200    //200ms
#define  TIME_SEND     200    //200ms
#define  TIME_DELAY    4      //4ms切换


typedef struct
{
    u8 Buff[64];      //接收缓冲、发送缓冲
    u8 Len;           //接收数据域长度、发送数据域长度
    u8 Step;          //接收步骤
    u8 Flag;          //通信标志
    u8 TimMs_Over;    //超时计时
    u8 TimMs_Delay;   //切换延时计时
}
S_CommData;

typedef union
{
    struct
    {
        u8 Bit1:2;
        u8 Bit2:2;
        u8 Bit3:2;
        u8 Bit4:2;
    };
    u8 OneByte;
}
U_CommDot;

extern S_CommData Com;


extern void UartDeal(void);


#endif

