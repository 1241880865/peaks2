

#ifndef MID_AD_H
#define MID_AD_H


#define  AD_NUM     2     //共7路ad采样

extern u16 gAdBuff[16][AD_NUM];//AD采样值
extern u8  gAdOk;              //采样滤波完成标志

//函数
extern void AdCalc(void);

#endif
