
							 

/* include-----------------------------------------------------------------*/
#include "main.h"
#include "bsp_init.h"


static DMA_InitTypeDef ADDMA_InitStructure;

static void GPIO_PWM_Config(void);
static void GPIO_UART1_Config(void);
static void GPIO_AD_Config(void);
static void GPIO_InOut_Config(void);
/*******************************************************************************
* 函数名 		: RCC_Config
* 函数描述  	: 设置系统各部分时钟
* 输入参数  	: 无
* 输出结果  	: 无
* 返回值    	: 无
*******************************************************************************/
void RCC_Configuration(void)
{
    /* 定义枚举类型变量 HSEStartUpStatus */
    ErrorStatus HSEStartUpStatus;
    
    /* 复位系统时钟设置*/
    RCC_DeInit();
    /* 开启HSE*/
    RCC_HSEConfig(RCC_HSE_ON);
    /* 等待HSE起振并稳定*/
    HSEStartUpStatus = RCC_WaitForHSEStartUp();
    /* 判断HSE起是否振成功，是则进入if()内部 */
    if(HSEStartUpStatus == SUCCESS)
    {
		/* 选择HCLK（AHB）时钟源为SYSCLK 1分频 */
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		/* 选择PCLK2时钟源为 HCLK（AHB-APB2） 1分频 */
		RCC_PCLK2Config(RCC_HCLK_Div1);
		/* 选择PCLK1时钟源为 HCLK（AHB-APB1） 1分频 */
		RCC_PCLK1Config(RCC_HCLK_Div1);
		/* 设置FLASH延时周期数为0(主频 <24MHz,0个周期; <48MHz,1个周期; <72MHz,2个周期) changed by song 2015-07-17 */
		FLASH_SetLatency(FLASH_Latency_0);
		/* 使能FLASH预取缓存 */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		/* 选择锁相环（PLL）时钟源为HSE 1分频，倍频数为2，则PLL输出频率为 8MHz * 2 = 16MHz*/
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_2);
		/* 使能PLL */
		RCC_PLLCmd(ENABLE);
		/* 等待PLL输出稳定 */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
		/* 选择SYSCLK时钟源为PLL */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		/* 等待PLL成为SYSCLK时钟源 */
		while(RCC_GetSYSCLKSource() != 0x08);
    }
	/* 配置ADC时钟分频 */
	RCC_ADCCLKConfig(RCC_PCLK2_Div2); //8MHz

	/* 开外设时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 |\
						   RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_ADC1 |\
						   RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | \
						   RCC_APB2Periph_AFIO, ENABLE);
	/* 开DMA时钟 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* 开启 HSI */
	RCC_HSICmd(ENABLE);
}

/*******************************************************************************
* 函数名  		  : GPIO_Config
* 函数描述    	: 设置各GPIO端口功能
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void GPIO_Configuration(void)
{
	//复位
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	
	//12路PWM输出
	GPIO_PWM_Config();
	
	//UART1通讯
	GPIO_UART1_Config();
	
	//AD采样 15路AD切换
	GPIO_AD_Config();
	
	//配置输入输出，LED，继电器
	GPIO_InOut_Config();
}

void GPIO_PWM_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |\
                                  GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用TIM
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 |\
                                  GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用TIM
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void GPIO_UART1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
		
	  /* 设置USART1的Tx脚（PA.9）为第二功能推挽输出模式 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 设置USART1的Rx脚（PA.10）为上拉输入脚 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
  	/* 设置PA12口为推挽输出，UART1数据流控制*/
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void GPIO_AD_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
					 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void GPIO_InOut_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE); //关闭jtag调试口作普通口，保留swd模式

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//电子开关
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //LED
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//JD
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB, GPIO_Pin_4); //两路灯关闭
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
}

/*******************************************************************************
* 函数名  		: Tim_Config
* 函数描述    	: 定时器Pwm配置
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void Tim_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	/* 复位TIM */
	TIM_DeInit(TIM2);
	TIM_DeInit(TIM3);
	TIM_DeInit(TIM4);
	
	/* TIM时基配置，要求重装载值为100，f出为1KHz;
		 100 * 分频/f主 = 1/f出;
	   即：预分频值 = f主/(100*f出);
	*/
	TIM_TimeBaseStructure.TIM_Prescaler = 0x4F;//预分频值80  (changed 2015-06-24 PWM-2K)
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseStructure.TIM_Period = 0x63;   //自动重装载寄存器周期的值100 ***************
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x01;//重复计数值
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);//初始化TIM2
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//初始化TIM3
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//初始化TIM4
	
	/* TIM输出PWM配置 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//使能
	TIM_OCInitStructure.TIM_Pulse = 0;//脉冲宽度,由这个设置占空比
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//高电平有效
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;//低电平
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);//初始化TIM2
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);
	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
	TIM_OC4Init(TIM2,&TIM_OCInitStructure);
	TIM_OC1Init(TIM3,&TIM_OCInitStructure);//初始化TIM3
	TIM_OC2Init(TIM3,&TIM_OCInitStructure);
	TIM_OC3Init(TIM3,&TIM_OCInitStructure);
	TIM_OC4Init(TIM3,&TIM_OCInitStructure);
	TIM_OC1Init(TIM4,&TIM_OCInitStructure);//初始化TIM4
	TIM_OC2Init(TIM4,&TIM_OCInitStructure);
	TIM_OC3Init(TIM4,&TIM_OCInitStructure);
	TIM_OC4Init(TIM4,&TIM_OCInitStructure);
	
	/* 使能预装载寄存器 */
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);//使能TIMx在CCRx寄存器预装载功能
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2, ENABLE);//使能TIMx在ARR寄存器预装载功能
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	
	/* 输出使能 */
	TIM_Cmd(TIM2, ENABLE); //输出使能TIM2
	TIM_Cmd(TIM3, ENABLE); //输出使能TIM3
	TIM_Cmd(TIM4, ENABLE); //输出使能TIM4
}

