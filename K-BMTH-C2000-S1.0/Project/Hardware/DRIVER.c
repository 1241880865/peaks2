


#include "stm8s.h"
#include "DRIVER.h"
#include "MotorDriver.h"

/*------------------------------------------------------------------------------
*************************************�궨��*************************************
------------------------------------------------------------------------------*/
//λ�궨��
#define BITN 0x00
#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

/*******************************************************************************
         Function name:    Delay_Ms
	  Descriptions:	   Delay x milliseconds
	    Parameters:	   u16 xms
        Returned value:    void
------------------------------------------------------------------------------*/
void Delay_Ms(u16 xms)
{
    u16 counter;
    
    while(xms--)
    {
        for (counter = 0; counter < 200; counter++)
        {
            nop();nop();nop();nop();
            nop();nop();nop();nop();
        }
    }
}

/*******************************************************************************
         Function name:    CLK_Config
	  Descriptions:    The clock initial
	    parameters:    void
        Returned value:    void
------------------------------------------------------------------------------*/
void CLK_Config(void) /* 16MHz(����ˢ�������) */
{
    /* ʱ�Ӹ�λ */
    CLK_DeInit();
    
    /* ʱ��Դѡ�� */
    CLK_HSICmd(ENABLE); //�ڲ���Ƶʱ�ӿ�
    CLK_HSECmd(DISABLE);//�ⲿ��Ƶʱ�ӹ�
    
    /* �ſ�ʱ��(stm8s105K4) */
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);   //ADC��
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART2, ENABLE); //UART2��
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE);//TIM1��
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);//TIM2��
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER3, DISABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);//TIM4��
    
    /* ��ʱ�ӷ�Ƶ */
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);//����HSI(1)��Ƶ
    CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);      //����CPU(1)��Ƶ��Ҳ������HSI��Ƶ
}

/*******************************************************************************
         Function name:    CLK_Config
	  Descriptions:    The clock initial
	    parameters:    void
        Returned value:    void
------------------------------------------------------------------------------*/
void ITC_Config(void)
{
    ITC_SetSoftwarePriority(ITC_IRQ_TIM1_OVF, ITC_PRIORITYLEVEL_1);
    ITC_SetSoftwarePriority(ITC_IRQ_TIM2_OVF, ITC_PRIORITYLEVEL_2);
    ITC_SetSoftwarePriority(ITC_IRQ_TIM2_CAPCOM, ITC_PRIORITYLEVEL_2);
    ITC_SetSoftwarePriority(ITC_IRQ_ADC1, ITC_PRIORITYLEVEL_2);
    ITC_SetSoftwarePriority(ITC_IRQ_TIM4_OVF, ITC_PRIORITYLEVEL_2);
}

