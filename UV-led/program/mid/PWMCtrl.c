


#include "PWMCtrl.h"



typedef struct
{
    void (*pTimSet)(TIM_TypeDef*, uint16_t);//选择函数
	TIM_TypeDef* tim_x;						//选择函数入口
}
PWM_STRUCT;

/*------------------------------------------------------------------------------
********************************** 变量定义 ************************************
------------------------------------------------------------------------------*/
PWM_STRUCT PwmTab[12] = 
{
	{TIM_SetCompare4, TIM3},//PWM1
	{TIM_SetCompare3, TIM3},
	{TIM_SetCompare2, TIM3},
	{TIM_SetCompare1, TIM3},
	{TIM_SetCompare4, TIM2},//PWM5
	{TIM_SetCompare3, TIM2},
	{TIM_SetCompare2, TIM2},
	{TIM_SetCompare1, TIM2},
	{TIM_SetCompare4, TIM4},
	{TIM_SetCompare3, TIM4},//PWM10
	{TIM_SetCompare2, TIM4},
	{TIM_SetCompare1, TIM4}
};
const PWM_STRUCT* pTimTab = PwmTab;

//实际赋值占空比
static u16 sPwmDuty;
/*******************************************************************************
** Function name:  PwmOutput
** Descriptions:   Pwm输出控制
** input:          无
** output:         无
** Return:         无
*******************************************************************************/
void PwmOutOn(void)
{
	u8 i;
	
	for(i = 0; i <= 11; i++)
	{
		if(pm_od->onOff[i] != 0)
		{
			(pTimTab+i)->pTimSet((pTimTab+i)->tim_x,(u16)sPwmDuty);
		}
		else //关闭此路PWM输出
		{
			(pTimTab+i)->pTimSet((pTimTab+i)->tim_x, 0);
		}
	}
}

/*******************************************************************************
** Function name:  PwmAllOff
** Descriptions:   关闭所有Pwm输出
** input:          无
** output:         无
** Return:         无
*******************************************************************************/
void PwmAllOff(void)
{
	u8 i;
	
	for(i = 0; i <= 11; i++)
	{
		(pTimTab+i)->pTimSet((pTimTab+i)->tim_x, 0);
	}
}

/*******************************************************************************
** Function name:  CalcDutyWeb
** Descriptions:   计算通讯版占空比
** input:          无
** output:         占空比
** Return:         占空比监视值
*******************************************************************************/
u16 CalcDutyWeb(void)
{
	sPwmDuty = m_para.duty * m_para.factor;
	sPwmDuty = sPwmDuty / 100;
	
	return(sPwmDuty);
}

/*******************************************************************************
-------------------------------------- END -------------------------------------
*******************************************************************************/
