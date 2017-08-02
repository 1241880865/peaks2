

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

/* С���� */
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

S_CommData Com; //ͨѶ����



//��������
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
** Descriptions:   ͨѶ������������
** input:          void
** output:         void
** Return:         void
*******************************************************************************/
void UartDeal(void)
{
    u8 sum;
    
    if(Com.Flag == 3) //����
    {
        //У��� ������
        sum = CheckSum(Com.Buff, Com.Len);
        
        if(sum == Com.Buff[Com.Len])
        {
            //���ݴ�������ֵװ�
            DataRcved();
            //��������
            Com.Flag = 4;
            Com.TimMs_Delay = TIME_DELAY;
            UART_485TX;
        }
        else
        {
            Com.Flag = 0;
        }
    }
    else if(Com.Flag == 4) //����
    {
        if(Com.TimMs_Delay == 0)
        {
            Com.Flag = 5;
            Com.Step = 1;
            
            UART2->SR &= 0xBF;      //��UART2�����ж�
            UART2->DR = Com.Buff[0];//��ֵ����
        }
    }
    else if(Com.Flag == 6) //��ʱ
    {
        //��λ
        Com.Flag = 0;
        Com.Step = 0;
        //�н���״̬
        UART_485RX;
    }
}

/*******************************************************************************
** Function name:  DataRcved
** Descriptions:   �������ݴ���
** input:          void
** output:         void
** Return:         void
*******************************************************************************/
void DataRcved(void)
{
    switch (Com.Buff[1])
    {
        case 0xA0: //��ʵʱ���ݷ���
          RdRtData();
          break;
          
        case 0xA1: //��ȡ�趨ֵ����
          RdSetData();
          break;
          
        case 0xC0: //д���趨ֵ������
          WrSetData();
          break;
          
        case 0x9F: //д������������
          WrOd();
          break;
          
        default:
          break;
    }
}

/*******************************************************************************
** Function name:  RdRtData
** Descriptions:   ��ʵʱ����
** input:          void
** output:         void
** Return:         void
*******************************************************************************/
void RdRtData(void)
{
    //u16 run_st;
    //run_st = GetRunStatus();
    
    Com.Buff[0] = 0xFC;       //ǰ���ֽ�
    Com.Buff[1] = 0xFC;
    Com.Buff[2] = DEVICE_ADDR;//�ڵ��ַ
    Com.Buff[3] = 0xA0;       //������
    
    APART_PARA(DEVICE_TYPE,   Com.Buff[4],  Com.Buff[5]) //�Ǳ�����
    APART_PARA(GetRunStatus(),Com.Buff[6],  Com.Buff[7]) //��ǰ״̬
    APART_PARA(DATA_DOT,      Com.Buff[8],  Com.Buff[9]) //С����
    APART_PARA(SpdDisplay,    Com.Buff[10], Com.Buff[11])//�ٶȲ���
    APART_PARA(TempDisplay,   Com.Buff[12], Com.Buff[13])//�¶Ȳ���
    FillZeros(&(Com.Buff[14]), (29 - 13)); //����
    
    Com.Buff[30] = CheckSum(&Com.Buff[2], 28);//У���
    Com.Buff[31] = 0xFA;//������
    Com.Len = 32;
}

/*******************************************************************************
** Function name:  FillZeros
** Descriptions:   ������������
** input:          �����������׵�ַ�������ֽ���
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
** Descriptions:   ���趨����
** input:          void
** output:         void
** Return:         void
*******************************************************************************/
void RdSetData(void)
{
    Com.Buff[0] = 0xFC;       //ǰ���ֽ�
    Com.Buff[1] = 0xFC;
    Com.Buff[2] = DEVICE_ADDR;//�ڵ��ַ
    Com.Buff[3] = 0xA0;       //������
    
    APART_PARA(DEVICE_TYPE,   Com.Buff[4],  Com.Buff[5]) //�Ǳ�����
    APART_PARA(GetRunStatus(),Com.Buff[6],  Com.Buff[7]) //��ǰ״̬
    APART_PARA(DATA_DOT,      Com.Buff[8],  Com.Buff[9]) //С����
    APART_PARA(SpdSetVal,     Com.Buff[10], Com.Buff[11])//�ٶ��趨ֵ
    APART_PARA(TempSetVal,    Com.Buff[12], Com.Buff[13])//�¶��趨ֵ
    FillZeros(&(Com.Buff[14]), (29 - 13));//����
    
    //У���
    Com.Buff[30] = CheckSum(&Com.Buff[2], 28);
    //������
    Com.Buff[31] = 0xFA;
    Com.Len = 32;
}

