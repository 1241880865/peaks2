


#ifndef __DRIVER_H
#define __DRIVER_H

/*------------------------------------------------------------------------------
*********************************** 函数声明 ***********************************
------------------------------------------------------------------------------*/
void Delay_Ms(u16 xms);  //上电初始化不精确延时

void CLK_Config(void);   //时钟初始化
void GPIO_Config(void);  //GPIO初始化
void TIM1_Config(void);  //TIM1初始化
void TIM2_Config(void);  //TIM2初始化
void TIM4_Config(void);  //TIM4初始化
void ADC1_Config(void);  //ADC1初始化
void IWDG_Config(void);  //IWDG初始化


#endif


