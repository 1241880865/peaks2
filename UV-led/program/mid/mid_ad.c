

#include "main.h"
#include "bsp_init.h"
#include "mid_ad.h"


/*------------------------------------------------------------------------------
********************************** 变量定义 ************************************
------------------------------------------------------------------------------*/
//环境温度 12位采样,3.3V系统
const s16 Ntc_Temp[11] =
    {-20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80}; //-20℃~80℃
const s16 Ntc_Value[11] =
    {2455, 1922, 1413, 1007, 688, 474, 322, 224, 157, 110, 80};

/*------------------------------------------------------------------------------
********************************* 结构体 声明 **********************************
------------------------------------------------------------------------------*/
typedef struct
{
	u32 sample;
	u32 overSum;
}
AD_SMPL;
AD_SMPL m_ovAd[2];

u16 gAdBuff[16][AD_NUM];   //AD采样值
u8  gAdOk;

//函数
static void AdFilt(void);
static u16 CalcTEnv(s32 smpl);
/*******************************************************************************
** Function name:  AdCalc
** Descriptions:   AD计算
** input:          无
** output:         无
** Return:         无
*******************************************************************************/
void AdCalc(void)
{
	if(gAdOk != 0)
	{
		gAdOk = 0;
		
		//采样滤波
		AdFilt();
		
		//计算ntc环境温度
		m_para.ntc1 = CalcTEnv(m_ovAd[0].sample);
		m_para.ntc2 = CalcTEnv(m_ovAd[1].sample);
		
		DMA_ReStart();
	}
}

/*******************************************************************************
** Function name:  CalcTEnv
** Descriptions:   环境温度 采样字 --> 温度值
** input:          环境温度
** output:         无
** Return:         环境温度值
*******************************************************************************/
u16 CalcTEnv(s32 smpl)
{
	u8  i;
	s32 a, b;
	u16 meas_value;
	
	/* 采样值 --> 温度值 */
	if(smpl <= 80) smpl = 80; //80对应的采样值
	for(i = 0; i < 10; i++)
	{
		if(smpl >= Ntc_Value[i + 1])
		{
			a = (s32)smpl - Ntc_Value[i];
			b = a * (Ntc_Temp[i+1] - Ntc_Temp[i]);
			a = b / (Ntc_Value[i+1] - Ntc_Value[i]);
			meas_value = a + Ntc_Temp[i];
			break;
		}
	}
	
    return(meas_value);
}

/*******************************************************************************
* Function Name  : AdFilt
* Description    : 采样滤波
* Input          : ad计算中间参数
* Output         : None
* Return         : None
*******************************************************************************/
void AdFilt(void) 
{
	u8  i, j;
	
	for(j=0; j<AD_NUM; j++) //共2路AD
	{
		for(i=0; i<=15; i++)
		{
			m_ovAd[j].overSum += gAdBuff[i][j];
		}
		
		m_ovAd[j].sample = m_ovAd[j].overSum >> 4;
		m_ovAd[j].overSum = 0;
	}
}






