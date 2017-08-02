


#include "stm8s.h"
#include "DRIVER.h"
#include "MotorDriver.h"

/*------------------------------------------------------------------------------
*************************************宏定义*************************************
------------------------------------------------------------------------------*/
//位宏定义
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
void CLK_Config(void) /* 16MHz(带有刷电机驱动) */
{
    /* 时钟复位 */
    CLK_DeInit();
    
    /* 时钟源选择 */
    CLK_HSICmd(ENABLE); //内部高频时钟开
    CLK_HSECmd(DISABLE);//外部高频时钟关
    
    /* 门控时钟(stm8s105K4) */
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);   //ADC开
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART2, ENABLE); //UART2开
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE);//TIM1开
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);//TIM2开
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER3, DISABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);//TIM4开
    
    /* 主时钟分频 */
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);//配置HSI(1)分频
    CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);      //配置CPU(1)分频，也可配置HSI分频
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
    /* 复位TIM1 */
    TIM1_DeInit();
    
    /* TIM1基础配置 */
    PWMResolution = (u16)(16000 / FREQUENCY);//ARR自动重装载值
    TIM1_TimeBaseInit(0x0000,               //预分频无分频
                      TIM1_COUNTERMODE_UP,  //向上计数
                      PWMResolution,        //ARR自动重装载值
                      0x00);                //禁止重复计数
    
//    /* 禁止主从模式 */
//    TIM1_SelectMasterSlaveMode(DISABLE);
//    
//    /* 外部触发禁止 */
//    TIM1_ETRConfig(TIM1_EXTTRGPSC_OFF,          //预分频关闭
//                   TIM1_EXTTRGPOLARITY_INVERTED,//外部触发极性：高电平或上升沿
//                   0x00);                       //无滤波
    
    /* 输出比较模式配置(TIM1通道1) */
    TIM1_OC1Init(TIM1_OCMODE_PWM1,         //PWM模式1
                 TIM1_OUTPUTSTATE_ENABLE,  //启用捕获/比较输出
                 TIM1_OUTPUTNSTATE_DISABLE,//禁止捕获/比较互补输出
                 0x0000,                   //脉冲宽度值(比较值)
                 TIM1_OCPOLARITY_HIGH,     //高电平有效
                 TIM1_OCNPOLARITY_LOW,     //互补通道低电平有效
                 TIM1_OCIDLESTATE_RESET,   //输出空闲状态寄存器清零
                 TIM1_OCNIDLESTATE_RESET); //互补输出空闲状态寄存器清零
    
    /* 刹车功能(代码生成器，下一函数第一入口参数，取值错误--相反) */
    TIM1_BDTRConfig(TIM1_OSSISTATE_DISABLE,//运行模式下"关闭状态",当定时器不工作时，禁止OC/OCN输出；
                    TIM1_LOCKLEVEL_OFF,    //锁定关闭，寄存器无写保护
                    DEAD_TIME,             //死区时间
                    TIM1_BREAK_DISABLE,    //禁止刹车输入
                    TIM1_BREAKPOLARITY_LOW,//刹车输入低电平有效
                    TIM1_AUTOMATICOUTPUT_DISABLE);//禁止自动输出，MOE只能被软件置1
    
    /* 预装载使能 */
    TIM1_OC2PreloadConfig(ENABLE);//输出比较2预装载使能
    
    /* 由软件产生更新事件 */
    TIM1_GenerateEvent(TIM1_EVENTSOURCE_COM);   //捕获/比较事件,产生控制更新
    TIM1_GenerateEvent(TIM1_EVENTSOURCE_UPDATE);//产生更新事件
    
    /* 新添加项：TIM1中断配置，2ms触发中断 */
    TIM1_ITConfig(TIM1_IT_TRIGGER, ENABLE);
    
    /* 使能输出 */
    TIM1_CtrlPWMOutputs(ENABLE);//开刹车
    TIM1_Cmd(ENABLE);           //使能TIM1
}

/*******************************************************************************
         Function name:    TIM2_Config
	  Descriptions:	   The TIM2 initial					
	    Parameters:	   void
        Returned value:    void							 
------------------------------------------------------------------------------*/
void TIM2_Config(void)
{
    /* 复位TIM2 */
    TIM2_DeInit();
    
    /* TIM2基础配置 */
    TIM2_TimeBaseInit(TIM2_PRESCALER_256, 0xFFFF);//计数器时钟256分频;(每次计数时间为16us【(1/16MHz)*256】)
                                                  //自动装载寄存器赋初值
    /* 输入捕获配置 */
    TIM2_ICInit(TIM2_CHANNEL_2,           //输入捕获通道2
                TIM2_ICPOLARITY_RISING,   //上升沿捕获
                TIM2_ICSELECTION_DIRECTTI,//捕获输入脚映射在：TI2FP2
                TIM2_ICPSC_DIV1,          //捕获预分频：1分频（1/2/4/8个事件触发一次）
                0x0a);                    //滤波频率(8/16)MHz，连续5个捕获电平信号后触发
    
    /* 中断配置 */
    TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);//计数器溢出时产生中断
    TIM2_ITConfig(TIM2_IT_CC2, ENABLE);   //允许TIM2_CH2捕获中断
    
    /* 使能计数器 */
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
    TIM4->ARR = 0x7D;  //【125*8us=1ms;设定定时时间】
    
    TIM4->PSCR = 0x07; //128分频
    TIM4->IER |= 0x01; //使能定时器中断
    TIM4->CR1 |= 0x01; //开始计时
}

