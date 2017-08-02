

#include "stm8s.h"
#include "BT3153.h"

/*----------------------------------------------------------------------------*/
//��ʾ����
#define HIDE_ZERO  0xAF //������ʾ��"0"ֵΪ0xAF
const u8 c_code[] =
{
    /*0,  1,   2,   3,   4,   5,   6,   7,   8,   9 */
    0xAF,0xA0,0xCB,0xE9,0xE4,0x6D,0x6F,0xA8,0xEF,0xED
};

/*----------------------------------------------------------------------------*/
STLAMP_CS StLamp[23] =
{
//�Ҳ���
    {0, 0x10},//"M"
    {4, 0x80},//"RPM1"
    {4, 0x40},//"F"
    {4, 0x20},//"RPM2"
    {4, 0x10},//"H"
    {4, 0x08},//"��1"
    {6, 0x10},//"��2"
//����
    {4, 0x04},//"HEAT"
    {4, 0x02},//"OUT"
    {4, 0x01},//"AT"
    {5, 0x10},//"MAIN"
    {5, 0x20},//"ALM"
    {5, 0x40},//"COOL"
//�����
    {3, 0x10},//"TIME"
    {9, 0x10},//"SET"
    {10, 0x80},//"PV"
    {10, 0x40},//"SV1"
    {10, 0x20},//"A"
    {10, 0x10},//"B"
    {10, 0x08},//"TEMP"
    {10, 0x04},//"SV2"
    {10, 0x02},//"STOP"
    {10, 0x01} //"RUN"
};
/*----------------------------------------------------------------------------*/
u8 g_screen_buf[BUFF_NUM];//HT1621��������
static u8 s_dis_buff[4];  //��ʾ�м�����

//ʱ�����
u16 TimMs_DisDelay;

//����
static void updateScreenBuf(u8 dot, u8 row);
static u8 convertHCD(u8 ad_num);
/*******************************************************************************
** Function name:  SendToScreen
** Descriptions:   ��������Ļ
** Input:          ��
** Output:         ��
** Return:         ��
*******************************************************************************/
void SendToScreen(void)
{
    DisplayToHT1621(g_screen_buf, BUFF_NUM);
}

/*******************************************************************************
** Function name:  updateScreenBuf
** Descriptions:   ��ֵ����������
** Input:          ��ʾ С���� ����
** Output:         ��������
** Return:         ��
*******************************************************************************/
void updateScreenBuf(u8 dot, u8 row)
{
    switch (row)
    {
      case 0x01:        //������һ�� �¶�
          g_screen_buf[9] = s_dis_buff[0];
          g_screen_buf[8] = s_dis_buff[1];
          g_screen_buf[7] = s_dis_buff[2];
          g_screen_buf[6] = s_dis_buff[3];
          if (dot == 1)
          {
              g_screen_buf[7] |= 0x10;
          }
          break;
          
      case 0x02:        //�����ڶ��� �ٶ�
          g_screen_buf[3] = s_dis_buff[0];
          g_screen_buf[2] = s_dis_buff[1];
          g_screen_buf[1] = s_dis_buff[2];
          g_screen_buf[0] = s_dis_buff[3];
          if (dot == 1)
          {
              g_screen_buf[1] |= 0x10;
          }
          else if(dot == 2)
          {
              g_screen_buf[2] |= 0x10;
          }
          break;
          
      default:
          break;
    }
}

/*******************************************************************************
                           ---- ͨ�ã���ʾӦ�ò㣩 ----
*******************************************************************************/
/*******************************************************************************
** Function name:  displaySglAbc
** Descriptions:   ��ʾ�̶��ַ�
** Input:          ��λ��ʾ a b c d, ��ʾ�����������У�
** Output:         ��
** Return:         ��
*******************************************************************************/
void DisplaySglABC(u8 a, u8 b, u8 c, u8 d, u8 row)
{
    s_dis_buff[0] = a;
    s_dis_buff[1] = b;
    s_dis_buff[2] = c;
    s_dis_buff[3] = d;
	
    updateScreenBuf(0, row);
}

/*******************************************************************************
** Function name:  display_abc
** Descriptions:   ��ʾ��ĸ
** Input:          ��ʾ ����
** Output:         ��
** Return:         ��
*******************************************************************************/
void DisplayABC(PARARANGE *d, u8 row)
{
    s_dis_buff[0] = d->dis_a;
    s_dis_buff[1] = d->dis_b;
    s_dis_buff[2] = d->dis_c;
    s_dis_buff[3] = d->dis_d;
	
    updateScreenBuf(0, row);
}

