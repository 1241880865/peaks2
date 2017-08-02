


/* Includes ------------------------------------------------------------------*/
#include "stm8s_flash.h"

#define FLASH_CLEAR_BYTE ((uint8_t)0x00)
#define FLASH_SET_BYTE  ((uint8_t)0xFF)
#define OPERATION_TIMEOUT  ((uint32_t)0xFFFFF)

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void FLASH_Unlock(FLASH_MemType_TypeDef FLASH_MemType)
{
    /* Check parameter */
    assert_param(IS_MEMORY_TYPE_OK(FLASH_MemType));

    /* Unlock program memory */
    if (FLASH_MemType == FLASH_MEMTYPE_PROG)
    {
        FLASH->PUKR = FLASH_RASS_KEY1;
        FLASH->PUKR = FLASH_RASS_KEY2;
    }
    /* Unlock data memory */
    else
    {
        FLASH->DUKR = FLASH_RASS_KEY2; /* Warning: keys are reversed on data memory !!! */
        FLASH->DUKR = FLASH_RASS_KEY1;
    }
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void FLASH_Lock(FLASH_MemType_TypeDef FLASH_MemType)
{
    /* Check parameter */
    assert_param(IS_MEMORY_TYPE_OK(FLASH_MemType));

  /* Lock memory */
  FLASH->IAPSR &= (uint8_t)FLASH_MemType;
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void FLASH_DeInit(void)
{
    FLASH->CR1 = FLASH_CR1_RESET_VALUE;
    FLASH->CR2 = FLASH_CR2_RESET_VALUE;
    FLASH->NCR2 = FLASH_NCR2_RESET_VALUE;
    FLASH->IAPSR &= (uint8_t)(~FLASH_IAPSR_DUL);
    FLASH->IAPSR &= (uint8_t)(~FLASH_IAPSR_PUL);
    (void) FLASH->IAPSR; /* Reading of this register causes the clearing of status flags */
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void FLASH_ProgramByte(uint32_t Address, uint8_t Data)
{
    /* Check parameters */
    assert_param(IS_FLASH_ADDRESS_OK(Address));
    *(PointerAttr uint8_t*) (uint16_t)Address = Data;
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
uint8_t FLASH_ReadByte(uint32_t Address)
{
    /* Check parameter */
    assert_param(IS_FLASH_ADDRESS_OK(Address));
    
    /* Read byte */
    return(*(PointerAttr uint8_t *) (uint16_t)Address);
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void FLASH_SetProgrammingTime(FLASH_ProgramTime_TypeDef FLASH_ProgTime)
{
    /* Check parameter */
    assert_param(IS_FLASH_PROGRAM_TIME_OK(FLASH_ProgTime));

    FLASH->CR1 &= (uint8_t)(~FLASH_CR1_FIX);
    FLASH->CR1 |= (uint8_t)FLASH_ProgTime;
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
IN_RAM(FLASH_Status_TypeDef FLASH_WaitForLastOperation(FLASH_MemType_TypeDef FLASH_MemType)) 
{
    uint8_t flagstatus = 0x00;
    uint32_t timeout = OPERATION_TIMEOUT;
    
    /* Wait until operation completion or write protection page occurred */
    if (FLASH_MemType == FLASH_MEMTYPE_PROG)
    {
        while ((flagstatus == 0x00) && (timeout != 0x00))
        {
            flagstatus = (uint8_t)(FLASH->IAPSR & (uint8_t)(FLASH_IAPSR_EOP |
                                              FLASH_IAPSR_WR_PG_DIS));
            timeout--;
        }
    }
    else
    {
        while ((flagstatus == 0x00) && (timeout != 0x00))
        {
            flagstatus = (uint8_t)(FLASH->IAPSR & (uint8_t)(FLASH_IAPSR_HVOFF |
                                              FLASH_IAPSR_WR_PG_DIS));
            timeout--;
        }
    }
    
    if (timeout == 0x00)
    {
        flagstatus = FLASH_STATUS_TIMEOUT;
    }
    
    return((FLASH_Status_TypeDef)flagstatus);
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
