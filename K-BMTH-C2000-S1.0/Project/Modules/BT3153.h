

#ifndef  HDS3058__H
#define  HDS3058__H

#include "HT1621.h"

/*------------------------------------------------------------------------------
****************************  LCD显示字母  宏定义 ******************************
------------------------------------------------------------------------------*/
#define DF__  0x00 //" "
#define DF_1  0x40 //"-"
#define DF_A  0xEE
#define DF_b  0x67
#define DF_C  0x0F
#define DF_c  0x43
#define DF_d  0xE3
#define DF_E  0x4F
#define DF_F  0x4E
#define DF_H  0xE6
#define DF_I  0x06
#define DF_k  0xc7
#define DF_L  0x07
#define DF_n  0x62
#define DF_o  0x63
#define DF_P  0xCE
#define DF_r  0x42
#define DF_S  0x6D
//#define DF_t  0x47
#define DF_T  0x0E
#define DF_u  0x23
#define DF_y  0xE5

/*------------------------------------------------------------------------------
*****************************  LCD指示灯  宏定义 *******************************
------------------------------------------------------------------------------*/
//右侧列（指示）
#define DFL_M      0
#define DFL_RPM1   1
#define DFL_F      2
#define DFL_RPM2   3
#define DFL_H      4
#define DFL_C1     5
#define DFL_C2     6
//上排（指示）
#define DFL_HEAT   7
#define DFL_OUT    8
#define DFL_AT     9
#define DFL_MAIN  10
#define DFL_ALM   11
#define DFL_COOL  12
//左侧列（指示）
#define DFL_TIME  13
#define DFL_SET   14
#define DFL_PV    15
#define DFL_SV1   16
#define DFL_A     17
#define DFL_B     18
#define DFL_TEMP  19
#define DFL_SV2   20
#define DFL_STOP  21
#define DFL_RUN   22

//DisplayLit(DFL_SET);

/*------------------------------------------------------------------------------*/
//变量
#define BUFF_NUM  11
extern u8 g_screen_buf[BUFF_NUM]; //HT1621缓存数组

//指示灯
typedef const struct
{
    u8 array_num;
    u8 array_val;
}
STLAMP_CS;
extern STLAMP_CS StLamp[23];
#define DisplayLit(a) g_screen_buf[(StLamp + a)->array_num] |= (StLamp + a)->array_val

//字母显示及属性
typedef const struct
{
    u8  dis_dot;  //小数点位
    s16 limit_dn; //范围下限
    s16 limit_up; //范围上限
    u8  dis_a;    //显示字母
    u8  dis_b;
    u8  dis_c;
    u8  dis_d;
}
PARARANGE;

//数字显示
typedef struct
{
    s16 num;        //要显示数值
    u8  dot;        //小数点位置
    u8  flag_flash; //是否闪烁   1:闪最后一位； 2:全部闪烁；
    u8  flashBit;   //移位闪烁位
    u8  flag_hide_0;//是否消零
}
DISPLAYGNUM;

//时间变量
extern u16  TimMs_DisDelay;

//函数
extern void DisplaySglABC(u8 a, u8 b, u8 c, u8 d, u8 row);
extern void DisplayABC(PARARANGE* d, u8 row);
extern void DisplayNum(DISPLAYGNUM* d, u8 row);
extern void DisplayHCD(s16 dec_num, u8 hcd_row);
extern void SendToScreen(void);


#endif

