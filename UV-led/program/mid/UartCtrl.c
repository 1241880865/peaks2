

/*����ͨѶЭ��
5mS�����Ϊ��ͷ
*/
#include "UartCtrl.h"


#define ERROR_CRC	0x08   	//CRC�������
#define ERROR_CODE	0x01   	//���������
#define ERROR_ADD	0x02   	//���ݵ�ַ����
#define ERROR_VAL	0x03   	//����ֵ����
#define ERROR_ERR  	0x04   	//�豸���ϴ���


u8 Flag_RcvStep;  //��������ִ�в���
u8 RcvCount;      //��������
u8 RcvLength;     //���ݳ���
u8 RcvBuff[32];   //��������
static u8 SendBuff[32];//��������

//ʱ�����
u8 TimMs_ModbusWait; //Modbus-RTUͷ��⣨�жϣ�
u8 TimMs_ModbusClear;//Modbus-Clear

//�ڲ�����
static void	Delay_Ms(u16 ms);
static void SendByte(u8 senddata);
static void SendData(u8 num, u8 *senddat);
static void CrcCheck(u8 *pCheckBuf,u8 CheckLth,u8 *CheckLow,u8 *CheckHigh);
static void UartErr(u8 err);
static bool AddDetectData(s16 head, s16 end);
/*******************************************************************************
** Function name:       Delay_Ms
** Descriptions:        ������ʱ
** input parameters:    ��ʱʱ��
** output parameters:   ��
** Returned value:      ��
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
** Descriptions:        ������ʱ
** input parameters:    ��ʱʱ��
** output parameters:   ��
** Returned value:      ��
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
** Descriptions:        ����һ���ֽ�
** input parameters:    һ�ֽڱ���
** output parameters:   ��
** Returned value:      ��
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
** Descriptions:        ��������
** input parameters:    1�������������
                        2�����������׵�ַ
** output parameters:   ��
** Returned value:      ��
*******************************************************************************/
void SendData(u8 num, u8 *senddat)
{	   
    u8 i;
    
	if(RcvBuff[0] != BROADCAST_ADDR) //��ַ��ΪBROADCAST_ADDRʱ�����з���
	{
		WR_485;//�л�Ϊ����
		Delay_Ms( 10 );
		
		for(i = 0; i < num; i++)
		{
			SendByte(*senddat);
			senddat++;
		}
		
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		
		Delay_Ms( 3 );
		RD_485;//�л�Ϊ����
		Delay_Ms( 7 );
	}
		
    Flag_RcvStep = 0;	//�����־
}

/*******************************************************************************
** Function name:       CrcCheck
** Descriptions:        ���������CRCУ��
** input parameters:    u8 *pCheckBuf	Ҫ���������
						u8 CheckLth	  ���鳤��
						u8 CheckHigh	У���λ
						u8 CheckLow	  У���λ
** output parameters:   ��
** Returned value:      ��
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
** Descriptions:        �쳣���뷢��
** input parameters:    u8 err �쳣����
** output parameters:   ��
** Returned value:      ��
*******************************************************************************/
void UartErr(u8 err)
{
    u8  crclow, crchigh;
    
    SendBuff[0] = RcvBuff[0];	    //�ڵ��ַ
    SendBuff[1] = RcvBuff[1] + 0x80;//��������
    SendBuff[2] = err;		        //�쳣��
    
    //CRCУ��
    CrcCheck(SendBuff, 3, &crclow, &crchigh);
    
    SendBuff[3] = crclow;
    SendBuff[4] = crchigh;
    
    SendData(5, SendBuff);//��������
}

/*******************************************************************************
** Function name:       UartTest
** Descriptions:        ����������Ч����֤
** input parameters:    ��
** output parameters:   ��
** Returned value:      ���ܴ��룬0XFF��ʾ�д����δ�н��գ������½���
*******************************************************************************/
u8 UartTest(void)
{
    u8  crclow, crchigh;
    
    if (Flag_RcvStep == 6)//�������
    {
        UartErr(ERROR_ERR);
        return(0xff);
    }
    if (Flag_RcvStep != 5)//�Ƿ�������
    {
        return(0xff);
    }
    
    /* CRCУ�� */
    CrcCheck(RcvBuff, RcvLength-2, &crclow, &crchigh);
    //У��ʧ��
    if((RcvBuff[RcvLength-1] != crchigh) ||\
       (RcvBuff[RcvLength-2] != crclow))
    {
        UartErr(ERROR_CRC);
        return(0xff);
    }
    
    //�жϹ�����
    if((RcvBuff[1] != DATA_CODE_READ)	 &&\
       (RcvBuff[1] != DATA_CODE_SWRITE))
    {
        UartErr(ERROR_CODE);//��������
        return(0xff);
    }
    
    return(RcvBuff[1]);//���ع��ܴ���
}

