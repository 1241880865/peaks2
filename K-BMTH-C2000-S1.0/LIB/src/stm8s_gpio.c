
/* Includes ------------------------------------------------------------------*/
#include "stm8s_gpio.h"

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void GPIO_DeInit(GPIO_TypeDef* GPIOx)
{
    GPIOx->ODR = GPIO_ODR_RESET_VALUE; /* Reset Output Data Register */
    GPIOx->DDR = GPIO_DDR_RESET_VALUE; /* Reset Data Direction Register */
    GPIOx->CR1 = GPIO_CR1_RESET_VALUE; /* Reset Control Register 1 */
    GPIOx->CR2 = GPIO_CR2_RESET_VALUE; /* Reset Control Register 2 */
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin, GPIO_Mode_TypeDef GPIO_Mode)
{
    /*----------------------*/
    /* Check the parameters */
    /*----------------------*/

    assert_param(IS_GPIO_MODE_OK(GPIO_Mode));
    assert_param(IS_GPIO_PIN_OK(GPIO_Pin));
    
  /* Reset corresponding bit to GPIO_Pin in CR2 register */
  GPIOx->CR2 &= (uint8_t)(~(GPIO_Pin));

    /*-----------------------------*/
    /* Input/Output mode selection */
    /*-----------------------------*/

    if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x80) != (uint8_t)0x00) /* Output mode */
    {
        if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x10) != (uint8_t)0x00) /* High level */
        {
            GPIOx->ODR |= (uint8_t)GPIO_Pin;
        } 
        else /* Low level */
        {
            GPIOx->ODR &= (uint8_t)(~(GPIO_Pin));
        }
        /* Set Output mode */
        GPIOx->DDR |= (uint8_t)GPIO_Pin;
    } 
    else /* Input mode */
    {
        /* Set Input mode */
        GPIOx->DDR &= (uint8_t)(~(GPIO_Pin));
    }

    /*------------------------------------------------------------------------*/
    /* Pull-Up/Float (Input) or Push-Pull/Open-Drain (Output) modes selection */
    /*------------------------------------------------------------------------*/

    if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x40) != (uint8_t)0x00) /* Pull-Up or Push-Pull */
    {
        GPIOx->CR1 |= (uint8_t)GPIO_Pin;
    } 
    else /* Float or Open-Drain */
    {
        GPIOx->CR1 &= (uint8_t)(~(GPIO_Pin));
    }

    /*-----------------------------------------------------*/
    /* Interrupt (Input) or Slope (Output) modes selection */
    /*-----------------------------------------------------*/

    if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x20) != (uint8_t)0x00) /* Interrupt or Slow slope */
    {
        GPIOx->CR2 |= (uint8_t)GPIO_Pin;
    } 
    else /* No external interrupt or No slope control */
    {
        GPIOx->CR2 &= (uint8_t)(~(GPIO_Pin));
    }
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void GPIO_Write(GPIO_TypeDef* GPIOx, uint8_t PortVal)
{
    GPIOx->ODR = PortVal;
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void GPIO_WriteHigh(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins)
{
    GPIOx->ODR |= (uint8_t)PortPins;
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void GPIO_WriteLow(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins)
{
    GPIOx->ODR &= (uint8_t)(~PortPins);
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void GPIO_WriteReverse(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins)
{
    GPIOx->ODR ^= (uint8_t)PortPins;
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
uint8_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx)
{
    return ((uint8_t)GPIOx->ODR);
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
uint8_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx)
{
    return ((uint8_t)GPIOx->IDR);
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
BitStatus GPIO_ReadInputPin(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin)
{
    return ((BitStatus)(GPIOx->IDR & (uint8_t)GPIO_Pin));
}

/*******************************************************************************
*	Function name:	      ----
*	Descriptions:	      ----
*******************************************************************************/
void GPIO_ExternalPullUpConfig(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_GPIO_PIN_OK(GPIO_Pin));
    assert_param(IS_FUNCTIONALSTATE_OK(NewState));

    if (NewState != DISABLE) /* External Pull-Up Set*/
    {
        GPIOx->CR1 |= (uint8_t)GPIO_Pin;
    } else /* External Pull-Up Reset*/
    {
        GPIOx->CR1 &= (uint8_t)(~(GPIO_Pin));
    }
}

  
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