/*******************************************************************************
         Function name:    TIM1_Config
	  Descriptions:	   The TIM1 initial
	    Parameters:	   void
        Returned value:    void
------------------------------------------------------------------------------*/
void TIM1_Config(void)
{
    /* ��λTIM1 */
    TIM1_DeInit();
    
    /* TIM1�������� */
    PWMResolution = (u16)(16000 / FREQUENCY);//ARR�Զ���װ��ֵ
    TIM1_TimeBaseInit(0x0000,               //Ԥ��Ƶ�޷�Ƶ
                      TIM1_COUNTERMODE_UP,  //���ϼ���
                      PWMResolution,        //ARR�Զ���װ��ֵ
                      0x00);                //��ֹ�ظ�����
    
//    /* ��ֹ����ģʽ */
//    TIM1_SelectMasterSlaveMode(DISABLE);
//    
//    /* �ⲿ������ֹ */
//    TIM1_ETRConfig(TIM1_EXTTRGPSC_OFF,          //Ԥ��Ƶ�ر�
//                   TIM1_EXTTRGPOLARITY_INVERTED,//�ⲿ�������ԣ��ߵ�ƽ��������
//                   0x00);                       //���˲�
    
    /* ����Ƚ�ģʽ����(TIM1ͨ��1) */
    TIM1_OC1Init(TIM1_OCMODE_PWM1,         //PWMģʽ1
                 TIM1_OUTPUTSTATE_ENABLE,  //���ò���/�Ƚ����
                 TIM1_OUTPUTNSTATE_DISABLE,//��ֹ����/�Ƚϻ������
                 0x0000,                   //������ֵ(�Ƚ�ֵ)
                 TIM1_OCPOLARITY_HIGH,     //�ߵ�ƽ��Ч
                 TIM1_OCNPOLARITY_LOW,     //����ͨ���͵�ƽ��Ч
                 TIM1_OCIDLESTATE_RESET,   //�������״̬�Ĵ�������
                 TIM1_OCNIDLESTATE_RESET); //�����������״̬�Ĵ�������
    
    /* ɲ������(��������������һ������һ��ڲ�����ȡֵ����--�෴) */
    TIM1_BDTRConfig(TIM1_OSSISTATE_DISABLE,//����ģʽ��"�ر�״̬",����ʱ��������ʱ����ֹOC/OCN�����
                    TIM1_LOCKLEVEL_OFF,    //�����رգ��Ĵ�����д����
                    DEAD_TIME,             //����ʱ��
                    TIM1_BREAK_DISABLE,    //��ֹɲ������
                    TIM1_BREAKPOLARITY_LOW,//ɲ������͵�ƽ��Ч
                    TIM1_AUTOMATICOUTPUT_DISABLE);//��ֹ�Զ������MOEֻ�ܱ������1
    
    /* Ԥװ��ʹ�� */
    TIM1_OC2PreloadConfig(ENABLE);//����Ƚ�2Ԥװ��ʹ��
    
    /* ��������������¼� */
    TIM1_GenerateEvent(TIM1_EVENTSOURCE_COM);   //����/�Ƚ��¼�,�������Ƹ���
    TIM1_GenerateEvent(TIM1_EVENTSOURCE_UPDATE);//���������¼�
    
    /* ������TIM1�ж����ã�2ms�����ж� */
    TIM1_ITConfig(TIM1_IT_TRIGGER, ENABLE);
    
    /* ʹ����� */
    TIM1_CtrlPWMOutputs(ENABLE);//��ɲ��
    TIM1_Cmd(ENABLE);           //ʹ��TIM1
}

/*******************************************************************************
         Function name:    TIM2_Config
	  Descriptions:	   The TIM2 initial					
	    Parameters:	   void
        Returned value:    void							 
------------------------------------------------------------------------------*/
void TIM2_Config(void)
{
    /* ��λTIM2 */
    TIM2_DeInit();
    
    /* TIM2�������� */
    TIM2_TimeBaseInit(TIM2_PRESCALER_256, 0xFFFF);//������ʱ��256��Ƶ;(ÿ�μ���ʱ��Ϊ16us��(1/16MHz)*256��)
                                                  //�Զ�װ�ؼĴ�������ֵ
    /* ���벶������ */
    TIM2_ICInit(TIM2_CHANNEL_2,           //���벶��ͨ��2
                TIM2_ICPOLARITY_RISING,   //�����ز���
                TIM2_ICSELECTION_DIRECTTI,//���������ӳ���ڣ�TI2FP2
                TIM2_ICPSC_DIV1,          //����Ԥ��Ƶ��1��Ƶ��1/2/4/8���¼�����һ�Σ�
                0x0a);                    //�˲�Ƶ��(8/16)MHz������5�������ƽ�źź󴥷�
    
    /* �ж����� */
    TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);//���������ʱ�����ж�
    TIM2_ITConfig(TIM2_IT_CC2, ENABLE);   //����TIM2_CH2�����ж�
    
    /* ʹ�ܼ����� */
    TIM2_Cmd(ENABLE);
}

/*******************************************************************************
         Function name:    TIM4_Config
	  Descriptions:	   The TIM4 initial					
	    Parameters:	   void
        Returned value:    void							 
------------------------------------------------------------------------------*/
void TIM4_Config(void)
{
    TIM4->ARR = 0x7D;  //��125*8us=1ms;�趨��ʱʱ�䡿
    
    TIM4->PSCR = 0x07; //128��Ƶ
    TIM4->IER |= 0x01; //ʹ�ܶ�ʱ���ж�
    TIM4->CR1 |= 0x01; //��ʼ��ʱ
}

