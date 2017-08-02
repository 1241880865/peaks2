


#ifndef KEYDIS__H
#define KEYDIS__H

#include "BT3153.h"


typedef struct
{
    PARARANGE* p_tab;
    s16 dis_buf;
}
DISPLAYBUF;
extern DISPLAYBUF m_disp;

extern PARARANGE c_tab1[8];
extern PARARANGE c_tab2[6];
extern PARARANGE c_tab3[1];
extern PARARANGE c_tab4[1];
extern PARARANGE c_tab5[8];
extern PARARANGE c_tab_smpl[2];


//时间变量定义
extern u16 TimMs_DelaySetting;
extern u16 TimMs_FlashDelay;
extern u16 TimMs_LampFlash;
extern u16 TimMs_ALMFlash;  //电机故障闪烁
extern u16 TimMs_ATFlash;   //自整定闪烁

//函数
void Display(void);   //显示函数
void DisplayPwr(void);//上电显示状态

#endif
