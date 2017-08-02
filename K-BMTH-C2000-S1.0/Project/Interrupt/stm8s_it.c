


/*------------------------------------------------------------------------------
***********************************头文件包含***********************************
----Includes -----------------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_it.h"
#include "main.h"

#include "key.h"
#include "display.h"

#include "heatCtrl.h"
#include "MotorDriver.h"
/*------------------------------------------------------------------------------
*********************************** 变量声明 ***********************************
------------------------------------------------------------------------------*/
static u16 TimMs_Ms;  //Tim4毫秒时基
/*******************************************************************************
         Function name:    TIM1_UPD_OVF_TRG_BRK_IRQHandler
	  Descriptions:	   Check the HALL error and the IPM fault
	    Parameters:	   void
        Returned value:    void
------------------------------------------------------------------------------*/
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11) //TIM1触发中断
{
    SpeedCalculate(); //计算速度
    MotorCtrl();      //电机控制
    
    //清触发中断标志位
    TIM1_ClearITPendingBit(TIM1_IT_TRIGGER);
}

/*******************************************************************************
         Function name:    TIM2_UPD_OVF_BRK_IRQHandler
	  Descriptions:	   The TIM2 capture buffer assignment
	    Parameters:	   void
        Returned value:    void
------------------------------------------------------------------------------*/
INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13)
{
    TIM2_ClearITPendingBit(TIM2_IT_UPDATE); //清除溢出标志位
    
    TIM2_CaptureBuff[0] = 0xFFFFFF;
    TIM2_CaptureBuff[1] = 0xFFFFFF;
}

/*******************************************************************************
         Function name:    TIM2_CAP_COM_IRQHandler
	  Descriptions:	   Handle the TIM2 input capture interrupt
                           judge the hall code and obtain the speed value
	    Parameters:	   void
        Returned value:    void
------------------------------------------------------------------------------*/
INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14) //捕获中断（测速）
{
    static u8 hallstatus;
    
    /* 清捕获2中断标志位 */
    TIM2_ClearITPendingBit(TIM2_IT_CC2);
    
    /* 霍尔故障检测计时清零 */
    TimS_HallDetect = 0;
    
    /* 修改检测沿 */
    if(HALL_SENSOR)
    {
        hallstatus = 0;
        TIM2->CCER1 |= 0x20; //下降沿捕获（库函数为static）
    }
    else
    {
        hallstatus = 1;
        TIM2->CCER1 &= (~0x20); //上升沿捕获
    }
    
    /* 获取(TIM2_CH2通道)计数值 */
    TIM2_CaptureBuff[hallstatus] = TIM2_GetCapture2();
    
    /* TIM2计数值清零 */
    TIM2_SetCounter(0x0000);
}

/*******************************************************************************
         Function name:    ADC1_IRQHandler
	  Descriptions:	   Handle the ADC1 change over interrupt
	    Parameters:	   void
        Returned value:    void
------------------------------------------------------------------------------*/
INTERRUPT_HANDLER(ADC1_IRQHandler, 22)
{
    Pt100.OverVal += ADC1_GetConversionValue();
    
    //15位过采样处理【10位AD处理为15位AD】
    Pt100.SmplCont++;
    if (Pt100.SmplCont >= 1024)
    {
        Pt100.SmplCont = 0;
        Pt100.OverVal >>= 5;
        Pt100.SmplSum += Pt100.OverVal;
        Pt100.OverVal = 0;
        
        Pt100.AverageCont++;
        if(Pt100.AverageCont >= 16)
        {
            Pt100.AverageCont = 0;
            Pt100.SmplSum >>= 4;
            Pt100.SmplVal = Pt100.SmplSum;
            Pt100.SmplSum = 0;
            Pt100.FlagOK = 1;
        }
    }
    
    ADC1_ClearITPendingBit(ADC1_IT_EOC);//清中断标志位
    ADC1_StartConversion();//启动AD转换
}

/*******************************************************************************
         Function name:    TIM4_UPD_OVF_IRQHandler
	  Descriptions:    Handle the TIM4 Overflow Interrupt
	    parameters:    void
        Returned value:    void
------------------------------------------------------------------------------*/
INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
    TimMs_Ms++;
    if (TimMs_Ms >= 1000)  
    {
        TimMs_Ms = 0;
        
        TimS_PowerOn++;    //上电状态计时
        
        TimS_HeatPeriod++; //加热周期计时
        TimS_EnAT++;       //自整定时间计时
        
        TimS_HallDetect++; //霍尔故障检测3s累计计时
        TimS_BuzzFlash++;  //蜂鸣器鸣叫闪烁
    }
    
    TimMs_MotorTrig++;
    if (TimMs_MotorTrig >= 2)
    {
        TimMs_MotorTrig = 0;
        
        //手动产生更新事件
        TIM1_GenerateEvent(TIM1_EVENTSOURCE_TRIGGER);
    }
    
    TimMs_SpeakPowerOn++; //上电蜂鸣器鸣叫计时
    TimMs_DisDelay++;
    TimMs_keyLgset++;
    TimMs_Jitter++;
    TimMs_KeyDelayAdd++;
    
    TimMs_DelaySetting++;
    TimMs_FlashDelay++;
    TimMs_LampFlash++;
    TimMs_ALMFlash++;  //电机故障闪烁
    TimMs_ATFlash++;   //自整定闪烁
    
    TimMs_StepWait++;   //斜坡一步时间
    TimMs_UpdatePWM++;  //PWM赋值更新计时
    TimMs_SpeedCalculate++;//计算速度延时
    TimMs_SpdToDisplay++;  //速度显示值500ms刷新
    
    TimMs_HeatCounter++;//加热周期占空比计时
        
    //清除中断标志位
    TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
}

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- End Of File -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/




