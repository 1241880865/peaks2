
							 

/* include-----------------------------------------------------------------*/
#include "main.h"
#include "bsp_init.h"


static DMA_InitTypeDef ADDMA_InitStructure;

static void GPIO_PWM_Config(void);
static void GPIO_UART1_Config(void);
static void GPIO_AD_Config(void);
static void GPIO_InOut_Config(void);
/*******************************************************************************
* ������ 		: RCC_Config
* ��������  	: ����ϵͳ������ʱ��
* �������  	: ��
* ������  	: ��
* ����ֵ    	: ��
*******************************************************************************/
void RCC_Configuration(void)
{
    /* ����ö�����ͱ��� HSEStartUpStatus */
    ErrorStatus HSEStartUpStatus;
    
    /* ��λϵͳʱ������*/
    RCC_DeInit();
    /* ����HSE*/
    RCC_HSEConfig(RCC_HSE_ON);
    /* �ȴ�HSE�����ȶ�*/
    HSEStartUpStatus = RCC_WaitForHSEStartUp();
    /* �ж�HSE���Ƿ���ɹ����������if()�ڲ� */
    if(HSEStartUpStatus == SUCCESS)
    {
		/* ѡ��HCLK��AHB��ʱ��ԴΪSYSCLK 1��Ƶ */
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		/* ѡ��PCLK2ʱ��ԴΪ HCLK��AHB-APB2�� 1��Ƶ */
		RCC_PCLK2Config(RCC_HCLK_Div1);
		/* ѡ��PCLK1ʱ��ԴΪ HCLK��AHB-APB1�� 1��Ƶ */
		RCC_PCLK1Config(RCC_HCLK_Div1);
		/* ����FLASH��ʱ������Ϊ0(��Ƶ <24MHz,0������; <48MHz,1������; <72MHz,2������) changed by song 2015-07-17 */
		FLASH_SetLatency(FLASH_Latency_0);
		/* ʹ��FLASHԤȡ���� */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		/* ѡ�����໷��PLL��ʱ��ԴΪHSE 1��Ƶ����Ƶ��Ϊ2����PLL���Ƶ��Ϊ 8MHz * 2 = 16MHz*/
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_2);
		/* ʹ��PLL */
		RCC_PLLCmd(ENABLE);
		/* �ȴ�PLL����ȶ� */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
		/* ѡ��SYSCLKʱ��ԴΪPLL */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		/* �ȴ�PLL��ΪSYSCLKʱ��Դ */
		while(RCC_GetSYSCLKSource() != 0x08);
    }
	/* ����ADCʱ�ӷ�Ƶ */
	RCC_ADCCLKConfig(RCC_PCLK2_Div2); //8MHz

	/* ������ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 |\
						   RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_ADC1 |\
						   RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | \
						   RCC_APB2Periph_AFIO, ENABLE);
	/* ��DMAʱ�� */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* ���� HSI */
	RCC_HSICmd(ENABLE);
}

/*******************************************************************************
* ������  		  : GPIO_Config
* ��������    	: ���ø�GPIO�˿ڹ���
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void GPIO_Configuration(void)
{
	//��λ
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	
	//12·PWM���
	GPIO_PWM_Config();
	
	//UART1ͨѶ
	GPIO_UART1_Config();
	
	//AD���� 15·AD�л�
	GPIO_AD_Config();
	
	//�������������LED���̵���
	GPIO_InOut_Config();
}

void GPIO_PWM_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |\
                                  GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //����TIM
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 |\
                                  GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //����TIM
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void GPIO_UART1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
		
	  /* ����USART1��Tx�ţ�PA.9��Ϊ�ڶ������������ģʽ */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* ����USART1��Rx�ţ�PA.10��Ϊ��������� */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
  	/* ����PA12��Ϊ���������UART1����������*/
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void GPIO_AD_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
					 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void GPIO_InOut_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE); //�ر�jtag���Կ�����ͨ�ڣ�����swdģʽ

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//���ӿ���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //LED
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//JD
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB, GPIO_Pin_4); //��·�ƹر�
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
}

