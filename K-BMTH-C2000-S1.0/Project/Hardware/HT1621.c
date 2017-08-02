


/*------------------------------------------------------------------------------
************************************ 头文件 ************************************
---- include -----------------------------------------------------------------*/
#include "stm8s.h"
#include "HT1621.h"


/*------------------------------------------------------------------------------
********************************* 内部函数声明 *********************************
------------------------------------------------------------------------------*/
static void HT1621SendCommand(u8 command);   //发送命令
static void HT1621SendAddress(u8 address);   //发送地址
static void HT1621Sendbit(u8 data,u8 number);//发送数据
/*******************************************************************************
	Function name:	      DisplayHT1621
	Descriptions:	      Display the data in the LCD					
	Parameters:	      void
	Returned value:       void
*******************************************************************************/
void DisplayToHT1621(u8* p_data, u8 num)
{
    u16 counter;
    
    //HT1621配置
    HT1621SendCommand(0x52); //1/3偏压，4背极
    HT1621SendCommand(0x30); //内部晶振
    HT1621SendCommand(0x02); //启动振荡器
    HT1621SendCommand(0x06); //打开LCD,关为0x04
    HT1621SendCommand(0x08); //关时基输出
    
    //数据显示
    /* 从0地址 全部刷新一遍 地址不变，数据自动四位一组，地址加一 */
    HT1621SendAddress(0x00);
    for(counter = 0; counter < num; counter++)
    {
        HT1621Sendbit(*(p_data + counter), 8);
    }
    LCD_CS_HIGH;
}

/*******************************************************************************
        Function name:	      HT1621_Init
        Descriptions:	      The HT1621 initial
        parameters:	      void
        Returned value:       void
*******************************************************************************/
void HT1621_Init(void)
{
    HT1621SendCommand(0x52); //1/3偏压，4背极
    HT1621SendCommand(0x30); //内部晶振
    HT1621SendCommand(0x02); //启动振荡器
    HT1621SendCommand(0x06); //打开LCD,关为0x04
    HT1621SendCommand(0x08); //关时基输出
    
    HT1621SendAddress(0x00); //从地址00开始
    HT1621Sendbit(0x00,128); //32字节RAM全部清零
    
    LCD_CS_HIGH;
}

/*******************************************************************************
        Function name:	      HT1621SendCommand
	Descriptions:	      Send command to the HT1621			
	parameters:           u8 command
	Returned value:       void	
*******************************************************************************/
void HT1621SendCommand(u8 command)
{
    LCD_CS_LOW;
    
    HT1621Sendbit(0x80,4);
    HT1621Sendbit(command,8);
    
    LCD_CS_HIGH;
}

/*******************************************************************************
        Function name:	      HT1621SendAddress
	Descriptions:	      Send address to the HT1621					
	parameters:	      u8 address
	Returned value:       void	
*******************************************************************************/
void HT1621SendAddress(u8 address)
{
    LCD_CS_LOW;
    
    HT1621Sendbit(0xA0,4);/* 地址开始 发送1010 */
    HT1621Sendbit(address,5);
}

/*******************************************************************************
	Function name:	      HT1621Sendbit
	Descriptions:	      Send bit to the HT1621
	parameters:	      u8 data,u8 number
	Returned value:       void
*******************************************************************************/
void HT1621Sendbit(u8 data,u8 number)
{
    u16 counter=0;
    
    for(counter = 0; counter < number; counter++)
    {
        if((data & 0x80) == 0)
        {
            LCD_DATA_LOW;
        }
	else
        {
            LCD_DATA_HIGH;
        }
        
	LCD_CLK_LOW;
        nop();nop();nop();nop();
        nop();nop();nop();nop();
        nop();nop();nop();nop();
        nop();nop();nop();nop();
        nop();nop();nop();nop();
        nop();nop();nop();nop();
        nop();nop();nop();nop();
        nop();nop();nop();nop();
        
	LCD_CLK_HIGH;
        
        nop();nop();nop();nop();
        nop();nop();nop();nop();
        nop();nop();nop();nop();
        nop();nop();nop();nop();
        
	data=data<<1; //发送数据左移一位
    }
}

/******************************************************************************/