


#ifndef __EEPROMCTRL_H
#define __EEPROMCTRL_H

#include "EEPROM.h"

/*------------------------------------------------------------------------------
********************************* ���� ���� ************************************
------------------------------------------------------------------------------*/
extern void FactoryInit(void);  //����������
extern void DataFirstInit(void);//�״γ���������
extern void ReadEEprom(void);   //�ϵ������

//���籣��
extern void PwrSave(void);
extern void ReadProtect(void);

extern void WriteTempSet(void);
extern void WriteSpeedSet(void);
extern void WritePara1ToEE(void);
extern void WritePara2ToEE(void);
extern void WritePara5ToEE(void);
extern void WriteSmplToEE(void);
#endif


