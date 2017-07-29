

#ifndef BSP_INIT_H
#define BSP_INIT_H


//º¯Êý
extern void RCC_Configuration(void);
extern void GPIO_Configuration(void);
extern void Tim_Configuration(void);
extern void USART_Configuration(void);
extern void NVIC_Configuration(void);
extern void Systick_Configuration(void);
extern void DMA_Configuration(u16* base_add, u16 size);
extern void ADC_Configuration(void);
extern void IWDG_Configuration(void);
extern void DMA_ReStart(void);

#endif



