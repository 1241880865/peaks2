


#ifndef __EEPROMCTRL_H
#define __EEPROMCTRL_H

#include "EEPROM.h"

/*------------------------------------------------------------------------------
********************************* 函数 声明 ************************************
------------------------------------------------------------------------------*/
extern void FactoryInit(void);  //出厂化函数
extern void DataFirstInit(void);//首次出厂化函数
extern void ReadEEprom(void);   //上电读数据

//掉电保护
extern void PwrSave(void);
extern void ReadProtect(void);

extern void WriteTempSet(void);
extern void WriteSpeedSet(void);
extern void WritePara1ToEE(void);
extern void WritePara2ToEE(void);
extern void WritePara5ToEE(void);
extern void WriteSmplToEE(void);
#endif


