


#ifndef __MOTORDRIVER_H
#define __MOTORDRIVER_H

/*------------------------------------------------------------------------------
*************************************宏定义*************************************
------------------------------------------------------------------------------*/
/* PID计算参数宏定义 */
#define SPEED_KP_DIVISOR  128 //比例因子宏定义
#define SPEED_KI_DIVISOR  512 //积分因子宏定义
#define SPEED_KD_DIVISOR  16  //微分因子宏定义
#define SPEED_OUT_MIN     0   //PID输出最小值宏定义
#define SPEED_KISUM_MIN   0   //积分累加和最小值宏定义
/* 斜坡计算时间宏定义 */
#define RAMP_TIME         2   //斜坡计算，每一步间隔时间(ms)
/*----------------------------------------------------------------------------*/
#define DEAD_TIME 40 //PWM死区设置
#define FREQUENCY 16 //PWM载波频率
/*----------------------------------------------------------------------------*/
//传感器端口
#define HALL_SENSOR   GPIOD->IDR & 0x08 //TIM2_CH2

/*------------------------------------------------------------------------------
*********************************** 变量声明 ***********************************
------------------------------------------------------------------------------*/
//电机控制状态枚举类型
typedef enum
{
    //电机运行状态【IDLE/START/RUN/STOP/FAULT】
    IDLE  = 0,
    START = 1,
    RUN   = 2,
    STOP  = 3,
    FAULT = 4
} 
CONTROL_ENUM;

/*----------------------------------------------------------------------------*/
//斜坡函数计算结构体
typedef struct 
{
    s32 ActualVal;   //斜坡函数输出值
    s32 DesiredVal;  //斜坡函数期望值
    s32 StepForward; //斜坡函数加速步进值
    s32 StepBack;    //斜坡函数减速步进值
}
Ramp_STRUCT;

/*----------------------------------------------------------------------------*/
typedef struct
{
    s32 ISumVaule;   //PID积分和
    s32 ISumUpLimit; //PID积分和上限
    s32 ISumLowLimit;//PID积分和下限
    s32 OutUpLimit;  //PID输出上限
    s32 OutLowLimit; //PID输出下限
    u16 KpFactor;    //比例因子
    u16 KiFactor;    //积分因子
    u16 DutyRatio;   //PID计算输出
}
MPID_STRUCT;

/*----------------------------------------------------------------------------*/
typedef struct
{
    u8 EnMotorRun;//电机运行停止
    
    u8 ErrHall;   //比例增益
}
SpeedFlag_STRUCT;

/*----------------------------------------------------------------------------*/
typedef struct
{
    u16 pd;   //比例增益
    u16 id;   //积分系数
    u16 intt; //加速时间
    u16 det;  //减速时间
    
    u16 sdl;  //速度设定下限
    u16 sdh;  //速度设定上限
    u16 pol;  //极数(有刷：霍尔测速的级数)
    u16 db;   //显示不灵敏区
}
SpeedPara_STRUCT;

/*******************************************************************************
                    ---------------- 对外 -----------------
*******************************************************************************/
//电机运行标志位
extern SpeedFlag_STRUCT Mflag;

//速度参数表
extern SpeedPara_STRUCT ParaSpd;

//速度值
extern u16 SpdSetVal;    //速度设定值
extern u16 SpdDisplay;   //速度显示值

//采样计算速度
extern u32 TIM2_CaptureBuff[2];

//PWM分辨率
extern u16 PWMResolution;
/*----------------------------------------------------------------------------*/
//时间变量
extern u8 TimMs_StepWait;   //斜坡一步时间
extern u8 TimMs_UpdatePWM;  //PWM赋值更新计时
extern u8 TimMs_SpeedCalculate;//计算速度延时
extern u16 TimMs_SpdToDisplay; //速度显示值500ms刷新
extern u8 TimS_HallDetect;     //霍尔故障检测3s累计计时
extern u8 TimMs_MotorTrig;     //2ms触发电机控制
/*------------------------------------------------------------------------------
*********************************** 函数声明 ***********************************
------------------------------------------------------------------------------*/
void MotorCtrl(void);     //电机运行主函数
void SpeedCalculate(void);//速度计算

#endif
