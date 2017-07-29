

#ifndef MAIN_H
#define MAIN_H

#include "stm32f10x.h"

/* ���� */
#define LED1OFF()		GPIO_ResetBits(GPIOB, GPIO_Pin_5)
#define LED1ON()		GPIO_SetBits(GPIOB, GPIO_Pin_5) //��

/* ͨѶ */
#define LED2OFF()		GPIO_ResetBits(GPIOB, GPIO_Pin_4)
#define LED2ON()		GPIO_SetBits(GPIOB, GPIO_Pin_4) //��

/* �����̵��� */
#define ALM_OFF()		GPIO_SetBits(GPIOA, GPIO_Pin_15)
#define ALM_ON()		GPIO_ResetBits(GPIOA, GPIO_Pin_15)//��
/*------------------------------------------------------------------------------
********************************* �ṹ�� ���� **********************************
------------------------------------------------------------------------------*/
typedef struct
{
	u16 add;    //0x00-�ӻ���ַ
	u16 duty;   //0x01-Led�������,�ٷֱ�
	u16 status1;//0x02-����״̬��1��12��·����״̬��14��������״̬��15���±���״̬��
	u16 status2;//0x03-����״̬��13�����̵������״̬��15��·�ƣ�16���µơ�
	u16 order1; //0x04-�������1��12·ѡ��
	u16 order2; //0x05-�������1�ܿ������2ȫ�����3ȫ������
	u16 ntc1;   //0x06-�����¶�1
	u16 ntc2;   //0x07-�����¶�2
	u16 ntcAlm; //0x08-�¶ȱ����趨ֵ
	u16 almOd;  //0x09-�ܱ���״̬
	u16 trigMd; //0x0A-�㶯ģʽѡ��
	u16 factor; //0x0B-ռ�ձ�ϵ��(�޷�ֵ)
}
UART_PARA;
extern UART_PARA m_para;
#define PARA_NUM  (sizeof(UART_PARA) / 2) //�������������

//ͨѶ������Χ
typedef const struct
{
    u16 Min; //��������
    u16 Max; //��������
}
RANGE_STRUCT;

//ӳ��λ������״̬
typedef volatile struct
{
	u32 ledOpenI[12];//Led��·����״̬
	u32 NA1;		 //Ԥ��
	u32 switchOn;	 //��������״̬
	u32 ntcAlm;	     //���±���״̬
	u32 NA2;         //Ԥ��
	u32 NA3[12];     //Ԥ��
	u32 almOut;      //�����̵������״̬
	u32 NA4;         //Ԥ��
	u32 openLed;     //��·������
	u32 ntcLed;      //���±�����
}
RUNST;

//ӳ��λ����������
typedef volatile struct
{
	u32 onOff[12];//�ӻ���������
	u32 NA1[4];   //Ԥ��
	u32 devOnOff; //�ӻ���������
	u32 allOn;    //12·ȫ��
	u32 allOff;   //12·ȫ��
}
RUNOD;

extern RUNST* pm_st; //����״̬
extern RUNOD* pm_od; //��������
extern u8 gUartAlm;  //����ͨѶ����

//ʱ�����
extern u16 TimMs_uartErr; //ͨѶͣ������ʱ���ʱ

#endif


