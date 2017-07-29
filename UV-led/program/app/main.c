/***********************************Copyright(c)********************************
**   Product Name:  UV-led-从
**       MCU Type： STM32F103CBT6
** 		    
**       Designer:  SongHaihao
**   Company Name:  Peaks Measure&Control Tech Ltd
**           Date:  2017-07-25
**         Vision:  S1.0
--------------------------------------------------------------------------------
～～～～～～～～～～～～～～～～～～ 更改记录 ～～～～～～～～～～～～～～～～～
--------------------------------------------------------------------------------
**  创建日期:  **  2017-07-25
**    版本号:  **  S1.0
**  功能描述:  **  LED驱动从机板
--------------------------------------------------------------------------------
***********************************头文件包含***********************************
------------------------------------------------------------------------------*/
#include "main.h"
#include "bsp_init.h"
#include "bsp_in.h"
#include "mid_flash.h"
#include "mid_out.h"

#include "mid_ad.h"
#include "UartCtrl.h"
#include "PWMCtrl.h"

//参数
UART_PARA m_para;//参数表
RUNST* pm_st;    //运行状态
RUNOD* pm_od;    //控制命令
u8 gUartAlm;

/* 数据范围 */
RANGE_STRUCT gRngTab[12] =
{
	/****   --- 1 ----  --- 2 ----  --- 3 ----  --- 4 ----  --- 5 ---- */
	/*01*/  {0,	65535}, {0,	65535}, {0,	65535}, {0,	65535}, {0,	65535},
	/*06*/  {0,	65535}, {0,	65535}, {0,	65535}, {0,	65535}, {0,	65535},
	/*11*/  {0,	65535}, {0,	65535},
};

//时间变量
u16 TimMs_uartErr; //通讯停机保护时间计时

//函数
void BitBand(void);
void UartComm(void);
void UartProtect(void);
void PwmCtrl(void);
void AlmDeal(void);
/*******************************************************************************
** Function name:  main
** Descriptions:   主函数
** input:          无
** output:         无
** Return:         无
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
** Descriptions:   将控制命令与运行状态映射到位段区   1M-->32M
				   SRAM起始地址        SRAM_BASE      0x20000000
				   位段映射起始地址    SRAM_BB_BASE   0x22000000
** input:          无
** output:         无
** Return:         无
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
** Descriptions:   报警处理
** input:          无
** output:         无
** Return:         无
*******************************************************************************/
void AlmDeal(void)
{
	//断路报警
	GetOpenCircuit();
	
	//超温报警
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
** Descriptions:   PWM输出控制
** input:          无
** output:         无
** Return:         无
*******************************************************************************/
void PwmCtrl(void)
{
	u8 onoff;
	
	//计算占空比
	CalcDutyWeb();
	
	//点动模式
	if(m_para.trigMd == 1)
	{
		onoff = pm_st->switchOn; //外部开关触发
	}
	else
	{
		onoff = pm_od->devOnOff; //上位机开关
	}
	
	//pwm输出
	if((onoff == 0)                     || //开关为0
	   ((m_para.status1 & 0x4fff) != 0) || //12路断路报警 超温报警
	   (m_para.almOd != 0)              || //总报警状态
	   (gUartAlm != 0))                    //通讯断开报警
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
** Descriptions:   4s钟无通讯，停机
** input:          无
** output:         无
** Return:         无
*******************************************************************************/
void UartProtect(void)
{
    if(TimMs_uartErr >= 4000) //5s
    {
        TimMs_uartErr = 4000;
        
        m_para.order2 = 0; //停机保护
		gUartAlm = 1;
    }
}

/*******************************************************************************
** Function name:  LedAllCtrl
** Descriptions:   Led集体控制
** input:          无
** output:         无
** Return:         无
*******************************************************************************/
void LedAllCtrl(UART_PARA* od_all)
{
	if((od_all->order2 & 0x0004) != 0) //全关命令
	{
		od_all->order2 &= 0xfffB; //清全关命令
		
		od_all->order1 &= 0xf000;
		
		WrToFlash();
	}
	else if((od_all->order2 & 0x0002) != 0) //全开命令
	{
		od_all->order2 &= 0xfffD; //清全开命令
		
		od_all->order1 |= 0x0fff;
		
		WrToFlash();
	}
}

/*******************************************************************************
** Function name:  UartComm
** Descriptions:   串口通讯主程序
** input:          无
** output:         无
** Return:         无
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
	u8  uart_order;//通讯命令功能码
	u16 offset;
	
	//延时复位
	ModbusClear();
	
	/* 数据验证是否为有效值 */
	uart_order = UartTest();
	if(uart_order == 0xff) //返回0xff为无效值或未有接收
	{
		return;
	}
	
	/* 解析有效接收数据 */
	switch(uart_order)
	{
		case DATA_CODE_READ://读数据
			RdData(&m_para);//03
		break;
		
		case DATA_CODE_SWRITE://写单个数据
			offset = WrDataSgl(&m_para, gRngTab);
			if(isNeedSave(offset) != 0) //06
			{
				//flash保存
				WrToFlash();
			}
			//全开 全关
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
