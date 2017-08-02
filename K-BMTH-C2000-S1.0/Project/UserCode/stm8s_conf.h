/***********************************Copyright(c)********************************  
**   Program Name:  stm8s_conf.H
**   Descriptions:  The head file of stm8s
**       MCU Type： STM8S105K4T6C
** 			                
**       Designer:  Song Haihao
**   Company Name:  Peaks Measure&Control Tech Ltd
**           Date:  2015-01-04
**         Vision:  ----
--------------------------------------------------------------------------------
～～～～～～～～～～～～～～～～～～ 更改记录 ～～～～～～～～～～～～～～～～～
--------------------------------------------------------------------------------
**  创建日期:  **  2015-01-04
**    版本号:  **  ----
**  功能描述:  **  仅保留stm8s105外设头文件，并注释掉未用到外设头文件，删减调试
                   assert_param()函数
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8S_CONF_H
#define __STM8S_CONF_H

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

#if defined(STM8S105)

#include "stm8s_adc1.h"
#include "stm8s_clk.h"
#include "stm8s_itc.h"
#include "stm8s_flash.h"
#include "stm8s_gpio.h"
#include "stm8s_iwdg.h"
#include "stm8s_tim1.h"
#include "stm8s_tim2.h"
#include "stm8s_tim4.h"
#include "stm8s_uart2.h"

#endif
/*********************** STM8S105 */

/* Debug -- assert_failed */
//#define USE_FULL_ASSERT    (1)

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))

void assert_failed(uint8_t* file, uint32_t line);
#else
#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif
/*********************** __STM8S_CONF_H */

/********************************* END OF FILE ********************************/