/*******************************************************************************
         Function name:    ADC1_Config
	  Descriptions:    The ADC1 initial			
	    parameters:    void
        Returned value:    void							 
------------------------------------------------------------------------------*/
void ADC1_Config(void)
{
    ADC1_DeInit(); //��λADC1
    ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE,//����ת��
                          ADC1_CHANNEL_4,      //ѡ��ADת��ͨ��:AIN4��Pt100��
                          ADC1_ALIGN_RIGHT);   //����*����
    ADC1_PrescalerConfig(ADC1_PRESSEL_FCPU_D10);//ADC1ת��ʱ��2MHz,��ʱ��8��Ƶ
    ADC1_ITConfig(ADC1_IT_EOCIE, ENABLE);      //ʹ��ת�������ж�
    ADC1_Cmd(ENABLE);      //ʹ��ADת��
    ADC1_StartConversion();//����ADת��
}

/*******************************************************************************
         Function name:    IWDG_Config
	  Descriptions:	   The IDWG initial
	    Parameters:	   void
        Returned value:    void
------------------------------------------------------------------------------*/
void IWDG_Config(void)
{
    /* ������˳�� */
    IWDG_Enable();       //ʹ���ڲ����Ź�
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//������ܱ���IWDG�Ĵ�������
    
    IWDG_SetPrescaler(IWDG_Prescaler_128);//128��Ƶ
    IWDG_SetReload(0xFF);//װ�ؼ���ֵ(���ʱ��510mS)
    IWDG_ReloadCounter();//���¿��Ź�
}

/*******************************************************************************
*	Function name:	      GPIO_Config
*	Descriptions:	      GPIO_Config
*	Parameters:	      void
*	Returned value:       void
*******************************************************************************/
/*
      stm8s105K4 ��25��GPIO��
*/
void GPIO_Config(void)
{
    //���Ÿ�λ
    GPIO_DeInit(GPIOA);
    GPIO_DeInit(GPIOB);
    GPIO_DeInit(GPIOC);
    GPIO_DeInit(GPIOD);
    GPIO_DeInit(GPIOE);
    GPIO_DeInit(GPIOF);
    
    //PinA(2) 1 2
    GPIO_Init(GPIOA, GPIO_PIN_1, GPIO_MODE_OUT_OD_HIZ_SLOW);//δ�ö˿ڣ���©��
    GPIO_Init(GPIOA, GPIO_PIN_2, GPIO_MODE_OUT_OD_HIZ_SLOW);//δ�ö˿ڣ���©��
    
    //PinB(6) 0~3 4 5
    GPIO_Init(GPIOB, GPIO_PIN_0, GPIO_MODE_OUT_PP_HIGH_SLOW);//LCD_DATA
    GPIO_Init(GPIOB, GPIO_PIN_1, GPIO_MODE_OUT_PP_HIGH_SLOW);//LCD_CLK
    GPIO_Init(GPIOB, GPIO_PIN_2, GPIO_MODE_OUT_PP_HIGH_SLOW);//LCD_CS
    GPIO_Init(GPIOB, GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);     //ADC(Pt100)��������
    GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);     //ADC(Pt100)��������
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_SLOW);//���������������
    
    //PinC(7) 1~2 3~7
    GPIO_Init(GPIOC, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_SLOW);//PWM���������
    GPIO_Init(GPIOC, GPIO_PIN_2, GPIO_MODE_OUT_OD_HIZ_SLOW);//δ�ö˿ڣ���©��
    GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT);//���� �����������ж�
    GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT);//���� �����������ж�
    GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT);//���� �����������ж�
    GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT);//���� �����������ж�
    GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_IN_PU_NO_IT);//���� �����������ж�
    
    //PinD(8)  0~7
    GPIO_Init(GPIOD, GPIO_PIN_0, GPIO_MODE_OUT_OD_HIZ_SLOW); //δ�ö˿ڣ���©��
    GPIO_Init(GPIOD, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_SLOW); //SWIM   �������
    GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_OUT_OD_HIZ_SLOW); //δ�ö˿ڣ���©��
    GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);     //����  ��������
    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_OD_HIZ_SLOW); //δ�ö˿ڣ���©��
    GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_SLOW); //δ�ö˿ڣ���©��
    GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_OUT_PP_HIGH_SLOW);//�������������
    GPIO_Init(GPIOD, GPIO_PIN_7, GPIO_MODE_OUT_PP_HIGH_SLOW);//���ȣ��������
    
    //PinE(1) 5
    GPIO_Init(GPIOE, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_SLOW);//δ�ö˿ڣ���©��
    
    //PinF(1) 4
    GPIO_Init(GPIOF, GPIO_PIN_4, GPIO_MODE_OUT_OD_HIZ_SLOW);//δ�ö˿ڣ���©��
}