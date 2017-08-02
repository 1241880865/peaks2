

#ifndef KEY__H
#define KEY__H

//按键状态--menu取值
#define MENU_NONE   0x00
#define MENU_SET    0x01
#define MENU_LGSET  0x02
#define MENU_LC     0x03
#define MENU_LO     0x04
#define MENU_SMPL   0x05

//按键状态--menu2取值（与password相同）
#define MENU2_PARA_NONE 0
#define MENU2_PARA_ONE  3
#define MENU2_PARA_TWO  9
#define MENU2_PARA_THR  567
#define MENU2_PARA_FOUR 27
#define MENU2_PARA_FIVE 103

//采样状态
#define SMPL_MENU_TEMP  11


typedef struct
{
    u8  menu;
    u16 menu2;
    u16 password;
    u8  index;
}
KEYDESTRUCT;

extern KEYDESTRUCT m_key;//按键状态
extern u8 g_changing;    //增加or减小标志
extern u8 g_moveBit;     //移位

//时间变量
extern u16 TimMs_keyLgset;
extern u8  TimMs_Jitter;
extern u8  TimMs_KeyDelayAdd;

//函数
extern void ScanKey(void);

#endif

