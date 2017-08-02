

#include "Commn.h"
#include "MotorDriver.h"
#include "heatCtrl.h"

/*----------------------------------------------------------------------------*/
typedef union
{
    struct
    {
        u16 Byte1:8;
        u16 Byte2:8;
    };
    u16 TwoByte;
}
U_CommnData;

U_CommnData ComDt;
#define  APART_PARA(ab, b, a)  { ComDt.TwoByte = (ab);\
                                 (a) = ComDt.Byte1;\
                                 (b) = ComDt.Byte2; }

typedef union
{
    struct
    {
        u16 Bit1:1;
        u16 Bit2:1;
        u16 Bit3:1;
        u16 Bit4:1;
        u16 Bit5:1;
        u16 Bit6:1;
        u16 Bit7:1;
        u16 Bit8:1;
    };
    u16 TwoByte;
}
U_RunStatus;

/* 小数点 */
typedef union
{
    struct
    {
        u8 Bit1:2;
        u8 Bit2:2;
        u8 Bit3:2;
        u8 Bit4:2;
    };
    u8 Byte;
}
U_CommnDot;

U_CommnDot  ComDot;
U_CommnData ComDt;

S_CommData Com; //通讯参数



//函数声明
void DataRcved(void);
void RdRtData(void);
void FillZeros(u8* head, u8 len);
void RdSetData(void);
void WrSetData(void);
void WrOd(void);
u16  MultiplyNum(u8 place);
u8   CheckSum(u8* p_head, u8 len);
u16  GetRunStatus(void);
/*******************************************************************************
** Function name:  UartDeal
** Descriptions:   通讯处理函数（主）
** input:          void
** output:         void
** Return:         void
*******************************************************************************/
void UartDeal(void)
{
    u8 sum;
    
    if(Com.Flag == 3) //接收
    {
        //校验和 结束符
        sum = CheckSum(Com.Buff, Com.Len);
        
        if(sum == Com.Buff[Com.Len])
        {
            //数据处理（返回值装填）
            DataRcved();
            //触发发送
            Com.Flag = 4;
            Com.TimMs_Delay = TIME_DELAY;
            UART_485TX;
        }
        else
        {
            Com.Flag = 0;
        }
    }
    else if(Com.Flag == 4) //发送
    {
        if(Com.TimMs_Delay == 0)
        {
            Com.Flag = 5;
            Com.Step = 1;
            
            UART2->SR &= 0xBF;      //清UART2发送中断
            UART2->DR = Com.Buff[0];//赋值发送
        }
    }
    else if(Com.Flag == 6) //超时
    {
        //复位
        Com.Flag = 0;
        Com.Step = 0;
        //切接收状态
        UART_485RX;
    }
}

/*******************************************************************************
** Function name:  DataRcved
** Descriptions:   接收数据处理
** input:          void
** output:         void
** Return:         void
*******************************************************************************/
void DataRcved(void)
{
    switch (Com.Buff[1])
    {
        case 0xA0: //读实时数据返回
          RdRtData();
          break;
          
        case 0xA1: //读取设定值返回
          RdSetData();
          break;
          
        case 0xC0: //写入设定值并返回
          WrSetData();
          break;
          
        case 0x9F: //写入操作命令并返回
          WrOd();
          break;
          
        default:
          break;
    }
}

/*******************************************************************************
** Function name:  RdRtData
** Descriptions:   读实时数据
** input:          void
** output:         void
** Return:         void
*******************************************************************************/
void RdRtData(void)
{
    //u16 run_st;
    //run_st = GetRunStatus();
    
    Com.Buff[0] = 0xFC;       //前导字节
    Com.Buff[1] = 0xFC;
    Com.Buff[2] = DEVICE_ADDR;//节点地址
    Com.Buff[3] = 0xA0;       //功能码
    
    APART_PARA(DEVICE_TYPE,   Com.Buff[4],  Com.Buff[5]) //仪表类型
    APART_PARA(GetRunStatus(),Com.Buff[6],  Com.Buff[7]) //当前状态
    APART_PARA(DATA_DOT,      Com.Buff[8],  Com.Buff[9]) //小数点
    APART_PARA(SpdDisplay,    Com.Buff[10], Com.Buff[11])//速度测量
    APART_PARA(TempDisplay,   Com.Buff[12], Com.Buff[13])//温度测量
    FillZeros(&(Com.Buff[14]), (29 - 13)); //补零
    
    Com.Buff[30] = CheckSum(&Com.Buff[2], 28);//校验和
    Com.Buff[31] = 0xFA;//结束符
    Com.Len = 32;
}

