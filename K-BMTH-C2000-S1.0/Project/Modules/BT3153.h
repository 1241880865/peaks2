

#ifndef  HDS3058__H
#define  HDS3058__H

#include "HT1621.h"

/*------------------------------------------------------------------------------
****************************  LCD��ʾ��ĸ  �궨�� ******************************
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
*****************************  LCDָʾ��  �궨�� *******************************
------------------------------------------------------------------------------*/
//�Ҳ��У�ָʾ��
#define DFL_M      0
#define DFL_RPM1   1
#define DFL_F      2
#define DFL_RPM2   3
#define DFL_H      4
#define DFL_C1     5
#define DFL_C2     6
//���ţ�ָʾ��
#define DFL_HEAT   7
#define DFL_OUT    8
#define DFL_AT     9
#define DFL_MAIN  10
#define DFL_ALM   11
#define DFL_COOL  12
//����У�ָʾ��
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
//����
#define BUFF_NUM  11
extern u8 g_screen_buf[BUFF_NUM]; //HT1621��������

//ָʾ��
typedef const struct
{
    u8 array_num;
    u8 array_val;
}
STLAMP_CS;
extern STLAMP_CS StLamp[23];
#define DisplayLit(a) g_screen_buf[(StLamp + a)->array_num] |= (StLamp + a)->array_val

//��ĸ��ʾ������
typedef const struct
{
    u8  dis_dot;  //С����λ
    s16 limit_dn; //��Χ����
    s16 limit_up; //��Χ����
    u8  dis_a;    //��ʾ��ĸ
    u8  dis_b;
    u8  dis_c;
    u8  dis_d;
}
PARARANGE;

//������ʾ
typedef struct
{
    s16 num;        //Ҫ��ʾ��ֵ
    u8  dot;        //С����λ��
    u8  flag_flash; //�Ƿ���˸   1:�����һλ�� 2:ȫ����˸��
    u8  flashBit;   //��λ��˸λ
    u8  flag_hide_0;//�Ƿ�����
}
DISPLAYGNUM;

//ʱ�����
extern u16  TimMs_DisDelay;

//����
extern void DisplaySglABC(u8 a, u8 b, u8 c, u8 d, u8 row);
extern void DisplayABC(PARARANGE* d, u8 row);
extern void DisplayNum(DISPLAYGNUM* d, u8 row);
extern void DisplayHCD(s16 dec_num, u8 hcd_row);
extern void SendToScreen(void);


#endif

