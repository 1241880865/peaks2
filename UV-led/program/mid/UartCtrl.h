


#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "main.h"

#define WR_485 	GPIO_SetBits(GPIOA, GPIO_Pin_12)  //485发送状态
#define RD_485 	GPIO_ResetBits(GPIOA, GPIO_Pin_12)//485接收状态

#define DATA_CODE_READ    0x03    //读数据功能码
#define DATA_CODE_SWRITE  0x06    //写单个数据功能码

#define BROADCAST_ADDR    0xAA    //通讯广播地址（全部接收，无返回）

extern u8 Flag_RcvStep;//接收数据执行步骤
extern u8 RcvCount;    //数据数量
extern u8 RcvLength;   //数据长度
extern u8 RcvBuff[32]; //接收数据
//时间变量
extern u8 TimMs_ModbusWait; //Modbus-RTU头检测
extern u8 TimMs_ModbusClear;//Modbus-Clear

u8   UartTest(void); //接收数据验证
void ModbusClear(void);
void RdData(UART_PARA *ppara);  //03
u16  WrDataSgl(UART_PARA *ppara, RANGE_STRUCT *range); //06

#endif