/*******************************************************************************
* ������  		: Tim_Config
* ��������    	: ��ʱ��Pwm����
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void Tim_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	/* ��λTIM */
	TIM_DeInit(TIM2);
	TIM_DeInit(TIM3);
	TIM_DeInit(TIM4);
	
	/* TIMʱ�����ã�Ҫ����װ��ֵΪ100��f��Ϊ1KHz;
		 100 * ��Ƶ/f�� = 1/f��;
	   ����Ԥ��Ƶֵ = f��/(100*f��);
	*/
	TIM_TimeBaseStructure.TIM_Prescaler = 0x4F;//Ԥ��Ƶֵ80  (changed 2015-06-24 PWM-2K)
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period = 0x63;   //�Զ���װ�ؼĴ������ڵ�ֵ100 ***************
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x01;//�ظ�����ֵ
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);//��ʼ��TIM2
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//��ʼ��TIM3
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//��ʼ��TIM4
	
	/* TIM���PWM���� */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWMģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0;//������,���������ռ�ձ�
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//�ߵ�ƽ��Ч
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;//�͵�ƽ
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);//��ʼ��TIM2
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);
	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
	TIM_OC4Init(TIM2,&TIM_OCInitStructure);
	TIM_OC1Init(TIM3,&TIM_OCInitStructure);//��ʼ��TIM3
	TIM_OC2Init(TIM3,&TIM_OCInitStructure);
	TIM_OC3Init(TIM3,&TIM_OCInitStructure);
	TIM_OC4Init(TIM3,&TIM_OCInitStructure);
	TIM_OC1Init(TIM4,&TIM_OCInitStructure);//��ʼ��TIM4
	TIM_OC2Init(TIM4,&TIM_OCInitStructure);
	TIM_OC3Init(TIM4,&TIM_OCInitStructure);
	TIM_OC4Init(TIM4,&TIM_OCInitStructure);
	
	/* ʹ��Ԥװ�ؼĴ��� */
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);//ʹ��TIMx��CCRx�Ĵ���Ԥװ�ع���
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
	TIM_ARRPreloadConfig(TIM2, ENABLE);//ʹ��TIMx��ARR�Ĵ���Ԥװ�ع���
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	
	/* ���ʹ�� */
	TIM_Cmd(TIM2, ENABLE); //���ʹ��TIM2
	TIM_Cmd(TIM3, ENABLE); //���ʹ��TIM3
	TIM_Cmd(TIM4, ENABLE); //���ʹ��TIM4
}

/*******************************************************************************
* ������  		: USART_Configuration
* ��������    	: ����USART1
* �������      : None
* ������      : None
* ����ֵ        : None
*******************************************************************************/
void USART_Configuration(void)
{
	/* ���� USART ��ʼ���ṹ�� USART_InitStructure */
	USART_InitTypeDef USART_InitStructure;

	/*
	* ������Ϊ9600bps;
	*	8λ���ݳ���;
	*	1��ֹͣλ
	*	żУ��;
	*	����Ӳ��������;
	*	��ֹUSARTʱ��;
	*	ʱ�Ӽ��Ե�;
	*	�ڵ�2�����ز�������
	*	���һλ���ݵ�ʱ�����岻�� SCLK �����
	*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_Cmd(USART1, ENABLE);/*ʹ��USART1*/
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);/*�����ж�ʹ��*/
}

/*******************************************************************************
* ������  		: NVIC_Config
* ��������    	: �жϷ���
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void NVIC_Configuration(void)
{
    /* ����NVIC��ʼ���ṹ�� NVIC_InitStructure */
    NVIC_InitTypeDef NVIC_InitStructure;
  
    /* #ifdef...#else...#endif�ṹ�������Ǹ���Ԥ�������������ж���������ʼ��ַ*/   
#ifdef  VECT_TAB_RAM
    /* �ж���������ʼ��ַ�� 0x20000000 ��ʼ */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else /* VECT_TAB_FLASH */
    /* �ж���������ʼ��ַ�� 0x80000000 ��ʼ */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
    /* ѡ�����ȼ�����0  */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
		
	/* ʹ�� DMA1_Channel1 �ж� */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/* ʹ�� UART1 ���� */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
 
/******************************************************************************
* ������  		: Systick_Config
* ��������    	: ����Systick��ʱ��,��װ��ʱ��Ϊ1ms
* �������      : ��
* ������      : ��
* ����ֵ        : ��
******************************************************************************/
void Systick_Configuration(void)
{
	if(SysTick_Config(16000000 / 1000)) //SystemCoreClock��װֵ��/1000��Ϊ1ms SystemCoreClock
		while(1);
}

