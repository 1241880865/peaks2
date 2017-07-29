


#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "main.h"

#define WR_485 	GPIO_SetBits(GPIOA, GPIO_Pin_12)  //485����״̬
#define RD_485 	GPIO_ResetBits(GPIOA, GPIO_Pin_12)//485����״̬

#define DATA_CODE_READ    0x03    //�����ݹ�����
#define DATA_CODE_SWRITE  0x06    //д�������ݹ�����

#define BROADCAST_ADDR    0xAA    //ͨѶ�㲥��ַ��ȫ�����գ��޷��أ�

extern u8 Flag_RcvStep;//��������ִ�в���
extern u8 RcvCount;    //��������
extern u8 RcvLength;   //���ݳ���
extern u8 RcvBuff[32]; //��������
//ʱ�����
extern u8 TimMs_ModbusWait; //Modbus-RTUͷ���
extern u8 TimMs_ModbusClear;//Modbus-Clear

u8   UartTest(void); //����������֤
void ModbusClear(void);
void RdData(UART_PARA *ppara);  //03
u16  WrDataSgl(UART_PARA *ppara, RANGE_STRUCT *range); //06

#endif
