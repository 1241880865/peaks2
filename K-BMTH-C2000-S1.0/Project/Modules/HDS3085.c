

#include "stm8s.h"
#include "HDS3085.h"

/*----------------------------------------------------------------------------*/
//显示代码
#define HIDE_ZERO  0xAF //消零显示，"0"值为0xAF
const u8 c_code[] =
{
    /*0,  1,   2,   3,   4,   5,   6,   7,   8,   9 */
    0xAF,0xA0,0xCB,0xE9,0xE4,0x6D,0x6F,0xA8,0xEF,0xED
};

/*----------------------------------------------------------------------------*/
STLAMP_CS StLamp[25] =
{
//第一列标识符
    {12,0x10},//"PV"
    {14,0x10},//"TEMP"
    {8, 0x40},//"SV"
    {8, 0x20},//"SPEED2"
    {4, 0x20},//"SPEED3"
    {4, 0x10},//"VOL"
    {0, 0x01},//"TIME"
    {4, 0x40},//"A正转"
    {4, 0x80},//"B反转"
//第二列标识符
    {8, 0x80},//"℃1"
    {0, 1},//"F" 11 0x10
    {5, 0x80},//"℃2"
    {6, 0x10},//"RPM2"
    {4, 0x08},//"RPM3"
    {4, 0x04},//"V"
    {3, 0x01},//"M"
//第三列标识符
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
u8 g_screen_buf[BUFF_NUM];//HT1621缓存数组
static u8 s_dis_buff[4];  //显示中间数组

//时间变量
u16 TimMs_DisDelay;

//函数
static u8 reversePlace(u8 positive);
static void updateScreenBuf(u8 dot, u8 row);
static u8 convertHCD(u8 ad_num);
/*******************************************************************************
** Function name:  SendToScreen
** Descriptions:   发送至屏幕
** Input:          无
** Output:         无
** Return:         无
*******************************************************************************/
void SendToScreen(void)
{
    DisplayToHT1621(g_screen_buf, BUFF_NUM);
}

/*******************************************************************************
** Function name:  updateScreenBuf
** Descriptions:   赋值到缓冲数组
** Input:          显示 小数点 排数
** Output:         缓冲数组
** Return:         无
*******************************************************************************/
void updateScreenBuf(u8 dot, u8 row)
{
    switch (row)
    {
      case 0x01:        //赋给第一行 温度
          g_screen_buf[12] = s_dis_buff[0];
          g_screen_buf[11] = s_dis_buff[1];
          g_screen_buf[10] = s_dis_buff[2];
          g_screen_buf[9]  = s_dis_buff[3];
          if (dot == 1)
          {
              g_screen_buf[9] |= 0x10;
          }
          break;
          
      case 0x02:        //赋给第二行 速度
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
          
      case 0x03:        //赋给第三行 时间
          g_screen_buf[0] = reversePlace(s_dis_buff[0]);
          g_screen_buf[1] = reversePlace(s_dis_buff[1]);
          g_screen_buf[2] = reversePlace(s_dis_buff[2]);
          g_screen_buf[3] = reversePlace(s_dis_buff[3]);
          if (dot == 1)
          {
              g_screen_buf[2] |= 0x01;  //反码
          }
          break;
          
      default:
          break;
    }
}

/*******************************************************************************
*	Function name:	      reversePlace
*	Descriptions:	      HT1621中，修改与编码相反的数组
*	Parameters:	      颠倒的编码
*	Returned value:       修改后的编码
*******************************************************************************/
u8 reversePlace(u8 positive)
{
    u8 reverse;
    
    reverse = (positive >> 4) | 0xf0;
    reverse &= ((positive << 4) | 0x0f);
    
    return(reverse);
}

/*******************************************************************************
                           ---- 通用（显示应用层） ----
*******************************************************************************/
/*******************************************************************************
** Function name:  displaySglAbc
** Descriptions:   显示固定字符
** Input:          四位显示 a b c d, 显示行数（共两行）
** Output:         无
** Return:         无
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
** Descriptions:   显示字母
** Input:          显示 行数
** Output:         无
** Return:         无
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
** Descriptions:   转换成BCD码显示
** Input:          1、十进制显示数；2、显示的行数；
** Output:         无
** Return:         无
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
** Descriptions:   BCD码转换
** Input:          十进制显示数
** Output:         无
** Return:         16进制显示数码
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
** Descriptions:   十进制显示
** Input:          显示 排数
** Output:         无
** Return:         无
*******************************************************************************/
void DisplayNum(DISPLAYGNUM* d, u8 row)
{
    u8 flag_negative; //负数
    s16 number;
    
    /* 取数值 */
    number = d->num;
    
    /* 负数 */
    if(number < 0)
    {
        flag_negative = 1;
        number = 0 - number;
    }
    else flag_negative = 0;
    
    /* 数字各位分离 */
    s_dis_buff[0] = *(c_code + number / 1000);         //分离千位
    s_dis_buff[1] = *(c_code + (number % 1000) / 100); //分离百位
    s_dis_buff[2] = *(c_code + (number % 100) / 10);   //分离十位
    s_dis_buff[3] = *(c_code + (number % 10));         //分离个位
    
    /* 显示消零 */
    if(d->flag_hide_0 != 0)
    {
        if (s_dis_buff[0] == HIDE_ZERO) // 0："HIDE_ZERO"
        {
            s_dis_buff[0] = 0x00;  //不显示："0x00"
            if (s_dis_buff[1] == HIDE_ZERO)
            {
                s_dis_buff[1] = 0x00;
                if ((s_dis_buff[2] == HIDE_ZERO) && (d->dot == 0))//倒数第二位为零，且无小数
                {
                    s_dis_buff[2] = 0x00;
                }
            }
        }
    }
    
    /* 负数显示 */
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
    
    /* 移位闪烁 */
    if(d->flag_flash == 1)
    {
        if(TimMs_DisDelay < 220)
        {
            s_dis_buff[3] = 0x00;  //此处仅0位闪烁
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
            s_dis_buff[0] = 0x00;  //全部闪烁
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
** Descriptions:   显示数字
** Input:          显示量  排数
** Output:         显示缓冲区
** Return:         无
*******************************************************************************/
//void display_num(DISPLAYGNUM disp_num, u8 flag_dec_hex, u8 row)
//{
//    if(flag_dec_hex != 0)
//    {
//        /* 十六进制显示 */
//        //DispHex(d, row);
//    }
//    else
//    {
//        /* 十进制显示 */
//        display_num_dec(disp_num, row);
//    }
//}