/******************************************************************************
* ������  		: DMA_Config
* ��������    	: ����DMA����
* �������      : ��
* ������      : ��
* ����ֵ        : ��
******************************************************************************/
void DMA_Configuration(u16* base_add, u16 size)
{
	/* DMAͨ��1*/
	DMA_DeInit(DMA1_Channel1);
	//ָ��DMA�������ַ
	ADDMA_InitStructure.DMA_PeripheralBaseAddr =(u32)(&(ADC1->DR));         //ADC1���ݼĴ���
	//�趨DMA�ڴ����ַ
	ADDMA_InitStructure.DMA_MemoryBaseAddr = (u32)base_add;                 //��ȡADC������
	//������Ϊ���ݴ������Դ
	ADDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                    //Ƭ��������Դͷ
	//ָ��DMAͨ����DMA�����С
	ADDMA_InitStructure.DMA_BufferSize = size;                              //ÿ��DMA16������
	//�����ַ�����������䣩
	ADDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;      //�����ַ������
	//�ڴ��ַ�����������䣩
	ADDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;               //�ڴ��ַ����
	//�趨�������ݿ��Ϊ16λ
	ADDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //����
	ADDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;          //����
	//�趨DMA�Ĺ���ģʽ��ͨģʽ������һ����ѭ��ģʽ
	ADDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                         //��ͨģʽ
	//�趨DMAͨ����������ȼ�
	ADDMA_InitStructure.DMA_Priority = DMA_Priority_High;                   //�����ȼ�
	//ʹ��DMA�ڴ浽�ڴ�Ĵ��䣬�˴�û���ڴ浽�ڴ�Ĵ���
	ADDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                          //���ڴ浽�ڴ�
	
	DMA_Init(DMA1_Channel1, &ADDMA_InitStructure);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);                         //DMAͨ��1��������ж�
}

/*********************************************************************************************************
** Function name:       DMAReConfig
** Descriptions:        ��������DMA
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
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
* ������  		: ADC_Config
* ��������    	: ��ʼ��������ADCת��
* �������      : ��
* ������      : ��
* ����ֵ        : ��
*******************************************************************************/
void ADC_Configuration(void)
{
	/* ���� ADC ��ʼ���ṹ�� ADC_InitStructure */
	ADC_InitTypeDef ADC_InitStructure;
	
	/* ADC1 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					//����ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;						//��ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;					//����ɨ��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//�������ת��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				//�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 2;								//2��ͨ��
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* ���� ADC1 ʹ��2ת��ͨ����ת��˳��1������ʱ��Ϊ 239.5 ���� */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_239Cycles5);//NTC_1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 2, ADC_SampleTime_239Cycles5);//NTC_2
	
	/* ����ADC1��DMAģʽ */
	ADC_DMACmd(ADC1, ENABLE);
	
	/* ����ADC1*/
	ADC_Cmd(ADC1, ENABLE);
	
	/*����У׼�Ĵ��� */
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	/*��ʼУ׼״̬*/
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

 /*******************************************************************************
 * ������  		 : IWDG_Configuration
 * ��������      : ����IWDG����ʱʱ��Ϊ350ms
 * �������      : ��
 * ������      : ��
 * ����ֵ        : ��
 *******************************************************************************/
 void IWDG_Configuration(void)
 {
   	/* ʹ�ܶԼĴ��� IWDG_PR �� IWDG_RLR ��д���� */
   	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
   	/* ���� IWDG ʱ��Ϊ LSI ��32��Ƶ����ʱ IWDG ������ʱ�� = 32KHz(LSI) / 32 = 1KHz */
   	IWDG_SetPrescaler(IWDG_Prescaler_32);
	
   	/* ���� IWDG ����ֵΪ349 */
   	IWDG_SetReload(1000);
	
   	/* ���� IWDG ����ֵ */
   	IWDG_ReloadCounter();
	
   	/* ���� IWDG */
   	IWDG_Enable();	
 }

/*******************************************************************************
-------------------------------------- END -------------------------------------
*******************************************************************************/
