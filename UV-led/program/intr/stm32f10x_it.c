/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "main.h"
#include "mid_ad.h"
#include "UartCtrl.h"
#include "mid_out.h"
#include "bsp_in.h"

static u16 TimMs_Base;
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/*******************************************************************************
** Function name:       DMA1_Channel1_IRQHandler
** Descriptions:        DMA1_Channel1
** input parameters:    None
** output parameters:   None
** Returned value:      None
*******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1))//通道1传输完成中断
	{
		ADC_Cmd(ADC1, DISABLE);  //关闭AD
		
		DMA_ClearITPendingBit(DMA1_IT_GL1);	//清除全部中断标志
		gAdOk = 1;
	}
}

/*******************************************************************************
** Function name:       SysTick_Handler
** Descriptions:        SysTick
** input parameters:    None
** output parameters:   None
** Returned value:      None
*******************************************************************************/
void SysTick_Handler(void)
{
	TimMs_uartErr++;
	TimMs_ledFlash++;
	
	TimMs_switch++;
	TimMs_open1++;
	TimMs_open2++;
	TimMs_open3++;
	TimMs_open4++;
	TimMs_openTab++;
	TimMs_openDly++;
	TimMs_openKeep++;
	
	TimMs_ModbusWait++;
	TimMs_ModbusClear++;
	
	TimMs_ledFlash++;
	
	TimMs_Base++;
	if(TimMs_Base >= 1000)
	{
		TimMs_Base = 0;
	}
}

/*******************************************************************************
** Function name:       USART1_IRQHandler
** Descriptions:        USART1
** input parameters:    None
** output parameters:   None
** Returned value:      None
*******************************************************************************/
void USART1_IRQHandler(void)
{
    u8 buff;
		
	FlagStatus Status;
	Status = USART_GetFlagStatus(USART1, USART_FLAG_PE);//检查指定的USART标志位设置与否
	if(Status == SET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);  //清除UART接收中断标志位
		buff = USART_ReceiveData(USART1); //接收一个字节数据
		Flag_RcvStep = 0;
		return;
	}
	
	TimMs_uartErr = 0; //通讯计时清零
	gUartAlm = 0;      //有通讯
    
    //处理接收到的数据
    while(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        LED2ON();
        buff = USART_ReceiveData(USART1); //接收一个字节数据
        
        switch(Flag_RcvStep)
        {
            case 0://节点地址
                RcvBuff[0] = buff;
                if(((buff == BROADCAST_ADDR) || (buff == m_para.add)) &&\
                   (TimMs_ModbusWait >= 3)) //设备地址*/
                    Flag_RcvStep = 1;
                else
                    Flag_RcvStep = 0;
                break;
                
            case 1://功能码
                RcvBuff[1] = buff;
                RcvCount = 2;
                
                if(buff==0x03) {RcvLength=8; Flag_RcvStep=2;}     //读数据
                else if(buff==0x06) {RcvLength=8; Flag_RcvStep=2;}//写单个数据
                break;
                
            case 2://读命令和写单个命令
                RcvBuff[RcvCount++] = buff;
                if(RcvCount==RcvLength) {Flag_RcvStep = 5;}
                else if(RcvCount>RcvLength) Flag_RcvStep = 0;
                break;
                
            case 5://等待处理
                break;
                
            case 6://溢出处理
                break;
                
            default:
                Flag_RcvStep = 6;
                break;
        }
        LED2OFF();
        TimMs_ModbusWait = 0;
    }
    
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);  //清除UART接收中断标志位
}

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/
/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
