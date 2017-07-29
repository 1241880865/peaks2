

#include "main.h"
#include "mid_out.h"




//ʱ�����
u16 TimMs_ledFlash;
/*******************************************************************************
** Function name:  Output
** Descriptions:   ���
** input:          ��
** output:         ��
** Return:         ��
*******************************************************************************/
void Output(void)
{
	//�����̵������
	if((m_para.status1 & 0x4fff) != 0) //12·��·���� ���±���
	{
		pm_st->almOut = 1;
		ALM_ON();
	}
	else
	{
		ALM_OFF();
		pm_st->almOut = 0;
	}
	
	//led���
	if((m_para.status1 & 0x0fff) != 0) //12·��·���� 
	{
		if(TimMs_ledFlash < 500)
		{
			LED1ON();
		}
		else if(TimMs_ledFlash < 1000)
		{
			LED1OFF();
		}
		else
		{
			TimMs_ledFlash = 0;
		}
	}
	else if((m_para.status1 & 0x4000) != 0) //���±��� 
	{
		if(TimMs_ledFlash < 250)
		{
			LED1ON();
		}
		else if(TimMs_ledFlash < 500)
		{
			LED1OFF();
		}
		else
		{
			TimMs_ledFlash = 0;
		}
	}
	else
	{
		LED1OFF();
	}
}






