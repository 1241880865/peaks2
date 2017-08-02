


#ifndef __HEATCTRL_H
#define __HEATCTRL_H

#include "adSample.h" //采样
/*------------------------------------------------------------------------------
*************************************宏定义*************************************
------------------------------------------------------------------------------*/
#define HEAT_ON      GPIO_WriteLow(GPIOC, GPIO_PIN_2) //加热开宏定义
#define HEAT_OFF     GPIO_WriteHigh(GPIOC, GPIO_PIN_2)//加热关宏定义
#define ALM_ON       GPIO_WriteLow(GPIOC, GPIO_PIN_3) //报警开宏定义
#define ALM_OFF      GPIO_WriteHigh(GPIOC, GPIO_PIN_3)//报警关宏定义

#define TEMP_MAX     4000 //产品控温上限(带小数)
#define TEMP_MIN     0    //产品控温下限
/*------------------------------------------------------------------------------
********************************* 结构体 声明 **********************************
------------------------------------------------------------------------------*/
typedef struct  //自整定 步骤
{
    u8  ATNOW:  1;  //正在自整定
    u8  ATN1:   1;
    u8  ATN2:   1;
    u8  ATN3:   1;
    
    u8  ATN4:   1;
    u8  ATJR:   1;  //自整定时加热输出
    u8  RES4:   2;
}
ATSTEP_STRUCT;

typedef struct  //PID温控真实运行参数
{
    s16 LastMeasure;//上一次温度测定值
    u16 TVal;       //PID计算周期
    s16 PkVal;      //比例值
    s32 ISum;       //积分累加值
    s16 IVal;       //积分值
    s16 DVal;       //微分值
    s16 DResult;    //经微分先行后微分值
    s16 Result;     //经pid计算输出
}
TempPID_SRTUCT;

//内部参数表一
typedef struct
{
    s16 alh;
    s16 Temp_t;
    s16 Temp_p;
    s16 Temp_i;
    s16 Temp_d;
    s16 pb;
    s16 pl;
    s16 adr;
}
ParaTone_STRUCT;

//内部参数表二
typedef struct
{
    s16 pon;
    s16 rut;
    s16 bl;
    s16 np;
    s16 spl;
    s16 sph;
}
ParaTtwo_STRUCT;

//温度标志位
typedef struct
{
    u8 EnTempRun; //温度运行允许
    u8 EnAT;      //自整定允许
    
    u8 TempAlm;   //超温报警标志位
    u8 TempOver;  //温度溢出标志位
    
    u8 HeatDis;   //温度输出显示标志位
    u8 ATDis;     //自整定显示
    u8 ManlAlm;   //手动超温
    u8 TAlmSpk;   //超温蜂鸣器鸣叫
}
TFlag_STRUCT;

/*******************************************************************************
                    ---------------- 对外 -----------------
*******************************************************************************/
//温度标志位
extern TFlag_STRUCT Tflag;

//温度运行参数
extern s16 TempSetVal;    //温度设定值
extern s16 TempDisplay;   //温度显示值

//内部参数表一
extern ParaTone_STRUCT ParaTempPID;

//内部参数表二
extern ParaTtwo_STRUCT ParaTempCtrl;

//时间变量
extern u16 TimS_HeatPeriod;  //加热周期计时
extern u16 TimMs_HeatCounter;//加热周期占空比计时
extern u8  TimS_EnAT;        //自整定时间计时
extern u8  TimS_SpeakFlash;  //蜂鸣器1s响一次
/*------------------------------------------------------------------------------
*********************************** 函数声明 ***********************************
------------------------------------------------------------------------------*/
//温度控制
void HeatControl(void); //加热控制主函数
void TempADToDisp(void);//ad采样到显示值
void PIDReset(void);    //未进行PID时保持初值（比例带外：无微分先行，且无积分累积）

//温度报警
void TempAlarm(void);

//自整定函数
void AT_pid(void);
void AT_ini(void);    //自整定初始化

#endif

