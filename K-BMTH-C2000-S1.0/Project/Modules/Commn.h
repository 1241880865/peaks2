

#ifndef COMMON_H
#define COMMON_H

#include "stm8s.h"
#include "main.h"

/*----------------------------------------------------------------------------*/
//UART1���ͽ���ת���궨��
#define  UART_485TX   GPIO_WriteHigh(GPIOD, GPIO_PIN_7)    //�����л��궨��
#define  UART_485RX   GPIO_WriteLow(GPIOD, GPIO_PIN_7)     //�����л��궨��


#define  DEVICE_TYPE   0x000C //�Ǳ�����
#define  DEVICE_ADDR   0x01   //�豸��ַ
#define  DATA_DOT      0x1000 //��̬С����λ
#define  NONE_DOT      0xFFFF //��̬С����λ��δ�ã�

#define  TIME_RECEIVE  200    //200ms
#define  TIME_DEAL     200    //200ms
#define  TIME_SEND     200    //200ms
#define  TIME_DELAY    4      //4ms�л�


typedef struct
{
    u8 Buff[64];      //���ջ��塢���ͻ���
    u8 Len;           //���������򳤶ȡ����������򳤶�
    u8 Step;          //���ղ���
    u8 Flag;          //ͨ�ű�־
    u8 TimMs_Over;    //��ʱ��ʱ
    u8 TimMs_Delay;   //�л���ʱ��ʱ
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

