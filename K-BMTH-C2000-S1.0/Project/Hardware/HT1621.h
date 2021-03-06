


/*******************************************************************************
                                 HT1621 驱动
*******************************************************************************/
#ifndef __HT1621_H
#define __HT1621_H

/*------------------------------------------------------------------------------
***************************** stm8接口1621 宏定义  *****************************
------------------------------------------------------------------------------*/
//液晶屏控制宏定义
#define LCD_CS_LOW      GPIO_WriteLow(GPIOB, GPIO_PIN_2);  //片选低
#define LCD_CS_HIGH     GPIO_WriteHigh(GPIOB, GPIO_PIN_2); //片选高
#define LCD_CLK_LOW     GPIO_WriteLow(GPIOB, GPIO_PIN_1);  //时钟低
#define LCD_CLK_HIGH    GPIO_WriteHigh(GPIOB, GPIO_PIN_1); //时钟高
#define LCD_DATA_LOW    GPIO_WriteLow(GPIOB, GPIO_PIN_0);  //数据低
#define LCD_DATA_HIGH   GPIO_WriteHigh(GPIOB, GPIO_PIN_0); //数据高

/*------------------------------------------------------------------------------
*********************************** 函数声明 ***********************************
------------------------------------------------------------------------------*/
void HT1621_Init(void);      //初始化
void DisplayHT1621(void);    //赋值发送
void DisplayToHT1621(u8* p_data, u8 num);

#endif