/*******************************************************************************
* 函数名  		: USART_Configuration
* 函数描述    	: 设置USART1
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void USART_Configuration(void)
{
	/* 定义 USART 初始化结构体 USART_InitStructure */
	USART_InitTypeDef USART_InitStructure;

	/*
	* 波特率为9600bps;
	*	8位数据长度;
	*	1个停止位
	*	偶校验;
	*	禁用硬件流控制;
	*	禁止USART时钟;
	*	时钟极性低;
	*	在第2个边沿捕获数据
	*	最后一位数据的时钟脉冲不从 SCLK 输出；
	*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_Cmd(USART1, ENABLE);/*使能USART1*/
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);/*接收中断使能*/
}

/*******************************************************************************
* 函数名  		: NVIC_Config
* 函数描述    	: 中断分组
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void NVIC_Configuration(void)
{
    /* 定义NVIC初始化结构体 NVIC_InitStructure */
    NVIC_InitTypeDef NVIC_InitStructure;
  
    /* #ifdef...#else...#endif结构的作用是根据预编译条件决定中断向量表起始地址*/   
#ifdef  VECT_TAB_RAM
    /* 中断向量表起始地址从 0x20000000 开始 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else /* VECT_TAB_FLASH */
    /* 中断向量表起始地址从 0x80000000 开始 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
    /* 选择优先级分组0  */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
		
	/* 使能 DMA1_Channel1 中断 */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/* 使能 UART1 设置 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
 
/******************************************************************************
* 函数名  		: Systick_Config
* 函数描述    	: 设置Systick定时器,重装载时间为1ms
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
******************************************************************************/
void Systick_Configuration(void)
{
	if(SysTick_Config(16000000 / 1000)) //SystemCoreClock重装值，/1000即为1ms SystemCoreClock
		while(1);
}

/******************************************************************************
* 函数名  		: DMA_Config
* 函数描述    	: 设置DMA参数
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
******************************************************************************/
void DMA_Configuration(u16* base_add, u16 size)
{
	/* DMA通道1*/
	DMA_DeInit(DMA1_Channel1);
	//指定DMA外设基地址
	ADDMA_InitStructure.DMA_PeripheralBaseAddr =(u32)(&(ADC1->DR));         //ADC1数据寄存器
	//设定DMA内存基地址
	ADDMA_InitStructure.DMA_MemoryBaseAddr = (u32)base_add;                 //获取ADC的数组
	//外设作为数据传输的来源
	ADDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                    //片内外设作源头
	//指定DMA通道的DMA缓存大小
	ADDMA_InitStructure.DMA_BufferSize = size;                              //每次DMA16个数据
	//外设地址不递增（不变）
	ADDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;      //外设地址不增加
	//内存地址不递增（不变）
	ADDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;               //内存地址增加
	//设定外设数据宽度为16位
	ADDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //半字
	ADDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;          //半字
	//设定DMA的工作模式普通模式，还有一种是循环模式
	ADDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                         //普通模式
	//设定DMA通道的软件优先级
	ADDMA_InitStructure.DMA_Priority = DMA_Priority_High;                   //高优先级
	//使能DMA内存到内存的传输，此处没有内存到内存的传输
	ADDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                          //非内存到内存
	
	DMA_Init(DMA1_Channel1, &ADDMA_InitStructure);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);                         //DMA通道1传输完成中断
}

/*********************************************************************************************************
** Function name:       DMAReConfig
** Descriptions:        重新允许DMA
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DMA_ReStart(void)
{
	DMA_DeInit(DMA1_Channel1);
	DMA_Init(DMA1_Channel1, &ADDMA_InitStructure);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

/*******************************************************************************
* 函数名  		: ADC_Config
* 函数描述    	: 初始化并启动ADC转换
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/
void ADC_Configuration(void)
{
	/* 定义 ADC 初始化结构体 ADC_InitStructure */
	ADC_InitTypeDef ADC_InitStructure;
	
	/* ADC1 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					//独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;						//多通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;					//连续扫描
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//软件启动转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				//数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 2;								//2个通道
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* 设置 ADC1 使用2转换通道，转换顺序1，采样时间为 239.5 周期 */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_239Cycles5);//NTC_1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 2, ADC_SampleTime_239Cycles5);//NTC_2
	
	/* 允许ADC1的DMA模式 */
	ADC_DMACmd(ADC1, ENABLE);
	
	/* 允许ADC1*/
	ADC_Cmd(ADC1, ENABLE);
	
	/*重置校准寄存器 */
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	/*开始校准状态*/
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

 /*******************************************************************************
 * 函数名  		 : IWDG_Configuration
 * 函数描述      : 设置IWDG，超时时间为350ms
 * 输入参数      : 无
 * 输出结果      : 无
 * 返回值        : 无
 *******************************************************************************/
 void IWDG_Configuration(void)
 {
   	/* 使能对寄存器 IWDG_PR 和 IWDG_RLR 的写操作 */
   	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
   	/* 设置 IWDG 时钟为 LSI 经32分频，此时 IWDG 计数器时钟 = 32KHz(LSI) / 32 = 1KHz */
   	IWDG_SetPrescaler(IWDG_Prescaler_32);
	
   	/* 设置 IWDG 计数值为349 */
   	IWDG_SetReload(1000);
	
   	/* 重载 IWDG 计数值 */
   	IWDG_ReloadCounter();
	
   	/* 启动 IWDG */
   	IWDG_Enable();	
 }

/*******************************************************************************
-------------------------------------- END -------------------------------------
*******************************************************************************/
