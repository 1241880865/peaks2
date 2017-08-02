


/*------------------------------------------------------------------------------
***********************************头文件包含***********************************
------------------------------------------------------------------------------*/
#include "stm8s.h"
#include "main.h"
#include "MotorDriver.h"
#include "heatCtrl.h"
/*******************************************************************************
*	Function name:	      FactoryInit
*	Descriptions:	      First factory of parameters
*	Parameters:	      void
*	Returned value:       void
*******************************************************************************/
void FactoryInit(void)
{
    //温度设定值
    TempSetVal = 300;
    EEpromWrite((u8 *)(&TempSetVal), EE_TEMPSETVAL, 2);
    
    //速度设定值
    SpdSetVal = 500;
    EEpromWrite((u8 *)(&SpdSetVal), EE_SPDSETVAL, 2);
    
    //表一  温度1参数
    ParaTempPID.alh = 100;
    ParaTempPID.Temp_t = 5;
    ParaTempPID.Temp_p = 300;
    ParaTempPID.Temp_i = 200;
    ParaTempPID.Temp_d = 200;
    ParaTempPID.pb = 0;
    ParaTempPID.pl = 0;
    ParaTempPID.adr = 1;
    EEpromWrite((u8 *)(&ParaTempPID), EE_TEMPARAONE, sizeof(ParaTempPID));
    
    //表二  温度2参数
    ParaTempCtrl.pon = 0;
    ParaTempCtrl.rut = 0;
    ParaTempCtrl.bl = 4000;
    ParaTempCtrl.np = 100;
    ParaTempCtrl.spl = 0;
    ParaTempCtrl.sph = 4000;
    EEpromWrite((u8 *)(&ParaTempCtrl), EE_TEMPARATWO, sizeof(ParaTempCtrl));
    
    //表三  速度1参数
    ParaSpd.pd = 40;
    ParaSpd.id = 2;
    ParaSpd.intt = 10;
    ParaSpd.det = 10;
    ParaSpd.sdl = 200;
    ParaSpd.sdh = 2000;
    ParaSpd.pol = 1;
    ParaSpd.db = 5;
    EEpromWrite((u8 *)(&ParaSpd), EE_SPDPARA, sizeof(ParaSpd));
}

/*******************************************************************************
*	Function name:	      DataFirstInit
*	Descriptions:	      The main function
*	Parameters:	      void
*	Returned value:       void
*******************************************************************************/
void DataFirstInit(void)
{
    u16 temp;
    
    //读取EEPROM写入标志字节
    EEpromRead((u8 *)(&temp), EE_FACTORY, 2);
    if (temp != 0x55AA) //仅在第一次下程序时，执行
    {
        //首次出厂化标志
        temp = 0x55AA;
        EEpromWrite((u8 *)(&temp), EE_FACTORY, 2);
        
        //出厂化参数赋值
        FactoryInit();
    }
}

/*******************************************************************************
	Function name:	      ReadEEprom
	Descriptions:	      Read the set data
	Parameters:	      void
	Returned value:       void
*******************************************************************************/
void ReadEEprom(void)  //上电读数据
{
    //温度设定值
    EEpromRead((u8 *)(&TempSetVal), EE_TEMPSETVAL, 2);
    
    //速度设定值
    EEpromRead((u8 *)(&SpdSetVal), EE_SPDSETVAL, 2);
    
    //表一  温度1参数
    EEpromRead((u8 *)(&ParaTempPID), EE_TEMPARAONE, sizeof(ParaTempPID));
    
    //表二  温度2参数
    EEpromRead((u8 *)(&ParaTempCtrl), EE_TEMPARATWO, sizeof(ParaTempCtrl));
    
    //表三 速度参数
    EEpromRead((u8 *)(&ParaSpd), EE_SPDPARA, sizeof(ParaSpd));
    
    //读温度采样参数
    EEpromRead((u8 *)(&Pt100Std), EE_TEMPSAMPLE, 4);
}

/*******************************************************************************
*	Function name:	      PwrSave
*	Descriptions:	      power-off save
*	Parameters:	      void
*	Returned value:       void
*******************************************************************************/
void PwrSave(void)
{
    EEpromWrite((u8 *)(&CtrlFlag.flgRun), EE_PWROFF, 1);
}

/*******************************************************************************
	Function name:	      ReadProtect
	Descriptions:	      Read the power off data
	Parameters:	      void
	Returned value:       void
*******************************************************************************/
void ReadProtect(void)
{
    if (ParaTempCtrl.pon == 1)
    {
        EEpromRead((u8 *)(&CtrlFlag.flgRun), EE_PWROFF, 1);
    }
}

void WriteTempSet(void)
{
    EEpromWrite((u8 *)(&TempSetVal), EE_TEMPSETVAL, 2);
}

void WriteSpeedSet(void)
{
    EEpromWrite((u8 *)(&SpdSetVal), EE_SPDSETVAL, 2);
}

void WritePara1ToEE(void)
{
    EEpromWrite((u8 *)(&ParaTempPID), EE_TEMPARAONE, sizeof(ParaTempPID));
}

void WritePara2ToEE(void)
{
    EEpromWrite((u8 *)(&ParaTempCtrl), EE_TEMPARATWO, sizeof(ParaTempCtrl));
}

void WritePara5ToEE(void)
{
    EEpromWrite((u8 *)(&ParaSpd), EE_SPDPARA, sizeof(ParaSpd));
}

void WriteSmplToEE(void)
{
    EEpromWrite((u8 *)(&Pt100Std), EE_TEMPSAMPLE, 4);
}
