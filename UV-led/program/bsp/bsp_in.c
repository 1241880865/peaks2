

#include "main.h"
#include "bsp_in.h"

#define TRIG_EFFECT_VOL  SET  //RESET

//切换引脚读入
typedef const struct
{
    u8 adA;		//A2
    u8 adB;		//A1
    u8 adC;		//A0
}
OPEN_CIRCUIT;

static OPEN_CIRCUIT gTab[8] =
{
	{1, 0, 1},
	{1, 1, 1},
	{1, 1, 0},
	{1, 0, 0},
	{0, 1, 1},
	
	{0, 0, 0},
	{0, 0, 1},
	{0, 1, 0},
};

#define SWITCH_IN     GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11)
#define OPEN_IN_1     GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)
#define OPEN_IN_2     GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)
#define OPEN_IN_3     GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)
#define OPEN_IN_4     GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)
#define OPEN_IN_TAB   GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)

static u8 sProSwitch;
static u8 sProOpen1, sProOpen2, sProOpen3, sProOpen4;
static u8 sStep, sCH;
static u8 sTabSt;

//时间变量
u16 TimMs_switch;
u16 TimMs_open1;
u16 TimMs_open2;
u16 TimMs_open3;
u16 TimMs_open4;
u16 TimMs_openTab;
u16 TimMs_openDly;
u16 TimMs_openKeep;

//函数
void getOpen4(void);
void getOpenTab(void);
/*******************************************************************************
** Function name:  Input
** Descriptions:   获取输入信号
** input:          无
** output:         无
** Return:         无
*******************************************************************************/
void Input(void)
{
    //外部开关
    if(SWITCH_IN != sProSwitch)
    {
        if(TimMs_switch >= 100)
        {
            sProSwitch = SWITCH_IN;
            
            if(SWITCH_IN == RESET)
            {
                sProSwitch = 0;
            }
            else
            {
                sProSwitch = 1;
            }
        }
    }
    else
    {
        TimMs_switch = 0;
    }
	pm_st->switchOn = sProSwitch;
}

/*******************************************************************************
** Function name:  GetOpenCircuit
** Descriptions:   获取开路报警状态
** input:          无
** output:         无
** Return:         无
*******************************************************************************/
void GetOpenCircuit(void)
{
	getOpen4();
	getOpenTab();
}

/*******************************************************************************
** Function name:  getOpenTab
** Descriptions:   获取电子开关控制8路，开路报警状态
** input:          无
** output:         无
** Return:         无
*******************************************************************************/
void getOpenTab(void)
{
	if(sStep == 0) //切换开始延时
	{
		A2() = gTab[sCH].adA;
		A1() = gTab[sCH].adB;
		A0() = gTab[sCH].adC;
		
		sStep = 1;
		TimMs_openDly = 0;
	}
	else if(sStep == 1) //2ms后读入状态
	{
		if(TimMs_openDly >= 2)
		{
			sStep = 2;
			TimMs_openKeep = 0;
			TimMs_openTab = 0;
			
			sTabSt = OPEN_IN_TAB;
		}
	}
	else //10ms维持后切换下一路读入
	{
		if(TimMs_openKeep < 10)
		{
			if(sTabSt == OPEN_IN_TAB)
			{
				if(TimMs_openTab >= 5) //点平持续5ms后，读取电平值，否则不变化
				{
					if(OPEN_IN_TAB == TRIG_EFFECT_VOL)
					{
						pm_st->ledOpenI[sCH+4] = 0;
					}
					else
					{
						if(pm_od->onOff[sCH+4] != 0)
						{
							pm_st->ledOpenI[sCH+4] = 1;
						}
						else
						{
							pm_st->ledOpenI[sCH+4] = 0;
						}
					}
				}
			}
			else
			{
				sTabSt = OPEN_IN_TAB;
				TimMs_openTab = 0;
			}
		}
		else
		{
			sStep = 0;
			sCH++;
			if(sCH >= 8)
			{
				sCH = 0;
			}
		}
	}
}

/*******************************************************************************
** Function name:  getOpen4
** Descriptions:   获取前4路开路报警状态
** input:          无
** output:         无
** Return:         无
*******************************************************************************/
void getOpen4(void)
{
    //断路故障检测1
    if(OPEN_IN_1 != sProOpen1)
    {
        if(TimMs_open1 >= 100)
        {
            sProOpen1 = OPEN_IN_1;
            
            if(OPEN_IN_1 == TRIG_EFFECT_VOL)
            {
                sProOpen1 = 0;
            }
            else
            {
                sProOpen1 = 1;
            }
        }
    }
    else
    {
        TimMs_open1 = 0;
    }
	
	//断路故障检测2
    if(OPEN_IN_2 != sProOpen2)
    {
        if(TimMs_open2 >= 100)
        {
            sProOpen2 = OPEN_IN_2;
            
            if(OPEN_IN_2 == TRIG_EFFECT_VOL)
            {
                sProOpen2 = 0;
            }
            else
            {
                sProOpen2 = 1;
            }
        }
    }
    else
    {
        TimMs_open2 = 0;
    }
	
	//断路故障检测3
    if(OPEN_IN_3 != sProOpen3)
    {
        if(TimMs_open3 >= 100)
        {
            sProOpen3 = OPEN_IN_3;
            
            if(OPEN_IN_3 == TRIG_EFFECT_VOL)
            {
                sProOpen3 = 0;
            }
            else
            {
                sProOpen3 = 1;
            }
        }
    }
    else
    {
        TimMs_open3 = 0;
    }
	
	//断路故障检测4
    if(OPEN_IN_4 != sProOpen4)
    {
        if(TimMs_open4 >= 100)
        {
            sProOpen4 = OPEN_IN_4;
            
            if(OPEN_IN_4 == TRIG_EFFECT_VOL)
            {
                sProOpen4 = 0;
            }
            else
            {
                sProOpen4 = 1;
            }
        }
    }
    else
    {
        TimMs_open4 = 0;
    }
	
	//断路赋值
	pm_st->ledOpenI[0] = 0;
	pm_st->ledOpenI[1] = 0;
	pm_st->ledOpenI[2] = 0;
	pm_st->ledOpenI[3] = 0;
	if(pm_od->onOff[0] != 0)
	{
		pm_st->ledOpenI[0] = sProOpen1;
	}
	if(pm_od->onOff[1] != 0)
	{
		pm_st->ledOpenI[1] = sProOpen2;
	}
	if(pm_od->onOff[2] != 0)
	{
		pm_st->ledOpenI[2] = sProOpen3;
	}
	if(pm_od->onOff[3] != 0)
	{
		pm_st->ledOpenI[3] = sProOpen4;
	}
}



