/*******************************************************************************
** Function name:  Display_HCD
** Descriptions:   ת����BCD����ʾ
** Input:          1��ʮ������ʾ����2����ʾ��������
** Output:         ��
** Return:         ��
*******************************************************************************/
void DisplayHCD(s16 dec_num, u8 hcd_row)
{
    u8 dath, datl;
    
    dath = (u8)(dec_num >> 8);
    datl = (u8)(dec_num);
    
    s_dis_buff[0] = convertHCD((dath >> 4) & 0x0F);
    s_dis_buff[1] = convertHCD(dath & 0x0F);
    s_dis_buff[2] = convertHCD((datl >> 4) & 0x0F);
    s_dis_buff[3] = convertHCD(datl & 0x0F);
    
    updateScreenBuf(0, hcd_row);
}

/*******************************************************************************
** Function name:  Display_HCD
** Descriptions:   BCD��ת��
** Input:          ʮ������ʾ��
** Output:         ��
** Return:         16������ʾ����
*******************************************************************************/
u8 convertHCD(u8 ad_num)
{
    if (ad_num <= 9)
    {
        ad_num = c_code[ad_num];
    }
    else
    {
        switch(ad_num)
        {
            case 10:  ad_num = DF_A;  break;
            case 11:  ad_num = DF_b;  break;
            case 12:  ad_num = DF_C;  break;
            case 13:  ad_num = DF_d;  break;
            case 14:  ad_num = DF_E;  break;
            case 15:  ad_num = DF_F;  break;
            
            default:  break;
        }
    }
    
    return(ad_num);
}

/*******************************************************************************
** Function name:  DisplayNum
** Descriptions:   ʮ������ʾ
** Input:          ��ʾ ����
** Output:         ��
** Return:         ��
*******************************************************************************/
void DisplayNum(DISPLAYGNUM* d, u8 row)
{
    u8 flag_negative; //����
    s16 number;
    
    /* ȡ��ֵ */
    number = d->num;
    
    /* ���� */
    if(number < 0)
    {
        flag_negative = 1;
        number = 0 - number;
    }
    else flag_negative = 0;
    
    /* ���ָ�λ���� */
    s_dis_buff[0] = *(c_code + number / 1000);         //����ǧλ
    s_dis_buff[1] = *(c_code + (number % 1000) / 100); //�����λ
    s_dis_buff[2] = *(c_code + (number % 100) / 10);   //����ʮλ
    s_dis_buff[3] = *(c_code + (number % 10));         //�����λ
    
    /* ��ʾ���� */
    if(d->flag_hide_0 != 0)
    {
        if (s_dis_buff[0] == HIDE_ZERO) // 0��"HIDE_ZERO"
        {
            s_dis_buff[0] = 0x00;  //����ʾ��"0x00"
            if (s_dis_buff[1] == HIDE_ZERO)
            {
                s_dis_buff[1] = 0x00;
                if ((s_dis_buff[2] == HIDE_ZERO) && (d->dot == 0))//�����ڶ�λΪ�㣬����С��
                {
                    s_dis_buff[2] = 0x00;
                }
            }
        }
    }
    
    /* ������ʾ */
    if(flag_negative != 0)          //display "-"
    {
        if(s_dis_buff[2] == 0x00)
        {
            s_dis_buff[2] = 0x40;
        }
        else if(s_dis_buff[1] == 0x00)
        {
            s_dis_buff[1] = 0x40;
        }
        else
        {
            s_dis_buff[0] = 0x40;
        }
    }
    
    /* ��λ��˸ */
    if(d->flag_flash == 1)
    {
        if(TimMs_DisDelay < 220)
        {
            s_dis_buff[3 - d->flashBit] = 0x00;  //��  d->flashBit ȡֵ��0 1 2 3
        }
        else if(TimMs_DisDelay > 520)
        {
            TimMs_DisDelay = 0;
        }
    }
    else if(d->flag_flash == 2)
    {
        if(TimMs_DisDelay < 220)
        {
            s_dis_buff[0] = 0x00;  //ȫ����˸
            s_dis_buff[1] = 0x00;
            s_dis_buff[2] = 0x00;
            s_dis_buff[3] = 0x00;
            
            d->dot = 0;
        }
        else if(TimMs_DisDelay > 520)
        {
            TimMs_DisDelay = 0;
        }
    }
    
    updateScreenBuf(d->dot, row);
}

/*******************************************************************************
** Function name:  display_num
** Descriptions:   ��ʾ����
** Input:          ��ʾ��  ����
** Output:         ��ʾ������
** Return:         ��
*******************************************************************************/
//void display_num(DISPLAYGNUM disp_num, u8 flag_dec_hex, u8 row)
//{
//    if(flag_dec_hex != 0)
//    {
//        /* ʮ��������ʾ */
//        //DispHex(d, row);
//    }
//    else
//    {
//        /* ʮ������ʾ */
//        display_num_dec(disp_num, row);
//    }
//}

