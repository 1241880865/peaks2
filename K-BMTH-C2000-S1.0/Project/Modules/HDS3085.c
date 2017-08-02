

#include "stm8s.h"
#include "HDS3085.h"

/*----------------------------------------------------------------------------*/
//��ʾ����
#define HIDE_ZERO  0xAF //������ʾ��"0"ֵΪ0xAF
const u8 c_code[] =
{
    /*0,  1,   2,   3,   4,   5,   6,   7,   8,   9 */
    0xAF,0xA0,0xCB,0xE9,0xE4,0x6D,0x6F,0xA8,0xEF,0xED
};

/*----------------------------------------------------------------------------*/
STLAMP_CS StLamp[25] =
{
//��һ�б�ʶ��
    {12,0x10},//"PV"
    {14,0x10},//"TEMP"
    {8, 0x40},//"SV"
    {8, 0x20},//"SPEED2"
    {4, 0x20},//"SPEED3"
    {4, 0x10},//"VOL"
    {0, 0x01},//"TIME"
    {4, 0x40},//"A��ת"
    {4, 0x80},//"B��ת"
//�ڶ��б�ʶ��
    {8, 0x80},//"��1"
    {0, 1},//"F" 11 0x10
    {5, 0x80},//"��2"
    {6, 0x10},//"RPM2"
    {4, 0x08},//"RPM3"
    {4, 0x04},//"V"
    {3, 0x01},//"M"
//�����б�ʶ��
    {5, 0x01},//"SET"
    {5, 0x02},//"AT"
    {5, 0x04},//"ALM!"
    {5, 0x08},//"sw"
    {5, 0x40},//"HEAT"
    {5, 0x20},//"COOL"
    {5, 0x10},//"OUT"
    {4, 0x02},//"run"
    {4, 0x01} //"stop"
};
/*----------------------------------------------------------------------------*/
u8 g_screen_buf[BUFF_NUM];//HT1621��������
static u8 s_dis_buff[4];  //��ʾ�м�����

//ʱ�����
u16 TimMs_DisDelay;

//����
static u8 reversePlace(u8 positive);
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
          g_screen_buf[12] = s_dis_buff[0];
          g_screen_buf[11] = s_dis_buff[1];
          g_screen_buf[10] = s_dis_buff[2];
          g_screen_buf[9]  = s_dis_buff[3];
          if (dot == 1)
          {
              g_screen_buf[9] |= 0x10;
          }
          break;
          
      case 0x02:        //�����ڶ��� �ٶ�
          g_screen_buf[14] = s_dis_buff[0];
          g_screen_buf[13] = s_dis_buff[1];
          g_screen_buf[7]  = s_dis_buff[2];
          g_screen_buf[6]  = s_dis_buff[3];
          if (dot == 1)
          {
              g_screen_buf[7] |= 0x10;
          }
          else if(dot == 2)
          {
              g_screen_buf[13] |= 0x10;
          }
          break;
          
      case 0x03:        //���������� ʱ��
          g_screen_buf[0] = reversePlace(s_dis_buff[0]);
          g_screen_buf[1] = reversePlace(s_dis_buff[1]);
          g_screen_buf[2] = reversePlace(s_dis_buff[2]);
          g_screen_buf[3] = reversePlace(s_dis_buff[3]);
          if (dot == 1)
          {
              g_screen_buf[2] |= 0x01;  //����
          }
          break;
          
      default:
          break;
    }
}

/*******************************************************************************
*	Function name:	      reversePlace
*	Descriptions:	      HT1621�У��޸�������෴������
*	Parameters:	      �ߵ��ı���
*	Returned value:       �޸ĺ�ı���
*******************************************************************************/
u8 reversePlace(u8 positive)
{
    u8 reverse;
    
    reverse = (positive >> 4) | 0xf0;
    reverse &= ((positive << 4) | 0x0f);
    
    return(reverse);
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
            s_dis_buff[3] = 0x00;  //�˴���0λ��˸
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