/*******************************************************************************
** Function name:       AddDetectData
** Descriptions:        ͨѶ-->���ݵ�ַУ��
** input parameters:    1�������׵�ַ
												2������ĩ��ַ
** output parameters:   ��
** Returned value:      ��ַ�Ƿ�����
*******************************************************************************/
bool AddDetectData(s16 head, s16 end)
{
    bool flag = true;
    
    if((head >= 0x00) || (end <= 0x0B))
    {
        flag = false; //��ַ��Χ����
    }
    
    return(flag);
}

/*******************************************************************************
** Function name:       RdData
** Descriptions:        ������03�����������
** input parameters:    1�����ݱ�
** output parameters:   ��
** Returned value:      ��
*******************************************************************************/
void RdData(UART_PARA *ppara) //03
{
    u8	i;
    bool  flag = true;
    u16 add_head, add_end;
    u8	num, j;
    u8  crclow, crchigh;
    
    /* �������ݣ���д��ַ����д������������ַ */
    add_head = (RcvBuff[2] << 8) + RcvBuff[3];//��ʼ��ַ
    num = (RcvBuff[4] << 8) + RcvBuff[5];     //״̬����
    add_end = add_head + num - 1;	          //������ַ
	
    /* �Ƿ�����ֵ(�˴�Ϊ������������) */
    if((num == 0) || (num > PARA_NUM))
    {
        UartErr(ERROR_VAL);
        return;
    }
		
    /* �Ƿ����ݵ�ַ */
    flag = AddDetectData(add_head, add_end); //��ַУ��
    if(flag) //�Ƿ���ַ
    {
        UartErr(ERROR_ADD);
        return;
    }
    
    /* ȡֵ��buffer */
    j = 3;
    for (i = 0; i < num; i++)
    {
		SendBuff[j++] = (u8)(*((u16 *)ppara + add_head + i) >> 8);
		SendBuff[j++] = (u8)(*((u16 *)ppara + add_head + i));
    }
    
    /* ���ݷ��� */
    SendBuff[0] = RcvBuff[0];	//�ڵ��ַ
    SendBuff[1] = RcvBuff[1];	//���ܴ���
	num = num << 1;
    SendBuff[2] = num;		    //�ֽ���
    
    //CRCУ��
    CrcCheck(SendBuff, j, &crclow, &crchigh);
    
    SendBuff[j++] = crclow;
    SendBuff[j++] = crchigh;
    
    SendData(j, SendBuff);	//��������
}

/*******************************************************************************
** Function name:  WrDataSgl
** Descriptions:   ������06��д��������
** input:          1�����ݱ�
** output:         ��
** Return:         ���ݱ�ƫ�Ƶ�ַ
*******************************************************************************/
u16 WrDataSgl(UART_PARA *ppara, RANGE_STRUCT *range) //06
{
    bool flag = true;
    u16  add_head;
    u8   crclow, crchigh;
    u16	 temp;
	u16  *padd_rtn;
    
    /* �������ݣ���д��ַ */
    add_head = (RcvBuff[2] << 8) + RcvBuff[3];//�׵�ַ����д��һ������
		
    /* �Ƿ����ݵ�ַ */
    flag = AddDetectData(add_head, add_head); //��ַУ��
    if(flag) //�Ƿ���ַ
    {
        UartErr(ERROR_ADD);
        return 0xff;
    }
	padd_rtn = (u16 *)ppara + add_head;
    
    /* ���ݽ��� */
    temp = (RcvBuff[4] << 8) + RcvBuff[5];//����ֵ
    if(temp > (range + add_head)->Max)  //�������ޣ���Ϊ����
    {
        temp = (range + add_head)->Max;
    }
    if(temp < (range + add_head)->Min)  //С�����ޣ���Ϊ����
    {
        temp = (range + add_head)->Min;
    }
    *padd_rtn = temp;
    
    /* ���ͷ��أ���д��ַ��Ϊ�ڵ��ַ�� */
	SendBuff[0] = RcvBuff[0];	//�ڵ��ַ
	SendBuff[1] = RcvBuff[1];	//���ܴ���
	SendBuff[2] = RcvBuff[2];	//��ʼ��ַ
	SendBuff[3] = RcvBuff[3];	//��ʼ��ַ
	SendBuff[4] = RcvBuff[4];	//��������
	SendBuff[5] = RcvBuff[5];	//��������
	
	//CRCУ��
	CrcCheck(SendBuff, 6, &crclow, &crchigh);
		
	SendBuff[6] = crclow;
	SendBuff[7] = crchigh;
	
	SendData(8, SendBuff);	//��������
	
	return(add_head);
}

/*******************************************************************************
-------------------------------------- END -------------------------------------
*******************************************************************************/
