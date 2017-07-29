

#include "main.h"
#include "mid_out.h"




//时间变量
u16 TimMs_ledFlash;
/*******************************************************************************
** Function name:  Output
** Descriptions:   输出
** input:          无
** output:         无
** Return:         无
*******************************************************************************/
void Output(void)
{
	//报警继电器输出
	if((m_para.status1 & 0x4fff) != 0) //12路断路报警 超温报警
	{
		pm_st->almOut = 1;
		ALM_ON();
	}
	else
	{
		ALM_OFF();
		pm_st->almOut = 0;
	}
	
	//led输出
	if((m_para.status1 & 0x0fff) != 0) //12路断路报警 
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
	else if((m_para.status1 & 0x4000) != 0) //超温报警 
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