/*******************************************************************************
** Function name:  FillZeros
** Descriptions:   空闲数据域补零
** input:          空闲数据区首地址，补零字节数
** output:         void
** Return:         void
*******************************************************************************/
void FillZeros(u8* head, u8 len)
{
    u8 i;
    
    for(i = 0; i < len; i++)
    {
        *(head + i) = 0;
    }
}

/*******************************************************************************
** Function name:  RdSetData
** Descriptions:   读设定数据
** input:          void
** output:         void
** Return:         void
*******************************************************************************/
void RdSetData(void)
{
    Com.Buff[0] = 0xFC;       //前导字节
    Com.Buff[1] = 0xFC;
    Com.Buff[2] = DEVICE_ADDR;//节点地址
    Com.Buff[3] = 0xA0;       //功能码
    
    APART_PARA(DEVICE_TYPE,   Com.Buff[4],  Com.Buff[5]) //仪表类型
    APART_PARA(GetRunStatus(),Com.Buff[6],  Com.Buff[7]) //当前状态
    APART_PARA(DATA_DOT,      Com.Buff[8],  Com.Buff[9]) //小数点
    APART_PARA(SpdSetVal,     Com.Buff[10], Com.Buff[11])//速度设定值
    APART_PARA(TempSetVal,    Com.Buff[12], Com.Buff[13])//温度设定值
    FillZeros(&(Com.Buff[14]), (29 - 13));//补零
    
    //校验和
    Com.Buff[30] = CheckSum(&Com.Buff[2], 28);
    //结束符
    Com.Buff[31] = 0xFA;
    Com.Len = 32;
}

/*******************************************************************************
** Function name:  WrSetData
** Descriptions:   写设定数据
** input:          void
** output:         void
** Return:         void
*******************************************************************************/
void WrSetData(void)
{
    u16 spdval = 0;
    u16 tempval = 0;
    //u16 dot_mult;
    
    //赋设定值
    ComDot.Byte = Com.Buff[6]; //小数点
    //dot_mult = MultiplyNum(ComDot.Bit3);
    
    ComDt.Byte2 = Com.Buff[8]; //速度设定值
    ComDt.Byte1 = Com.Buff[9];
    spdval = ComDt.TwoByte;
    
    ComDt.Byte2 = Com.Buff[10];//温度设定值
    ComDt.Byte1 = Com.Buff[11];
    tempval = ComDt.TwoByte;
    //tempval = tempval * 10;
    
    //赋返回值
    Com.Buff[0] = 0xFC;//前导字节
    Com.Buff[1] = 0xFC;
    Com.Buff[2] = DEVICE_ADDR;//节点地址
    Com.Buff[3] = 0xC0;//功能码
    Com.Buff[4] = 00;  //补零
    Com.Buff[5] = 00;
    Com.Buff[6] = CheckSum(&Com.Buff[2], 4);//校验和
    Com.Buff[7] = 0xFA;//结束符
    Com.Len = 8;
    
    //值若改变，保存EEprom
    if (tempval != TempSetVal)
    {
        if((tempval >= ParaTempCtrl.spl) && (tempval <= ParaTempCtrl.sph))
        {
            TempSetVal = tempval;
            EEpromWrite((u8 *)(&TempSetVal), EE_TEMPSETVAL, 2);
        }
    }
    if (spdval != SpdSetVal)
    {
        if((spdval >= ParaSpd.sdl) && (spdval <= ParaSpd.sdh))
        {
            SpdSetVal = spdval;
            EEpromWrite((u8 *)(&SpdSetVal), EE_SPDSETVAL, 2);
        }
    }
}

/*******************************************************************************
** Function name:  WrOd
** Descriptions:   读操作命令
** input:          void
** output:         void
** Return:         void
*******************************************************************************/
void WrOd(void)
{
    //赋操作命令
    if(Com.Buff[2] == 0x01)
    {
        if(Com.Buff[3] != Tflag.EnTempRun)
        {
            Tflag.EnTempRun = Com.Buff[3];
            PwrTemp(); //温度掉电保护
        }
    }
    else if(Com.Buff[2] == 0x02)
    {
        if(Com.Buff[3] != Mflag.EnMotorRun)
        {
            Mflag.EnMotorRun = Com.Buff[3];
            Mflag.ErrHall = 0;
            PwrSpeed();//电机掉电保护
        }
    }
    
    //赋返回值
    Com.Buff[4] = Com.Buff[2];//操作类型
    Com.Buff[5] = Com.Buff[3];//当前命令
    
    Com.Buff[0] = 0xFC;//前导字节
    Com.Buff[1] = 0xFC;
    Com.Buff[2] = DEVICE_ADDR;//节点地址
    Com.Buff[3] = 0x9F;//功能码
    
    Com.Buff[6] = CheckSum(&Com.Buff[2], 4);//校验和
    Com.Buff[7] = 0xFA;//结束符
    
    Com.Len = 8;
}

