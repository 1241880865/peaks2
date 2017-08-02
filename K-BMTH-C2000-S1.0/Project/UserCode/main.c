/***********************************Copyright(c)********************************
**   Product Name:  K-BMTH-C2000-S1.0
**       MCU Type�� STM8S105K4T6
**    Sensor Type:  Pt100
**  Setting Range: ��Set(0.0 �� 400.0��)��Display(-50.0 �� 410.0��)��
** 		 
**       Designer:  SongHaihao
**   Company Name:  Peaks Measure&Control Tech Ltd
**           Date:  2017-08-02
**         Vision:  S1.0
--------------------------------------------------------------------------------
������������������������������������ ���ļ�¼ ����������������������������������
--------------------------------------------------------------------------------
**  ��������:  **  2017-08-02
**    �汾��:  **  S1.0
**  ��������:  **  1��
--------------------------------------------------------------------------------
***********************************ͷ�ļ�����***********************************
------------------------------------------------------------------------------*/
#include "stm8s.h"
#include "main.h"
#include "DRIVER.h"

#include "key.h"
#include "display.h"

#include "heatCtrl.h"
#include "MotorDriver.h"
/*------------------------------------------------------------------------------
*********************************** �������� ***********************************
------------------------------------------------------------------------------*/
//�����Ʊ�־λ����
Control_FlagInit CtrlFlag;
PARATHREE m_para3;
PARAFOUR  m_para4;

//ʱ�����
u8  TimS_PowerOn;      //�ϵ��ʱ4����
u16 TimMs_SpeakPowerOn;//�ϵ�״̬��������ʱ
u8  TimS_BuzzFlash;    //��������˸����
/*------------------------------------------------------------------------------
*********************************** �ڲ����� ***********************************
------------------------------------------------------------------------------*/
void PowerUpState(void); //�ϵ��ʼ״̬
void RunCtrl(void);      //��ͣ���п���
void AlarmCtrl(void);    //���ϼ��д���
void BuzzOut(u8 buzz_on, u8 buzz_way);//����������
/*******************************************************************************
*	Function name:	      main
*	Descriptions:	      The main function
*	Parameters:	      int
*	Returned value:       void
*******************************************************************************/
int main( void )
{
    Delay_Ms(200);     //�ȴ���Դ�ȶ���ʱ
    
    disableInterrupts();//�ر��ж�
    CLK_Config();      //ʱ�ӳ�ʼ��
    GPIO_Config();     //GPIO��ʼ��
    TIM1_Config();     //TIM1��ʼ��
    TIM2_Config();     //TIM2��ʼ��
    TIM4_Config();     //TIM4��ʼ��
    ADC1_Config();     //ADC1��ʼ��
    enableInterrupts();//���ж�
    
    DataFirstInit();//���ݳ�ʼ��
    ReadEEprom();   //��ȡ�趨��������
    ReadProtect();  //�����籣��״̬
    
    IWDG_Config();  //���Ź���ʼ��
    HT1621_Init();  //HT1621��ʼ��
    
    PowerUpState(); //�ϵ��ʼ״̬
    
    while(1)
    {
        //���� ��ʾ
        ScanKey(); //����ɨ��
        RunCtrl(); //����
        Display(); //��ʾ������
        
        //������Ƽ�TIM1�����ж�
        
        //�¶�
        TempADToDisp();//ad��������ʾֵ
        HeatControl(); //�¶ȿ���
        TempAlarm();   //�¶ȹ��ϼ��
        AT_pid();      //������
        
        //������
        AlarmCtrl();
        
        //���¿��Ź�
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
    
    Tflag.ManlAlm = 1; //���ϵ糬�±�������ʾ
    
    while(TimS_PowerOn < 2)
    {
        /* ������ */
        if (TimMs_SpeakPowerOn <= 300)
        {
            BuzzOut(1, 0); //�ϵ�ʱ����������
        }
        else
        {
            BuzzOut(0, 0);
            TimMs_SpeakPowerOn = 320;
        }
        
        /* �ϵ���ʾ */
        DisplayPwr();    //һ����ʾ"P1-C"; ������ʾ"P1C6"
        
        /* �¶Ȳ��� */
        TempADToDisp();  //ad��������ʾֵ
        for (i = 0; i < 5; i++) //�ϵ��˲����鸳ֵ
        {
            *(DataFilter_Buff + i) = Pt100.SmplVal;
        }
        
        //���Ź�����
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
    else //�޹���
    {
        BuzzOut(0, 0);
        CtrlFlag.Mute = 0;
    }
}

/*******************************************************************************
*	Function name:	      BuzzOut
*	Descriptions:	      BuzzOut
*	Parameters:	      1�����������ر�־λ
                              2�����������з�ʽ
*	Returned value:       void
*******************************************************************************/
void BuzzOut(u8 buzz_on, u8 buzz_way)
{
    //����
    if (CtrlFlag.Mute != 0)
    {
        buzz_on = 0;
    }
    
    //�������
    if (buzz_on != 0)
    {
        //���з�ʽ
        if (buzz_way != 0)
        {
            if (TimS_BuzzFlash < 1) //better�޸�Ϊms
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
