/***********************************Copyright(c)********************************
**   Product Name:  K-BMTH-C2000-S1.0
**       MCU Type： STM8S105K4T6
**    Sensor Type:  Pt100
**  Setting Range: 【Set(0.0 ～ 400.0℃)；Display(-50.0 ～ 410.0℃)】
** 		 
**       Designer:  SongHaihao
**   Company Name:  Peaks Measure&Control Tech Ltd
**           Date:  2017-08-02
**         Vision:  S1.0
--------------------------------------------------------------------------------
～～～～～～～～～～～～～～～～～～ 更改记录 ～～～～～～～～～～～～～～～～～
--------------------------------------------------------------------------------
**  创建日期:  **  2017-08-02
**    版本号:  **  S1.0
**  功能描述:  **  1、
--------------------------------------------------------------------------------
***********************************头文件包含***********************************
------------------------------------------------------------------------------*/
#include "stm8s.h"
#include "main.h"
#include "DRIVER.h"

#include "key.h"
#include "display.h"

#include "heatCtrl.h"
#include "MotorDriver.h"
/*------------------------------------------------------------------------------
*********************************** 变量定义 ***********************************
------------------------------------------------------------------------------*/
//主控制标志位定义
Control_FlagInit CtrlFlag;
PARATHREE m_para3;
PARAFOUR  m_para4;

//时间变量
u8  TimS_PowerOn;      //上电计时4秒钟
u16 TimMs_SpeakPowerOn;//上电状态蜂鸣器计时
u8  TimS_BuzzFlash;    //蜂鸣器闪烁鸣叫
/*------------------------------------------------------------------------------
*********************************** 内部函数 ***********************************
------------------------------------------------------------------------------*/
void PowerUpState(void); //上电初始状态
void RunCtrl(void);      //起停运行控制
void AlarmCtrl(void);    //故障集中处理
void BuzzOut(u8 buzz_on, u8 buzz_way);//蜂鸣器控制
/*******************************************************************************
*	Function name:	      main
*	Descriptions:	      The main function
*	Parameters:	      int
*	Returned value:       void
*******************************************************************************/
int main( void )
{
    Delay_Ms(200);     //等待电源稳定延时
    
    disableInterrupts();//关闭中断
    CLK_Config();      //时钟初始化
    GPIO_Config();     //GPIO初始化
    TIM1_Config();     //TIM1初始化
    TIM2_Config();     //TIM2初始化
    TIM4_Config();     //TIM4初始化
    ADC1_Config();     //ADC1初始化
    enableInterrupts();//打开中断
    
    DataFirstInit();//数据初始化
    ReadEEprom();   //读取设定参数数据
    ReadProtect();  //读掉电保护状态
    
    IWDG_Config();  //看门狗初始化
    HT1621_Init();  //HT1621初始化
    
    PowerUpState(); //上电初始状态
    
    while(1)
    {
        //按键 显示
        ScanKey(); //按键扫描
        RunCtrl(); //控制
        Display(); //显示主函数
        
        //电机控制见TIM1触发中断
        
        //温度
        TempADToDisp();//ad采样到显示值
        HeatControl(); //温度控制
        TempAlarm();   //温度故障检测
        AT_pid();      //自整定
        
        //蜂鸣器
        AlarmCtrl();
        
        //更新看门狗
        IWDG_ReloadCounter();
    }
}

/*******************************************************************************
*	Function name:	      RunCtrl
*	Descriptions:	      control
*	Parameters:	      void
*	Returned value:       void
*******************************************************************************/
void RunCtrl(void)
{
    if(CtrlFlag.flgRun == 0)
    {
        Mflag.EnMotorRun = 0;
        Tflag.EnTempRun = 0;
        Tflag.EnAT = 0;
    }
    else
    {
        Mflag.EnMotorRun = 1;
        Tflag.EnTempRun = 1;
        
        if(SpdSetVal == 0)
        {
            Mflag.EnMotorRun = 0;
            Mflag.ErrHall = 0;
        }
        else if(Mflag.ErrHall != 0)
        {
            Mflag.EnMotorRun = 0;
        }
    }
}

/*******************************************************************************
*	Function name:	      PowerUpState
*	Descriptions:	      Power-Up State
*	Parameters:	      void
*	Returned value:       void
*******************************************************************************/
void PowerUpState(void)
{
    u8 i;
    
    Tflag.ManlAlm = 1; //刚上电超温报警不提示
    
    while(TimS_PowerOn < 2)
    {
        /* 蜂鸣器 */
        if (TimMs_SpeakPowerOn <= 300)
        {
            BuzzOut(1, 0); //上电时蜂鸣器鸣叫
        }
        else
        {
            BuzzOut(0, 0);
            TimMs_SpeakPowerOn = 320;
        }
        
        /* 上电显示 */
        DisplayPwr();    //一排显示"P1-C"; 二排显示"P1C6"
        
        /* 温度采样 */
        TempADToDisp();  //ad采样到显示值
        for (i = 0; i < 5; i++) //上电滤波数组赋值
        {
            *(DataFilter_Buff + i) = Pt100.SmplVal;
        }
        
        //看门狗重载
        IWDG_ReloadCounter();
    }
}

/*******************************************************************************
*	Function name:	      AlarmCtrl
*	Descriptions:	      The main function of Alarm Control
*	Parameters:	      int
*	Returned value:       void
*******************************************************************************/
void AlarmCtrl(void)
{
    if (Tflag.TempOver != 0)
    {
        ALM_OFF;
        HEAT_OFF;
        BuzzOut(1, 0);
    }
    else if (Mflag.ErrHall != 0)
    {
        BuzzOut(1, 1);
    }
    else if (Tflag.TAlmSpk != 0)
    {
        BuzzOut(1, 0);
    }
    else //无故障
    {
        BuzzOut(0, 0);
        CtrlFlag.Mute = 0;
    }
}

/*******************************************************************************
*	Function name:	      BuzzOut
*	Descriptions:	      BuzzOut
*	Parameters:	      1、蜂鸣器开关标志位
                              2、蜂鸣器鸣叫方式
*	Returned value:       void
*******************************************************************************/
void BuzzOut(u8 buzz_on, u8 buzz_way)
{
    //消音
    if (CtrlFlag.Mute != 0)
    {
        buzz_on = 0;
    }
    
    //鸣叫输出
    if (buzz_on != 0)
    {
        //鸣叫方式
        if (buzz_way != 0)
        {
            if (TimS_BuzzFlash < 1) //better修改为ms
            {
                BUZZER_ON;
            }
            else if (TimS_BuzzFlash < 2)
            {
                BUZZER_OFF;
            }
            else
            {
                TimS_BuzzFlash = 0;
            }
        }
        else
        {
            BUZZER_ON;
        }
    }
    else
    {
        BUZZER_OFF;
    }
}

/*******************************************************************************
*	Function name:	      assert_failed
*	Descriptions:	      assert_failed
*	Parameters:	      
*	Returned value:       void
*******************************************************************************/
//void assert_failed(u8* file, u32 line)
//{
//    while (1)
//    {
//        
//    }
//} 
