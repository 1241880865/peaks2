


/*------------------------------------------------------------------------------
***********************************头文件包含***********************************
------------------------------------------------------------------------------*/
#include "stm8s.h"
#include "EEPROM.h"
/*******************************************************************************
	Function name:	      EEpromInit
	Descriptions:	      EEprom initialization
	Parameters:	      void
	Returned value:       void
*******************************************************************************/
void EEpromInit(void)
{
    FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
}

/*******************************************************************************
	Function name:	      EEpromWrite
	Descriptions:	      Write Parameter values to EEPROM
	Parameters:	      1、要写入变量地址
                              2、写入EEPROM中偏移地址
                              3、字节数
	Returned value:       void
*******************************************************************************/
void EEpromWrite(u8 *eepoint, u8 zone, u8 count)
{
    u8 i;   //计数
    
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    
    for (i = 0; i < count; i++)
    {
        FLASH_ProgramByte(FLASH_DATA_START_PHYSICAL_ADDRESS + zone + i, *(eepoint + i));
        FLASH_WaitForLastOperation(FLASH_MEMTYPE_DATA);  //rww
    }
    
    FLASH_Lock(FLASH_MEMTYPE_DATA);
}

/*******************************************************************************
	Function name:	      EEpromRead
	Descriptions:	      Read Parameter value from EEPROM
	Parameters:	      1、读出内容存放地址
                              2、变量在EEPROM中偏移地址
                              3、字节数
	Returned value:       void
*******************************************************************************/
void EEpromRead(u8 *eepoint, u8 offset, u8 count)
{
    u8 i;
  
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    
    for (i = 0; i < count; i++)
    {
        *(eepoint + i) = FLASH_ReadByte(FLASH_DATA_START_PHYSICAL_ADDRESS + offset + i);
        FLASH_WaitForLastOperation(FLASH_MEMTYPE_DATA);/*等待数据读/写完成*/
    }
    
    FLASH_Lock(FLASH_MEMTYPE_DATA);
}
