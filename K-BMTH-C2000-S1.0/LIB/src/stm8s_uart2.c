

/* Includes ------------------------------------------------------------------*/
#include "stm8s_uart2.h"

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void UART2_DeInit(void)
{
    /*  Clear the Idle Line Detected bit in the status register by a read
       to the UART2_SR register followed by a Read to the UART2_DR register */
    (void) UART2->SR;
    (void)UART2->DR;

    UART2->BRR2 = UART2_BRR2_RESET_VALUE;  /*  Set UART2_BRR2 to reset value 0x00 */
    UART2->BRR1 = UART2_BRR1_RESET_VALUE;  /*  Set UART2_BRR1 to reset value 0x00 */

    UART2->CR1 = UART2_CR1_RESET_VALUE; /*  Set UART2_CR1 to reset value 0x00  */
    UART2->CR2 = UART2_CR2_RESET_VALUE; /*  Set UART2_CR2 to reset value 0x00  */
    UART2->CR3 = UART2_CR3_RESET_VALUE; /*  Set UART2_CR3 to reset value 0x00  */
    UART2->CR4 = UART2_CR4_RESET_VALUE; /*  Set UART2_CR4 to reset value 0x00  */
    UART2->CR5 = UART2_CR5_RESET_VALUE; /*  Set UART2_CR5 to reset value 0x00  */
    UART2->CR6 = UART2_CR6_RESET_VALUE; /*  Set UART2_CR6 to reset value 0x00  */

}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void UART2_Init(uint32_t BaudRate, UART2_WordLength_TypeDef WordLength, UART2_StopBits_TypeDef StopBits, UART2_Parity_TypeDef Parity, UART2_SyncMode_TypeDef SyncMode, UART2_Mode_TypeDef Mode)
{
    uint8_t BRR2_1 = 0, BRR2_2 = 0;
    uint32_t BaudRate_Mantissa = 0, BaudRate_Mantissa100 = 0;

    /* Check the parameters */
    //assert_param(IS_UART2_BAUDRATE_OK(BaudRate));
//    assert_param(IS_UART2_WORDLENGTH_OK(WordLength));
//    assert_param(IS_UART2_STOPBITS_OK(StopBits));
//    assert_param(IS_UART2_PARITY_OK(Parity));
//    assert_param(IS_UART2_MODE_OK((uint8_t)Mode));
//    assert_param(IS_UART2_SYNCMODE_OK((uint8_t)SyncMode));

     /* Clear the word length bit */
    UART2->CR1 &= (uint8_t)(~UART2_CR1_M);
    /* Set the word length bit according to UART2_WordLength value */
    UART2->CR1 |= (uint8_t)WordLength; 

    /* Clear the STOP bits */
    UART2->CR3 &= (uint8_t)(~UART2_CR3_STOP);
    /* Set the STOP bits number according to UART2_StopBits value  */
    UART2->CR3 |= (uint8_t)StopBits; 

    /* Clear the Parity Control bit */
    UART2->CR1 &= (uint8_t)(~(UART2_CR1_PCEN | UART2_CR1_PS  ));
    /* Set the Parity Control bit to UART2_Parity value */
    UART2->CR1 |= (uint8_t)Parity;

    /* Clear the LSB mantissa of UART2DIV  */
    UART2->BRR1 &= (uint8_t)(~UART2_BRR1_DIVM);
    /* Clear the MSB mantissa of UART2DIV  */
    UART2->BRR2 &= (uint8_t)(~UART2_BRR2_DIVM);
    /* Clear the Fraction bits of UART2DIV */
    UART2->BRR2 &= (uint8_t)(~UART2_BRR2_DIVF);

    /* Set the UART2 BaudRates in BRR1 and BRR2 registers according to UART2_BaudRate value */
    BaudRate_Mantissa    = ((uint32_t)CLK_GetClockFreq() / (BaudRate << 4)); //16000000
    BaudRate_Mantissa100 = (((uint32_t)CLK_GetClockFreq() * 100) / (BaudRate << 4));
    
    /* The fraction and MSB mantissa should be loaded in one step in the BRR2 register*/
    /* Set the fraction of UARTDIV  */
    BRR2_1 = (uint8_t)((uint8_t)(((BaudRate_Mantissa100 - (BaudRate_Mantissa * 100))
                        << 4) / 100) & (uint8_t)0x0F); 
    BRR2_2 = (uint8_t)((BaudRate_Mantissa >> 4) & (uint8_t)0xF0);

    UART2->BRR2 = (uint8_t)(BRR2_1 | BRR2_2);
    /* Set the LSB mantissa of UARTDIV  */
    UART2->BRR1 = (uint8_t)BaudRate_Mantissa;

    /* Disable the Transmitter and Receiver before seting the LBCL, CPOL and CPHA bits */
    UART2->CR2 &= (uint8_t)~(UART2_CR2_TEN | UART2_CR2_REN);
    /* Clear the Clock Polarity, lock Phase, Last Bit Clock pulse */
    UART2->CR3 &= (uint8_t)~(UART2_CR3_CPOL | UART2_CR3_CPHA | UART2_CR3_LBCL);
    /* Set the Clock Polarity, lock Phase, Last Bit Clock pulse */
    UART2->CR3 |= (uint8_t)((uint8_t)SyncMode & (uint8_t)(UART2_CR3_CPOL | \
                                              UART2_CR3_CPHA | UART2_CR3_LBCL));

    if ((uint8_t)(Mode & UART2_MODE_TX_ENABLE))
    {
        /* Set the Transmitter Enable bit */
        UART2->CR2 |= (uint8_t)UART2_CR2_TEN;
    }
    else
    {
        /* Clear the Transmitter Disable bit */
        UART2->CR2 &= (uint8_t)(~UART2_CR2_TEN);
    }
    if ((uint8_t)(Mode & UART2_MODE_RX_ENABLE))
    {
        /* Set the Receiver Enable bit */
        UART2->CR2 |= (uint8_t)UART2_CR2_REN;
    }
    else
    {
        /* Clear the Receiver Disable bit */
        UART2->CR2 &= (uint8_t)(~UART2_CR2_REN);
    }
    /* Set the Clock Enable bit, lock Polarity, lock Phase and Last Bit Clock 
           pulse bits according to UART2_Mode value */
    if ((uint8_t)(SyncMode & UART2_SYNCMODE_CLOCK_DISABLE))
    {
        /* Clear the Clock Enable bit */
        UART2->CR3 &= (uint8_t)(~UART2_CR3_CKEN); 
    }
    else
    {
        UART2->CR3 |= (uint8_t)((uint8_t)SyncMode & UART2_CR3_CKEN);
    }
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void UART2_Cmd(FunctionalState NewState)
{

    if (NewState != DISABLE)
    {
        /* UART2 Enable */
        UART2->CR1 &= (uint8_t)(~UART2_CR1_UARTD);
    }
    else
    {
        /* UART2 Disable */
        UART2->CR1 |= UART2_CR1_UARTD; 
    }
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void UART2_ITConfig(UART2_IT_TypeDef UART2_IT, FunctionalState NewState)
{
    uint8_t uartreg = 0, itpos = 0x00;
    
    /* Check the parameters */
//    assert_param(IS_UART2_CONFIG_IT_OK(UART2_IT));
//    assert_param(IS_FUNCTIONALSTATE_OK(NewState));

    /* Get the UART2 register index */
    uartreg = (uint8_t)((uint16_t)UART2_IT >> 0x08);

    /* Get the UART2 IT index */
    itpos = (uint8_t)((uint8_t)1 << (uint8_t)((uint8_t)UART2_IT & (uint8_t)0x0F));

    if (NewState != DISABLE)
    {
        /* Enable the Interrupt bits according to UART2_IT mask */
        if (uartreg == 0x01)
        {
            UART2->CR1 |= itpos;
        }
        else if (uartreg == 0x02)
        {
            UART2->CR2 |= itpos;
        }
        else if (uartreg == 0x03)
        {
            UART2->CR4 |= itpos;
        }
        else
        {
            UART2->CR6 |= itpos;
        }
    }
    else
    {
        /* Disable the interrupt bits according to UART2_IT mask */
        if (uartreg == 0x01)
        {
            UART2->CR1 &= (uint8_t)(~itpos);
        }
        else if (uartreg == 0x02)
        {
            UART2->CR2 &= (uint8_t)(~itpos);
        }
        else if (uartreg == 0x03)
        {
            UART2->CR4 &= (uint8_t)(~itpos);
        }
        else
        {
            UART2->CR6 &= (uint8_t)(~itpos);
        }
    }
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
uint16_t UART2_ReceiveData9(void)
{
  uint16_t temp = 0;

  temp = ((uint16_t)(((uint16_t)((uint16_t)UART2->CR1 & (uint16_t)UART2_CR1_R8)) << 1));
  
  return (uint16_t)((((uint16_t)UART2->DR) | temp) & ((uint16_t)0x01FF));
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void UART2_SendData9(uint16_t Data)
{
    /* Clear the transmit data bit 8 */
    UART2->CR1 &= ((uint8_t)~UART2_CR1_T8);                  
    
    /* Write the transmit data bit [8] */
    UART2->CR1 |= (uint8_t)(((uint8_t)(Data >> 2)) & UART2_CR1_T8); 
    
    /* Write the transmit data bit [0:7] */
    UART2->DR   = (uint8_t)(Data);
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
FlagStatus UART2_GetFlagStatus(UART2_Flag_TypeDef UART2_FLAG)
{
    FlagStatus status = RESET;

    /* Check parameters */
    //assert_param(IS_UART2_FLAG_OK(UART2_FLAG));

    /* Check the status of the specified UART2 flag*/
    if (UART2_FLAG == UART2_FLAG_LBDF)
    {
        if ((UART2->CR4 & (uint8_t)UART2_FLAG) != (uint8_t)0x00)
        {
            /* UART2_FLAG is set*/
            status = SET;
        }
        else
        {
            /* UART2_FLAG is reset*/
            status = RESET;
        }
    }
    else if (UART2_FLAG == UART2_FLAG_SBK)
    {
        if ((UART2->CR2 & (uint8_t)UART2_FLAG) != (uint8_t)0x00)
        {
            /* UART2_FLAG is set*/
            status = SET;
        }
        else
        {
            /* UART2_FLAG is reset*/
            status = RESET;
        }
    }
    else if ((UART2_FLAG == UART2_FLAG_LHDF) || (UART2_FLAG == UART2_FLAG_LSF))
    {
        if ((UART2->CR6 & (uint8_t)UART2_FLAG) != (uint8_t)0x00)
        {
            /* UART2_FLAG is set*/
            status = SET;
        }
        else
        {
            /* UART2_FLAG is reset*/
            status = RESET;
        }
    }
    else
    {
        if ((UART2->SR & (uint8_t)UART2_FLAG) != (uint8_t)0x00)
        {
            /* UART2_FLAG is set*/
            status = SET;
        }
        else
        {
            /* UART2_FLAG is reset*/
            status = RESET;
        }
    }

    /* Return the UART2_FLAG status*/
    return  status;
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void UART2_ClearFlag(UART2_Flag_TypeDef UART2_FLAG)
{
    //assert_param(IS_UART2_CLEAR_FLAG_OK(UART2_FLAG));

    /*  Clear the Receive Register Not Empty flag */
    if (UART2_FLAG == UART2_FLAG_RXNE)
    {
        UART2->SR = (uint8_t)~(UART2_SR_RXNE);
    }
    /*  Clear the LIN Break Detection flag */
    else if (UART2_FLAG == UART2_FLAG_LBDF)
    {
        UART2->CR4 &= (uint8_t)(~UART2_CR4_LBDF);
    }
    /*  Clear the LIN Header Detection Flag */
    else if (UART2_FLAG == UART2_FLAG_LHDF)
    {
        UART2->CR6 &= (uint8_t)(~UART2_CR6_LHDF);
    }
    /*  Clear the LIN Synch Field flag */
    else
    {
        UART2->CR6 &= (uint8_t)(~UART2_CR6_LSF);
    }
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
ITStatus UART2_GetITStatus(UART2_IT_TypeDef UART2_IT)
{
    ITStatus pendingbitstatus = RESET;
    uint8_t itpos = 0;
    uint8_t itmask1 = 0;
    uint8_t itmask2 = 0;
    uint8_t enablestatus = 0;

    /* Check parameters */
    //assert_param(IS_UART2_GET_IT_OK(UART2_IT));

    /* Get the UART2 IT index*/
    itpos = (uint8_t)((uint8_t)1 << (uint8_t)((uint8_t)UART2_IT & (uint8_t)0x0F));
    /* Get the UART2 IT index*/
    itmask1 = (uint8_t)((uint8_t)UART2_IT >> (uint8_t)4);
    /* Set the IT mask*/
    itmask2 = (uint8_t)((uint8_t)1 << itmask1);

    /* Check the status of the specified UART2 pending bit*/
    if (UART2_IT == UART2_IT_PE)
    {
        /* Get the UART2_ITPENDINGBIT enable bit status*/
        enablestatus = (uint8_t)((uint8_t)UART2->CR1 & itmask2);
        /* Check the status of the specified UART2 interrupt*/

        if (((UART2->SR & itpos) != (uint8_t)0x00) && enablestatus)
        {
            /* Interrupt occurred*/
            pendingbitstatus = SET;
        }
        else
        {
            /* Interrupt not occurred*/
            pendingbitstatus = RESET;
        }
    }
    else if (UART2_IT == UART2_IT_LBDF)
    {
        /* Get the UART2_IT enable bit status*/
        enablestatus = (uint8_t)((uint8_t)UART2->CR4 & itmask2);
        /* Check the status of the specified UART2 interrupt*/
        if (((UART2->CR4 & itpos) != (uint8_t)0x00) && enablestatus)
        {
            /* Interrupt occurred*/
            pendingbitstatus = SET;
        }
        else
        {
            /* Interrupt not occurred*/
            pendingbitstatus = RESET;
        }
    }
    else if (UART2_IT == UART2_IT_LHDF)
    {
        /* Get the UART2_IT enable bit status*/
        enablestatus = (uint8_t)((uint8_t)UART2->CR6 & itmask2);
        /* Check the status of the specified UART2 interrupt*/
        if (((UART2->CR6 & itpos) != (uint8_t)0x00) && enablestatus)
        {
            /* Interrupt occurred*/
            pendingbitstatus = SET;
        }
        else
        {
            /* Interrupt not occurred*/
            pendingbitstatus = RESET;
        }
    }
    else
    {
        /* Get the UART2_IT enable bit status*/
        enablestatus = (uint8_t)((uint8_t)UART2->CR2 & itmask2);
        /* Check the status of the specified UART2 interrupt*/
        if (((UART2->SR & itpos) != (uint8_t)0x00) && enablestatus)
        {
            /* Interrupt occurred*/
            pendingbitstatus = SET;
        }
        else
        {
            /* Interrupt not occurred*/
            pendingbitstatus = RESET;
        }
    }
    /* Return the UART2_IT status*/
    return  pendingbitstatus;
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void UART2_ClearITPendingBit(UART2_IT_TypeDef UART2_IT)
{
    //assert_param(IS_UART2_CLEAR_IT_OK(UART2_IT));

    /*  Clear the Receive Register Not Empty pending bit */
    if (UART2_IT == UART2_IT_RXNE)
    {
        UART2->SR = (uint8_t)~(UART2_SR_RXNE);
    }
    /*  Clear the LIN Break Detection pending bit */
    else if (UART2_IT == UART2_IT_LBDF)
    {
        UART2->CR4 &= (uint8_t)~(UART2_CR4_LBDF);
    }
    /*  Clear the LIN Header Detection pending bit */
    else
    {
        UART2->CR6 &= (uint8_t)(~UART2_CR6_LHDF);
    }
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