/*******************************************************************************
** Function name:  MultiplyNum
** Descriptions:   小数点偏移放大
** input:          小数点位数
** output:         void
** Return:         10的整数倍
*******************************************************************************/
u16 MultiplyNum(u8 place)
{
    u16 num = 1;
    
    if(place == 0)
    {
        num = 1;
    }
    else if(place == 1)
    {
        num = 10;
    }
    else if(place == 2)
    {
        num = 100;
    }
    else if(place == 3)
    {
        num = 1000;
    }
    
    return(num);
}

/*******************************************************************************
** Function name:  CheckSum
** Descriptions:   和校验
** input:          校验区字节首地址，字节个数
** output:         void
** Return:         校验和
*******************************************************************************/
u8 CheckSum(u8* p_head, u8 len)//起始地址 个数
{
    u8 sum = 0;
    u8  i;
    
    for(i = 0; i < len; i++)
    {
        sum += *(p_head + i);
    }
    
    return(sum);
}

/*******************************************************************************
** Function name:  GetRunStatus
** Descriptions:   获得当前运行状态
** input:          void
** output:         void
** Return:         当前运行状态
*******************************************************************************/
u16 GetRunStatus(void)
{
    U_RunStatus run_st;
    run_st.TwoByte = 0;
    
    //超温报警
    if(Tflag.TempAlm != 0)
    {
        run_st.Bit1 = 1;
    }
    
    //温度溢出
    if(Tflag.TempOver != 0)
    {
        run_st.Bit2 = 1;
    }
    
    //速度报警（）
    if(Mflag.ErrHall != 0)
    {
        run_st.Bit3 = 1;
    }
    
    //加热允许开启
    if(Tflag.EnTempRun != 0)
    {
        run_st.Bit4 = 1;
    }
    
    //搅拌允许开启
    if(Mflag.EnMotorRun != 0)
    {
        run_st.Bit5 = 1;
    }
    
    return(run_st.TwoByte);
}



///*
//* 读实时数据返回
//*/
//void RdRtData(void)
//{
//  	Com.Buff[0] = 0xFC;       //前导字节
//	Com.Buff[1] = 0xFC;
//	Com.Buff[2] = DEVICE_ADD; //节点地址
//	Com.Buff[3] = 0xA0;       //功能码
//	//仪表类型
//	ComDt.TwoByte = DEVICE_TYPE;
//  	Com.Buff[4] = ComDt.Byte1;
//	Com.Buff[5] = ComDt.Byte2;
//    //当前状态
//	ComDt.TwoByte = GetRunStatus;
//	Com.Buff[6] = ComDt.Byte1;
//	Com.Buff[7] = ComDt.Byte2;
//    //小数点
//	Com.Buff[8] = ;
//	Com.Buff[9] = DATA_DOT;
//    //速度测量
//	ComDt.TwoByte = SpdDisplay;
//	Com.Buff[10] = ComDt.Byte1;
//	Com.Buff[11] = ComDt.Byte2;
//    //温度测量
//	ComDt.TwoByte = TempDisplay;
//	Com.Buff[12] = ComDt.Byte1;
//	Com.Buff[13] = ComDt.Byte2;
//}




//结束符验证 校验码验证

//功能码选择
  
  //0xA0 实时数据 返回
    //仪表类型
    //当前状态
    //小数点
    //速度测量
    //温度测量
  
  //0xA1 读取设定值 返回
    //仪表类型
    //当前状态 00
    //小数点
    //速度设定
    //温度设定
  
  //0xC0 写入设定值 写入
    //仪表类型 00
    //当前状态 00
    //小数点
    //速度设定
    //温度设定
    
    //返回
    //0xC0 00 00
    
  
  //0x9F 写入操作命令 写入
    //操作类型
    //操作命令
    
    //返回
    //0x9F 00 00



