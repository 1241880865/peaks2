


#include "PWMCtrl.h"



typedef struct
{
    void (*pTimSet)(TIM_TypeDef*, uint16_t);//ѡ����
	TIM_TypeDef* tim_x;						//ѡ�������
}
PWM_STRUCT;

/*------------------------------------------------------------------------------
********************************** �������� ************************************
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

//ʵ�ʸ�ֵռ�ձ�
static u16 sPwmDuty;
/*******************************************************************************
** Function name:  PwmOutput
** Descriptions:   Pwm�������
** input:          ��
** output:         ��
** Return:         ��
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
		else //�رմ�·PWM���
		{
			(pTimTab+i)->pTimSet((pTimTab+i)->tim_x, 0);
		}
	}
}

/*******************************************************************************
** Function name:  PwmAllOff
** Descriptions:   �ر�����Pwm���
** input:          ��
** output:         ��
** Return:         ��
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
** Descriptions:   ����ͨѶ��ռ�ձ�
** input:          ��
** output:         ռ�ձ�
** Return:         ռ�ձȼ���ֵ
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
