


/*------------------------------------------------------------------------------
***********************************ͷ�ļ�����***********************************
----Includes -----------------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_it.h"
#include "main.h"

#include "key.h"
#include "display.h"

#include "heatCtrl.h"
#include "MotorDriver.h"
/*------------------------------------------------------------------------------
*********************************** �������� ***********************************
------------------------------------------------------------------------------*/
static u16 TimMs_Ms;  //Tim4����ʱ��
/*******************************************************************************
         Function name:    TIM1_UPD_OVF_TRG_BRK_IRQHandler
	  Descriptions:	   Check the HALL error and the IPM fault
	    Parameters:	   void
        Returned value:    void
------------------------------------------------------------------------------*/
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11) //TIM1�����ж�
{
    SpeedCalculate(); //�����ٶ�
    MotorCtrl();      //�������
    
    //�崥���жϱ�־λ
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
    TIM2_ClearITPendingBit(TIM2_IT_UPDATE); //��������־λ
    
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
INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14) //�����жϣ����٣�
{
    static u8 hallstatus;
    
    /* �岶��2�жϱ�־λ */
    TIM2_ClearITPendingBit(TIM2_IT_CC2);
    
    /* �������ϼ���ʱ���� */
    TimS_HallDetect = 0;
    
    /* �޸ļ���� */
    if(HALL_SENSOR)
    {
        hallstatus = 0;
        TIM2->CCER1 |= 0x20; //�½��ز��񣨿⺯��Ϊstatic��
    }
    else
    {
        hallstatus = 1;
        TIM2->CCER1 &= (~0x20); //�����ز���
    }
    
    /* ��ȡ(TIM2_CH2ͨ��)����ֵ */
    TIM2_CaptureBuff[hallstatus] = TIM2_GetCapture2();
    
    /* TIM2����ֵ���� */
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
    
    //15λ����������10λAD����Ϊ15λAD��
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
    
    ADC1_ClearITPendingBit(ADC1_IT_EOC);//���жϱ�־λ
    ADC1_StartConversion();//����ADת��
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
        
        TimS_PowerOn++;    //�ϵ�״̬��ʱ
        
        TimS_HeatPeriod++; //�������ڼ�ʱ
        TimS_EnAT++;       //������ʱ���ʱ
        
        TimS_HallDetect++; //�������ϼ��3s�ۼƼ�ʱ
        TimS_BuzzFlash++;  //������������˸
    }
    
    TimMs_MotorTrig++;
    if (TimMs_MotorTrig >= 2)
    {
        TimMs_MotorTrig = 0;
        
        //�ֶ����������¼�
        TIM1_GenerateEvent(TIM1_EVENTSOURCE_TRIGGER);
    }
    
    TimMs_SpeakPowerOn++; //�ϵ���������м�ʱ
    TimMs_DisDelay++;
    TimMs_keyLgset++;
    TimMs_Jitter++;
    TimMs_KeyDelayAdd++;
    
    TimMs_DelaySetting++;
    TimMs_FlashDelay++;
    TimMs_LampFlash++;
    TimMs_ALMFlash++;  //���������˸
    TimMs_ATFlash++;   //��������˸
    
    TimMs_StepWait++;   //б��һ��ʱ��
    TimMs_UpdatePWM++;  //PWM��ֵ���¼�ʱ
    TimMs_SpeedCalculate++;//�����ٶ���ʱ
    TimMs_SpdToDisplay++;  //�ٶ���ʾֵ500msˢ��
    
    TimMs_HeatCounter++;//��������ռ�ձȼ�ʱ
        
    //����жϱ�־λ
    TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
}

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- End Of File -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/