/*******************************************************************************
** Function name:  WrSetData
** Descriptions:   д�趨����
** input:          void
** output:         void
** Return:         void
*******************************************************************************/
void WrSetData(void)
{
    u16 spdval = 0;
    u16 tempval = 0;
    //u16 dot_mult;
    
    //���趨ֵ
    ComDot.Byte = Com.Buff[6]; //С����
    //dot_mult = MultiplyNum(ComDot.Bit3);
    
    ComDt.Byte2 = Com.Buff[8]; //�ٶ��趨ֵ
    ComDt.Byte1 = Com.Buff[9];
    spdval = ComDt.TwoByte;
    
    ComDt.Byte2 = Com.Buff[10];//�¶��趨ֵ
    ComDt.Byte1 = Com.Buff[11];
    tempval = ComDt.TwoByte;
    //tempval = tempval * 10;
    
    //������ֵ
    Com.Buff[0] = 0xFC;//ǰ���ֽ�
    Com.Buff[1] = 0xFC;
    Com.Buff[2] = DEVICE_ADDR;//�ڵ��ַ
    Com.Buff[3] = 0xC0;//������
    Com.Buff[4] = 00;  //����
    Com.Buff[5] = 00;
    Com.Buff[6] = CheckSum(&Com.Buff[2], 4);//У���
    Com.Buff[7] = 0xFA;//������
    Com.Len = 8;
    
    //ֵ���ı䣬����EEprom
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
** Descriptions:   ����������
** input:          void
** output:         void
** Return:         void
*******************************************************************************/
void WrOd(void)
{
    //����������
    if(Com.Buff[2] == 0x01)
    {
        if(Com.Buff[3] != Tflag.EnTempRun)
        {
            Tflag.EnTempRun = Com.Buff[3];
            PwrTemp(); //�¶ȵ��籣��
        }
    }
    else if(Com.Buff[2] == 0x02)
    {
        if(Com.Buff[3] != Mflag.EnMotorRun)
        {
            Mflag.EnMotorRun = Com.Buff[3];
            Mflag.ErrHall = 0;
            PwrSpeed();//������籣��
        }
    }
    
    //������ֵ
    Com.Buff[4] = Com.Buff[2];//��������
    Com.Buff[5] = Com.Buff[3];//��ǰ����
    
    Com.Buff[0] = 0xFC;//ǰ���ֽ�
    Com.Buff[1] = 0xFC;
    Com.Buff[2] = DEVICE_ADDR;//�ڵ��ַ
    Com.Buff[3] = 0x9F;//������
    
    Com.Buff[6] = CheckSum(&Com.Buff[2], 4);//У���
    Com.Buff[7] = 0xFA;//������
    
    Com.Len = 8;
}

/*******************************************************************************
** Function name:  MultiplyNum
** Descriptions:   С����ƫ�ƷŴ�
** input:          С����λ��
** output:         void
** Return:         10��������
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
** Descriptions:   ��У��
** input:          У�����ֽ��׵�ַ���ֽڸ���
** output:         void
** Return:         У���
*******************************************************************************/
u8 CheckSum(u8* p_head, u8 len)//��ʼ��ַ ����
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
** Descriptions:   ��õ�ǰ����״̬
** input:          void
** output:         void
** Return:         ��ǰ����״̬
*******************************************************************************/
u16 GetRunStatus(void)
{
    U_RunStatus run_st;
    run_st.TwoByte = 0;
    
    //���±���
    if(Tflag.TempAlm != 0)
    {
        run_st.Bit1 = 1;
    }
    
    //�¶����
    if(Tflag.TempOver != 0)
    {
        run_st.Bit2 = 1;
    }
    
    //�ٶȱ�������
    if(Mflag.ErrHall != 0)
    {
        run_st.Bit3 = 1;
    }
    
    //����������
    if(Tflag.EnTempRun != 0)
    {
        run_st.Bit4 = 1;
    }
    
    //����������
    if(Mflag.EnMotorRun != 0)
    {
        run_st.Bit5 = 1;
    }
    
    return(run_st.TwoByte);
}



///*
//* ��ʵʱ���ݷ���
//*/
//void RdRtData(void)
//{
//  	Com.Buff[0] = 0xFC;       //ǰ���ֽ�
//	Com.Buff[1] = 0xFC;
//	Com.Buff[2] = DEVICE_ADD; //�ڵ��ַ
//	Com.Buff[3] = 0xA0;       //������
//	//�Ǳ�����
//	ComDt.TwoByte = DEVICE_TYPE;
//  	Com.Buff[4] = ComDt.Byte1;
//	Com.Buff[5] = ComDt.Byte2;
//    //��ǰ״̬
//	ComDt.TwoByte = GetRunStatus;
//	Com.Buff[6] = ComDt.Byte1;
//	Com.Buff[7] = ComDt.Byte2;
//    //С����
//	Com.Buff[8] = ;
//	Com.Buff[9] = DATA_DOT;
//    //�ٶȲ���
//	ComDt.TwoByte = SpdDisplay;
//	Com.Buff[10] = ComDt.Byte1;
//	Com.Buff[11] = ComDt.Byte2;
//    //�¶Ȳ���
//	ComDt.TwoByte = TempDisplay;
//	Com.Buff[12] = ComDt.Byte1;
//	Com.Buff[13] = ComDt.Byte2;
//}




//��������֤ У������֤

//������ѡ��
  
  //0xA0 ʵʱ���� ����
    //�Ǳ�����
    //��ǰ״̬
    //С����
    //�ٶȲ���
    //�¶Ȳ���
  
  //0xA1 ��ȡ�趨ֵ ����
    //�Ǳ�����
    //��ǰ״̬ 00
    //С����
    //�ٶ��趨
    //�¶��趨
  
  //0xC0 д���趨ֵ д��
    //�Ǳ����� 00
    //��ǰ״̬ 00
    //С����
    //�ٶ��趨
    //�¶��趨
    
    //����
    //0xC0 00 00
    
  
  //0x9F д��������� д��
    //��������
    //��������
    
    //����
    //0x9F 00 00



