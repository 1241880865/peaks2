

/*串口通讯协议
5mS间隔作为字头
*/
#include "UartCtrl.h"


#define ERROR_CRC	0x08   	//CRC检验错误
#define ERROR_CODE	0x01   	//功能码错误
#define ERROR_ADD	0x02   	//数据地址错误
#define ERROR_VAL	0x03   	//数据值错误
#define ERROR_ERR  	0x04   	//设备故障错误


u8 Flag_RcvStep;  //接收数据执行步骤
u8 RcvCount;      //数据数量
u8 RcvLength;     //数据长度
u8 RcvBuff[32];   //接收数据
static u8 SendBuff[32];//发送数据

//时间变量
u8 TimMs_ModbusWait; //Modbus-RTU头检测（中断）
u8 TimMs_ModbusClear;//Modbus-Clear

//内部函数
static void	Delay_Ms(u16 ms);
static void SendByte(u8 senddata);
static void SendData(u8 num, u8 *senddat);
static void CrcCheck(u8 *pCheckBuf,u8 CheckLth,u8 *CheckLow,u8 *CheckHigh);
static void UartErr(u8 err);
static bool AddDetectData(s16 head, s16 end);
/*******************************************************************************
** Function name:       Delay_Ms
** Descriptions:        毫秒延时
** input parameters:    延时时间
** output parameters:   无
** Returned value:      无
*******************************************************************************/
void Delay_Ms(u16 ms)
{
	u16 i, j;

	for(i = 0; i < ms; i++)
	{
		for(j = 0; j < 1000; j++);
	}
}

/*******************************************************************************
** Function name:       Delay_Ms
** Descriptions:        毫秒延时
** input parameters:    延时时间
** output parameters:   无
** Returned value:      无
*******************************************************************************/
void ModbusClear(void)
{
	if(Flag_RcvStep > 1)
	{
		if(TimMs_ModbusClear >= 50)
		{
			Flag_RcvStep = 0;
		}
	}
	else
	{
		TimMs_ModbusClear = 0;
	}
}

/*******************************************************************************
** Function name:       send_byte
** Descriptions:        发送一个字节
** input parameters:    一字节变量
** output parameters:   无
** Returned value:      无
*******************************************************************************/
void SendByte(u8 senddata)
{
	LED2ON();
	USART_SendData(USART1, senddata);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	LED2OFF();
}

/*******************************************************************************
** Function name:       SendData
** Descriptions:        发送数据
** input parameters:    1、发送数组个数
                        2、发送数组首地址
** output parameters:   无
** Returned value:      无
*******************************************************************************/
void SendData(u8 num, u8 *senddat)
{	   
    u8 i;
    
	if(RcvBuff[0] != BROADCAST_ADDR) //地址不为BROADCAST_ADDR时接收有返回
	{
		WR_485;//切换为发送
		Delay_Ms( 10 );
		
		for(i = 0; i < num; i++)
		{
			SendByte(*senddat);
			senddat++;
		}
		
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		
		Delay_Ms( 3 );
		RD_485;//切换为接收
		Delay_Ms( 7 );
	}
		
    Flag_RcvStep = 0;	//清除标志
}

/*******************************************************************************
** Function name:       CrcCheck
** Descriptions:        计算数组的CRC校验
** input parameters:    u8 *pCheckBuf	要计算的数组
						u8 CheckLth	  数组长度
						u8 CheckHigh	校验高位
						u8 CheckLow	  校验低位
** output parameters:   无
** Returned value:      无
*******************************************************************************/
void CrcCheck(u8 *pCheckBuf,u8 CheckLth,u8 *CheckLow,u8 *CheckHigh)
{ 
    u8 	i;
    u8 	j;
    u16	CRCdata = 0xFFFF;
    u8 	TT;
    
    for(j = 0; j < CheckLth; j++)
    { 
        CRCdata = CRCdata ^ pCheckBuf[j];
        for(i = 0; i < 8; i++)
        {
            TT = CRCdata & 1;
            CRCdata = CRCdata>>1;
            CRCdata = CRCdata & 0x7fff;
            if (TT == 1)
            {
                CRCdata = CRCdata ^ 0xa001;
            }
        } 
    }
    *CheckLow = (u8)(CRCdata & 0xFF);
    *CheckHigh = CRCdata >> 8;
}

/*******************************************************************************
** Function name:       UartErr
** Descriptions:        异常代码发送
** input parameters:    u8 err 异常代码
** output parameters:   无
** Returned value:      无
*******************************************************************************/
void UartErr(u8 err)
{
    u8  crclow, crchigh;
    
    SendBuff[0] = RcvBuff[0];	    //节点地址
    SendBuff[1] = RcvBuff[1] + 0x80;//错误功能码
    SendBuff[2] = err;		        //异常码
    
    //CRC校验
    CrcCheck(SendBuff, 3, &crclow, &crchigh);
    
    SendBuff[3] = crclow;
    SendBuff[4] = crchigh;
    
    SendData(5, SendBuff);//发送数据
}

