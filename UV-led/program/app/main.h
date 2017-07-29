

#ifndef MAIN_H
#define MAIN_H

#include "stm32f10x.h"

/* 报警 */
#define LED1OFF()		GPIO_ResetBits(GPIOB, GPIO_Pin_5)
#define LED1ON()		GPIO_SetBits(GPIOB, GPIO_Pin_5) //开

/* 通讯 */
#define LED2OFF()		GPIO_ResetBits(GPIOB, GPIO_Pin_4)
#define LED2ON()		GPIO_SetBits(GPIOB, GPIO_Pin_4) //开

/* 报警继电器 */
#define ALM_OFF()		GPIO_SetBits(GPIOA, GPIO_Pin_15)
#define ALM_ON()		GPIO_ResetBits(GPIOA, GPIO_Pin_15)//开
/*------------------------------------------------------------------------------
********************************* 结构体 定义 **********************************
------------------------------------------------------------------------------*/
typedef struct
{
	u16 add;    //0x00-从机地址
	u16 duty;   //0x01-Led输出功率,百分比
	u16 status1;//0x02-运行状态，1～12断路报警状态，14开关输入状态，15超温报警状态。
	u16 status2;//0x03-运行状态，13报警继电器输出状态，15断路灯，16超温灯。
	u16 order1; //0x04-运行命令，1～12路选择。
	u16 order2; //0x05-运行命令，1总开关命令，2全开命令，3全关命令
	u16 ntc1;   //0x06-测量温度1
	u16 ntc2;   //0x07-测量温度2
	u16 ntcAlm; //0x08-温度报警设定值
	u16 almOd;  //0x09-总报警状态
	u16 trigMd; //0x0A-点动模式选择
	u16 factor; //0x0B-占空比系数(限幅值)
}
UART_PARA;
extern UART_PARA m_para;
#define PARA_NUM  (sizeof(UART_PARA) / 2) //参数表变量个数

//通讯参数范围
typedef const struct
{
    u16 Min; //参数下限
    u16 Max; //参数上限
}
RANGE_STRUCT;

//映射位段运行状态
typedef volatile struct
{
	u32 ledOpenI[12];//Led断路故障状态
	u32 NA1;		 //预留
	u32 switchOn;	 //开关输入状态
	u32 ntcAlm;	     //超温报警状态
	u32 NA2;         //预留
	u32 NA3[12];     //预留
	u32 almOut;      //报警继电器输出状态
	u32 NA4;         //预留
	u32 openLed;     //断路报警灯
	u32 ntcLed;      //超温报警灯
}
RUNST;

//映射位段运行命令
typedef volatile struct
{
	u32 onOff[12];//从机开关命令
	u32 NA1[4];   //预留
	u32 devOnOff; //从机开关命令
	u32 allOn;    //12路全开
	u32 allOff;   //12路全关
}
RUNOD;

extern RUNST* pm_st; //运行状态
extern RUNOD* pm_od; //控制命令
extern u8 gUartAlm;  //有无通讯报警

//时间变量
extern u16 TimMs_uartErr; //通讯停机保护时间计时

#endif