/*******************************************************************************
         Function name:    ADC1_Config
	  Descriptions:    The ADC1 initial			
	    parameters:    void
        Returned value:    void							 
------------------------------------------------------------------------------*/
void ADC1_Config(void)
{
    ADC1_DeInit(); //复位ADC1
    ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE,//单次转换
                          ADC1_CHANNEL_4,      //选择AD转换通道:AIN4（Pt100）
                          ADC1_ALIGN_RIGHT);   //数据*对齐
    ADC1_PrescalerConfig(ADC1_PRESSEL_FCPU_D10);//ADC1转换时钟2MHz,主时钟8分频
    ADC1_ITConfig(ADC1_IT_EOCIE, ENABLE);      //使能转换结束中断
    ADC1_Cmd(ENABLE);      //使能AD转换
    ADC1_StartConversion();//启动AD转换
}

/*******************************************************************************
         Function name:    IWDG_Config
	  Descriptions:	   The IDWG initial
	    Parameters:	   void
        Returned value:    void
------------------------------------------------------------------------------*/
void IWDG_Config(void)
{
    /* 下两个顺序 */
    IWDG_Enable();       //使能内部看门狗
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//允许对受保护IWDG寄存器操作
    
    IWDG_SetPrescaler(IWDG_Prescaler_128);//128分频
    IWDG_SetReload(0xFF);//装载计数值(溢出时间510mS)
    IWDG_ReloadCounter();//更新看门狗
}

/*******************************************************************************
*	Function name:	      GPIO_Config
*	Descriptions:	      GPIO_Config
*	Parameters:	      void
*	Returned value:       void
*******************************************************************************/
/*
      stm8s105K4 共25个GPIO口
*/
void GPIO_Config(void)
{
    //引脚复位
    GPIO_DeInit(GPIOA);
    GPIO_DeInit(GPIOB);
    GPIO_DeInit(GPIOC);
    GPIO_DeInit(GPIOD);
    GPIO_DeInit(GPIOE);
    GPIO_DeInit(GPIOF);
    
    //PinA(2) 1 2
    GPIO_Init(GPIOA, GPIO_PIN_1, GPIO_MODE_OUT_OD_HIZ_SLOW);//未用端口（开漏）
    GPIO_Init(GPIOA, GPIO_PIN_2, GPIO_MODE_OUT_OD_HIZ_SLOW);//未用端口（开漏）
    
    //PinB(6) 0~3 4 5
    GPIO_Init(GPIOB, GPIO_PIN_0, GPIO_MODE_OUT_PP_HIGH_SLOW);//LCD_DATA
    GPIO_Init(GPIOB, GPIO_PIN_1, GPIO_MODE_OUT_PP_HIGH_SLOW);//LCD_CLK
    GPIO_Init(GPIOB, GPIO_PIN_2, GPIO_MODE_OUT_PP_HIGH_SLOW);//LCD_CS
    GPIO_Init(GPIOB, GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);     //ADC(Pt100)浮空输入
    GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);     //ADC(Pt100)浮空输入
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_SLOW);//蜂鸣器，推挽输出
    
    //PinC(7) 1~2 3~7
    GPIO_Init(GPIOC, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_SLOW);//PWM，推挽输出
    GPIO_Init(GPIOC, GPIO_PIN_2, GPIO_MODE_OUT_OD_HIZ_SLOW);//未用端口（开漏）
    GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT);//按键 上拉输入无中断
    GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT);//按键 上拉输入无中断
    GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT);//按键 上拉输入无中断
    GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT);//按键 上拉输入无中断
    GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_IN_PU_NO_IT);//按键 上拉输入无中断
    
    //PinD(8)  0~7
    GPIO_Init(GPIOD, GPIO_PIN_0, GPIO_MODE_OUT_OD_HIZ_SLOW); //未用端口（开漏）
    GPIO_Init(GPIOD, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_SLOW); //SWIM   推挽输出
    GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_OUT_OD_HIZ_SLOW); //未用端口（开漏）
    GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);     //霍尔  浮空输入
    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_OD_HIZ_SLOW); //未用端口（开漏）
    GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_SLOW); //未用端口（开漏）
    GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_OUT_PP_HIGH_SLOW);//报警，推挽输出
    GPIO_Init(GPIOD, GPIO_PIN_7, GPIO_MODE_OUT_PP_HIGH_SLOW);//加热，推挽输出
    
    //PinE(1) 5
    GPIO_Init(GPIOE, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_SLOW);//未用端口（开漏）
    
    //PinF(1) 4
    GPIO_Init(GPIOF, GPIO_PIN_4, GPIO_MODE_OUT_OD_HIZ_SLOW);//未用端口（开漏）
}