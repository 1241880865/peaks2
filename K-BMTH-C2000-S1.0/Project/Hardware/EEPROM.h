
/*******************************************************************************
                                      EEPROM
1、分配 EEPROM 内存空间
2、直接调用 写入 读出 函数
*******************************************************************************/
#ifndef __EEPROM_H
#define __EEPROM_H

/*------------------------------------------------------------------------------
**************************** EE地址分配 宏定义 *********************************
------------------------------------------------------------------------------*/
//EEPROM空间分配，偏移地址宏定义（16进制地址）:
#define EE_FACTORY     0     // 出厂化指令，2字节
#define EE_TEMPSETVAL  2     // 温度设定值，2字节
#define EE_SPDSETVAL   4     // 速度设定值，2字节

#define EE_TEMPARAONE  10    // 温度参数表一，16字节
#define EE_TEMPARATWO  30    // 温度参数表二，12字节
#define EE_SPDPARA     50    // 速度参数，16字节

#define EE_TEMPSAMPLE  70    // 温度采样参数，8字节

#define EE_PWROFF      80    // 掉电保护参数，1字节

/*------------------------------------------------------------------------------
*********************************** 函数声明 ***********************************
------------------------------------------------------------------------------*/
//入口参数说明：变量存放地址，EEPROM偏移地址宏定义，字节数(sizeof(入口变量))
void EEpromInit(void);
void EEpromWrite(u8 *eepoint, u8 offset, u8 count);
void EEpromRead(u8 *eepoint, u8 offset, u8 count);

#endif