/*******************************************************************************
** Function name:       UartTest
** Descriptions:        接收数据有效性验证
** input parameters:    无
** output parameters:   无
** Returned value:      功能代码，0XFF表示有错误或未有接收，不向下进行
*******************************************************************************/
u8 UartTest(void)
{
    u8  crclow, crchigh;
    
    if (Flag_RcvStep == 6)//接收溢出
    {
        UartErr(ERROR_ERR);
        return(0xff);
    }
    if (Flag_RcvStep != 5)//是否接收完毕
    {
        return(0xff);
    }
    
    /* CRC校验 */
    CrcCheck(RcvBuff, RcvLength-2, &crclow, &crchigh);
    //校验失败
    if((RcvBuff[RcvLength-1] != crchigh) ||\
       (RcvBuff[RcvLength-2] != crclow))
    {
        UartErr(ERROR_CRC);
        return(0xff);
    }
    
    //判断功能码
    if((RcvBuff[1] != DATA_CODE_READ)	 &&\
       (RcvBuff[1] != DATA_CODE_SWRITE))
    {
        UartErr(ERROR_CODE);//代码有误
        return(0xff);
    }
    
    return(RcvBuff[1]);//返回功能代码
}

/*******************************************************************************
** Function name:       AddDetectData
** Descriptions:        通讯-->数据地址校验
** input parameters:    1、数据首地址
												2、数据末地址
** output parameters:   无
** Returned value:      地址是否有误
*******************************************************************************/
bool AddDetectData(s16 head, s16 end)
{
    bool flag = true;
    
    if((head >= 0x00) || (end <= 0x0B))
    {
        flag = false; //地址范围无误
    }
    
    return(flag);
}

/*******************************************************************************
** Function name:       RdData
** Descriptions:        功能码03，读多个数据
** input parameters:    1、数据表
** output parameters:   无
** Returned value:      无
*******************************************************************************/
void RdData(UART_PARA *ppara) //03
{
    u8	i;
    bool  flag = true;
    u16 add_head, add_end;
    u8	num, j;
    u8  crclow, crchigh;
    
    /* 解析数据：读写地址，读写数量，结束地址 */
    add_head = (RcvBuff[2] << 8) + RcvBuff[3];//起始地址
    num = (RcvBuff[4] << 8) + RcvBuff[5];     //状态数量
    add_end = add_head + num - 1;	          //结束地址
	
    /* 非法数据值(此处为变量数量个数) */
    if((num == 0) || (num > PARA_NUM))
    {
        UartErr(ERROR_VAL);
        return;
    }
		
    /* 非法数据地址 */
    flag = AddDetectData(add_head, add_end); //地址校验
    if(flag) //非法地址
    {
        UartErr(ERROR_ADD);
        return;
    }
    
    /* 取值赋buffer */
    j = 3;
    for (i = 0; i < num; i++)
    {
		SendBuff[j++] = (u8)(*((u16 *)ppara + add_head + i) >> 8);
		SendBuff[j++] = (u8)(*((u16 *)ppara + add_head + i));
    }
    
    /* 数据发送 */
    SendBuff[0] = RcvBuff[0];	//节点地址
    SendBuff[1] = RcvBuff[1];	//功能代码
	num = num << 1;
    SendBuff[2] = num;		    //字节数
    
    //CRC校验
    CrcCheck(SendBuff, j, &crclow, &crchigh);
    
    SendBuff[j++] = crclow;
    SendBuff[j++] = crchigh;
    
    SendData(j, SendBuff);	//发送数据
}

/*******************************************************************************
** Function name:  WrDataSgl
** Descriptions:   功能码06，写单个数据
** input:          1、数据表
** output:         无
** Return:         数据表偏移地址
*******************************************************************************/
u16 WrDataSgl(UART_PARA *ppara, RANGE_STRUCT *range) //06
{
    bool flag = true;
    u16  add_head;
    u8   crclow, crchigh;
    u16	 temp;
	u16  *padd_rtn;
    
    /* 解析数据：读写地址 */
    add_head = (RcvBuff[2] << 8) + RcvBuff[3];//首地址，仅写入一个变量
		
    /* 非法数据地址 */
    flag = AddDetectData(add_head, add_head); //地址校验
    if(flag) //非法地址
    {
        UartErr(ERROR_ADD);
        return 0xff;
    }
	padd_rtn = (u16 *)ppara + add_head;
    
    /* 数据接收 */
    temp = (RcvBuff[4] << 8) + RcvBuff[5];//数据值
    if(temp > (range + add_head)->Max)  //大于上限，即为上限
    {
        temp = (range + add_head)->Max;
    }
    if(temp < (range + add_head)->Min)  //小于下限，即为下限
    {
        temp = (range + add_head)->Min;
    }
    *padd_rtn = temp;
    
    /* 发送返回（若写地址不为节点地址） */
	SendBuff[0] = RcvBuff[0];	//节点地址
	SendBuff[1] = RcvBuff[1];	//功能代码
	SendBuff[2] = RcvBuff[2];	//起始地址
	SendBuff[3] = RcvBuff[3];	//起始地址
	SendBuff[4] = RcvBuff[4];	//数据数量
	SendBuff[5] = RcvBuff[5];	//数据数量
	
	//CRC校验
	CrcCheck(SendBuff, 6, &crclow, &crchigh);
		
	SendBuff[6] = crclow;
	SendBuff[7] = crchigh;
	
	SendData(8, SendBuff);	//发送数据
	
	return(add_head);
}

/*******************************************************************************
-------------------------------------- END -------------------------------------
*******************************************************************************/
