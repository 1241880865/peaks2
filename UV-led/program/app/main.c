/***********************************Copyright(c)********************************
**   Product Name:  UV-led-��
**       MCU Type�� STM32F103CBT6
** 		    
**       Designer:  SongHaihao
**   Company Name:  Peaks Measure&Control Tech Ltd
**           Date:  2017-07-25
**         Vision:  S1.0
--------------------------------------------------------------------------------
������������������������������������ ���ļ�¼ ����������������������������������
--------------------------------------------------------------------------------
**  ��������:  **  2017-07-25
**    �汾��:  **  S1.0
**  ��������:  **  LED�����ӻ���
--------------------------------------------------------------------------------
***********************************ͷ�ļ�����***********************************
------------------------------------------------------------------------------*/
#include "main.h"
#include "bsp_init.h"
#include "bsp_in.h"
#include "mid_flash.h"
#include "mid_out.h"

#include "mid_ad.h"
#include "UartCtrl.h"
#include "PWMCtrl.h"

//����
UART_PARA m_para;//������
RUNST* pm_st;    //����״̬
RUNOD* pm_od;    //��������
u8 gUartAlm;

/* ���ݷ�Χ */
RANGE_STRUCT gRngTab[12] =
{
	/****   --- 1 ----  --- 2 ----  --- 3 ----  --- 4 ----  --- 5 ---- */
	/*01*/  {0,	65535}, {0,	65535}, {0,	65535}, {0,	65535}, {0,	65535},
	/*06*/  {0,	65535}, {0,	65535}, {0,	65535}, {0,	65535}, {0,	65535},
	/*11*/  {0,	65535}, {0,	65535},
};

//ʱ�����
u16 TimMs_uartErr; //ͨѶͣ������ʱ���ʱ

//����
void BitBand(void);
void UartComm(void);
void UartProtect(void);
void PwmCtrl(void);
void AlmDeal(void);
/*******************************************************************************
** Function name:  main
** Descriptions:   ������
** input:          ��
** output:         ��
** Return:         ��
*******************************************************************************/
int main(void)
{
	RCC_Configuration();
	NVIC_Configuration();
	Systick_Configuration();
	GPIO_Configuration();
	DMA_Configuration((u16*)gAdBuff, 16*AD_NUM);
	ADC_Configuration();
	Tim_Configuration();
	USART_Configuration();
	//IWDG_Configuration();
	
	BitBand();
	DataFirstInit();
	
	while(1)
	{
		UartComm();
		UartProtect();
		
		AdCalc();
		AlmDeal();
		
		Input();
		PwmCtrl();
		Output();
		
		IWDG_ReloadCounter();
	}
}

/*******************************************************************************
** Function name:  BitBand
** Descriptions:   ����������������״̬ӳ�䵽λ����   1M-->32M
				   SRAM��ʼ��ַ        SRAM_BASE      0x20000000
				   λ��ӳ����ʼ��ַ    SRAM_BB_BASE   0x22000000
** input:          ��
** output:         ��
** Return:         ��
*******************************************************************************/
void BitBand(void)
{
	u32 p;
	
	m_para.status1 = 0;
	m_para.status2 = 0;
	
	p = (u32)(&(m_para.status1));
	pm_st = (RUNST*)(SRAM_BB_BASE + ((p - SRAM_BASE) << 5)); //0bit
	
	p = (u32)(&(m_para.order1));
	pm_od = (RUNOD*)(SRAM_BB_BASE + ((p - SRAM_BASE) << 5)); //0bit
}

/*******************************************************************************
** Function name:  AlmDeal
** Descriptions:   ��������
** input:          ��
** output:         ��
** Return:         ��
*******************************************************************************/
void AlmDeal(void)
{
	//��·����
	GetOpenCircuit();
	
	//���±���
	if((m_para.ntc1 >= m_para.ntcAlm) || (m_para.ntc2 >= m_para.ntcAlm))
	{
		pm_st->ntcAlm = 1;
	}
	else
	{
		pm_st->ntcAlm = 0;
	}
}

/*******************************************************************************
** Function name:  PwmCtrl
** Descriptions:   PWM�������
** input:          ��
** output:         ��
** Return:         ��
*******************************************************************************/
void PwmCtrl(void)
{
	u8 onoff;
	
	//����ռ�ձ�
	CalcDutyWeb();
	
	//�㶯ģʽ
	if(m_para.trigMd == 1)
	{
		onoff = pm_st->switchOn; //�ⲿ���ش���
	}
	else
	{
		onoff = pm_od->devOnOff; //��λ������
	}
	
	//pwm���
	if((onoff == 0)                     || //����Ϊ0
	   ((m_para.status1 & 0x4fff) != 0) || //12·��·���� ���±���
	   (m_para.almOd != 0)              || //�ܱ���״̬
	   (gUartAlm != 0))                    //ͨѶ�Ͽ�����
	{
		PwmAllOff();
	}
	else
	{
		PwmOutOn();
	}
}

/*******************************************************************************
** Function name:  UartProtect
** Descriptions:   4s����ͨѶ��ͣ��
** input:          ��
** output:         ��
** Return:         ��
*******************************************************************************/
void UartProtect(void)
{
    if(TimMs_uartErr >= 4000) //5s
    {
        TimMs_uartErr = 4000;
        
        m_para.order2 = 0; //ͣ������
		gUartAlm = 1;
    }
}

/*******************************************************************************
** Function name:  LedAllCtrl
** Descriptions:   Led�������
** input:          ��
** output:         ��
** Return:         ��
*******************************************************************************/
void LedAllCtrl(UART_PARA* od_all)
{
	if((od_all->order2 & 0x0004) != 0) //ȫ������
	{
		od_all->order2 &= 0xfffB; //��ȫ������
		
		od_all->order1 &= 0xf000;
		
		WrToFlash();
	}
	else if((od_all->order2 & 0x0002) != 0) //ȫ������
	{
		od_all->order2 &= 0xfffD; //��ȫ������
		
		od_all->order1 |= 0x0fff;
		
		WrToFlash();
	}
}

/*******************************************************************************
** Function name:  UartComm
** Descriptions:   ����ͨѶ������
** input:          ��
** output:         ��
** Return:         ��
*******************************************************************************/
u8 isNeedSave(u16 offset)
{
	u8 flg = 0;
	
	if((offset == 0x00) || (offset == 0x01) || (offset == 0x04) || (offset == 0x08) ||
	   (offset == 0x0A))
	{
		flg = 1;
	}
	
	return(flg);
}

void UartComm(void)
{
	u8  uart_order;//ͨѶ�������
	u16 offset;
	
	//��ʱ��λ
	ModbusClear();
	
	/* ������֤�Ƿ�Ϊ��Чֵ */
	uart_order = UartTest();
	if(uart_order == 0xff) //����0xffΪ��Чֵ��δ�н���
	{
		return;
	}
	
	/* ������Ч�������� */
	switch(uart_order)
	{
		case DATA_CODE_READ://������
			RdData(&m_para);//03
		break;
		
		case DATA_CODE_SWRITE://д��������
			offset = WrDataSgl(&m_para, gRngTab);
			if(isNeedSave(offset) != 0) //06
			{
				//flash����
				WrToFlash();
			}
			//ȫ�� ȫ��
			LedAllCtrl(&m_para);
		break;
		
		default:
		break;
	}
}





#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
  while (1)
  {
  }
}
#endif

/*******************************************************************************
-------------------------------------- END -------------------------------------
*******************************************************************************/
