
/* Includes ------------------------------------------------------------------*/
#include "stm8s_adc1.h"

/*----------------------------------------------------------------------------*/
void ADC1_DeInit(void)
{
    ADC1->CSR  = ADC1_CSR_RESET_VALUE;
    ADC1->CR1  = ADC1_CR1_RESET_VALUE;
    ADC1->CR2  = ADC1_CR2_RESET_VALUE;
    ADC1->CR3  = ADC1_CR3_RESET_VALUE;
    ADC1->TDRH = ADC1_TDRH_RESET_VALUE;
    ADC1->TDRL = ADC1_TDRL_RESET_VALUE;
    ADC1->HTRH = ADC1_HTRH_RESET_VALUE;
    ADC1->HTRL = ADC1_HTRL_RESET_VALUE;
    ADC1->LTRH = ADC1_LTRH_RESET_VALUE;
    ADC1->LTRL = ADC1_LTRL_RESET_VALUE;
    ADC1->AWCRH = ADC1_AWCRH_RESET_VALUE;
    ADC1->AWCRL = ADC1_AWCRL_RESET_VALUE;
}

/*----------------------------------------------------------------------------*/

void ADC1_Cmd(FunctionalState NewState)
{

    /* Check the parameters */
    assert_param(IS_FUNCTIONALSTATE_OK(NewState));

    if (NewState != DISABLE)
    {
        ADC1->CR1 |= ADC1_CR1_ADON;
    }
    else /* NewState == DISABLE */
    {
        ADC1->CR1 &= (uint8_t)(~ADC1_CR1_ADON);
    }

}

/*----------------------------------------------------------------------------*/

void ADC1_ITConfig(ADC1_IT_TypeDef ADC1_IT, FunctionalState NewState)
{

    /* Check the parameters */
    assert_param(IS_ADC1_IT_OK(ADC1_IT));
    assert_param(IS_FUNCTIONALSTATE_OK(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the ADC1 interrupts */
        ADC1->CSR |= (uint8_t)ADC1_IT;
    }
    else  /* NewState == DISABLE */
    {
        /* Disable the ADC1 interrupts */
        ADC1->CSR &= (uint8_t)((uint16_t)~(uint16_t)ADC1_IT);
    }

}

/*----------------------------------------------------------------------------*/

void ADC1_PrescalerConfig(ADC1_PresSel_TypeDef ADC1_Prescaler)
{

    /* Check the parameter */
    assert_param(IS_ADC1_PRESSEL_OK(ADC1_Prescaler));

    /* Clear the SPSEL bits */
    ADC1->CR1 &= (uint8_t)(~ADC1_CR1_SPSEL);
    /* Select the prescaler division factor according to ADC1_PrescalerSelection values */
    ADC1->CR1 |= (uint8_t)(ADC1_Prescaler);

}

/*----------------------------------------------------------------------------*/

void ADC1_ConversionConfig(ADC1_ConvMode_TypeDef ADC1_ConversionMode, ADC1_Channel_TypeDef ADC1_Channel, ADC1_Align_TypeDef ADC1_Align)
{

    /* Check the parameters */
    assert_param(IS_ADC1_CONVERSIONMODE_OK(ADC1_ConversionMode));
    assert_param(IS_ADC1_CHANNEL_OK(ADC1_Channel));
    assert_param(IS_ADC1_ALIGN_OK(ADC1_Align));

    /* Clear the align bit */
    ADC1->CR2 &= (uint8_t)(~ADC1_CR2_ALIGN);
    /* Configure the data alignment */
    ADC1->CR2 |= (uint8_t)(ADC1_Align);

    if (ADC1_ConversionMode == ADC1_CONVERSIONMODE_CONTINUOUS)
    {
        /* Set the continuous coversion mode */
        ADC1->CR1 |= ADC1_CR1_CONT;
    }
    else /* ADC1_ConversionMode == ADC1_CONVERSIONMODE_SINGLE */
    {
        /* Set the single conversion mode */
        ADC1->CR1 &= (uint8_t)(~ADC1_CR1_CONT);
    }

    /* Clear the ADC1 channels */
    ADC1->CSR &= (uint8_t)(~ADC1_CSR_CH);
    /* Select the ADC1 channel */
    ADC1->CSR |= (uint8_t)(ADC1_Channel);

}

/*----------------------------------------------------------------------------*/

void ADC1_StartConversion(void)
{
    ADC1->CR1 |= ADC1_CR1_ADON;
}

/*----------------------------------------------------------------------------*/

uint16_t ADC1_GetConversionValue(void)
{

    uint16_t temph = 0;
    uint8_t templ = 0;

    if ((ADC1->CR2 & ADC1_CR2_ALIGN) != 0) /* Right alignment */
    {
        /* Read LSB first */
        templ = ADC1->DRL;
        /* Then read MSB */
        temph = ADC1->DRH;

        temph = (uint16_t)(templ | (uint16_t)(temph << (uint8_t)8));
    }
    else /* Left alignment */
    {
        /* Read MSB firts*/
        temph = ADC1->DRH;
        /* Then read LSB */
        templ = ADC1->DRL;

        temph = (uint16_t)((uint16_t)((uint16_t)templ << 6) | (uint16_t)((uint16_t)temph << 8));
    }

    return ((uint16_t)temph);

}

/*----------------------------------------------------------------------------*/

void ADC1_ClearITPendingBit(ADC1_IT_TypeDef ITPendingBit)
{
    uint8_t temp = 0;

    /* Check the parameters */
    assert_param(IS_ADC1_ITPENDINGBIT_OK(ITPendingBit));

    if (((uint16_t)ITPendingBit & 0xF0) == 0x10)
    {
        /* Clear analog watchdog channel status */
        temp = (uint8_t)((uint16_t)ITPendingBit & 0x0F);
        if (temp < 8)
        {
            ADC1->AWSRL &= (uint8_t)~(uint8_t)((uint8_t)1 << temp);
        }
        else
        {
            ADC1->AWSRH &= (uint8_t)~(uint8_t)((uint8_t)1 << (temp - 8));
        }
    }
    else  /* Clear EOC | AWD flag status */
    {
        ADC1->CSR &= (uint8_t)((uint16_t)~(uint16_t)ITPendingBit);
    }
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
