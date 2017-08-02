


/*------------------------------------------------------------------------------
***********************************头文件包含***********************************
------------------------------------------------------------------------------*/
#include "stm8s.h"
#include "heatCtrl.h"
#include "eepromCtrl.h" //仅自整定后，参数保存
/*------------------------------------------------------------------------------
********************************** 全局 变量 ***********************************
------------------------------------------------------------------------------*/
//标志位
TFlag_STRUCT Tflag;//温度标志位结构体定义

//温度参数表格
ParaTone_STRUCT ParaTempPID;  //参数表格一定义
ParaTtwo_STRUCT ParaTempCtrl; //参数表格二定义

//温度值
s16 TempSetVal;     //温度设定值
s16 TempMeasure;    //温度测量值
s16 TempDisplay;    //温度显示值

//PID温控真实运行参数
TempPID_SRTUCT Tpid;

//自整定参数
ATSTEP_STRUCT At_Step;//自整定 步骤
u16  TimS_AtMax;  //自整定最长允许时间
s16  AtMax;       //自整定得到的最大温度测量值
s16  AtMin;       //自整定得到的最小温度测量值
u16  TimS_Bg;     //波谷时间
u16  TimS_Bf;     //波峰时间
//时间变量
u16 TimS_HeatPeriod;  //加热周期计时
u8  TimS_EnAT;        //自整定时间计时
u16 TimMs_HeatCounter;//加热周期占空比计时
/*------------------------------------------------------------------------------
********************************* 内部函数声明 *********************************
------------------------------------------------------------------------------*/
void PIDControl(void);//PID计算函数
/*******************************************************************************
	Function name:	      TempMesToDisp
	Descriptions:	      Control the temperature heat
	Parameters:	      void
	Returned value:       void
*******************************************************************************/
void TempADToDisp(void)
{
    s16 temp_filter;   //热电偶滤波值
    
    if (Pt100.FlagOK != 0)
    {
        Pt100.FlagOK  = 0;
        
        temp_filter = DataFilter(Pt100.SmplVal);//采样滤波
        TempMeasure = CalculateTemp(temp_filter, ParaTempPID.pb, ParaTempPID.pl);//采样值换算
        
        //显示防抖动
        if (TempMeasure <= (TempSetVal - 1))
        {
            TempDisplay = TempMeasure + 1;
        }
        else if (TempMeasure >= (TempSetVal + 1))
        {
            TempDisplay = TempMeasure - 1;
        }
        else TempDisplay = TempSetVal;
    }
}

//void TempMesToDisp(void)
//{
//    //显示防抖动
//    if (TempMeasure <= (TempSetVal - 3))
//    {
//	TempDisplay = TempMeasure + 3;
//    }
//    else if (TempMeasure >= (TempSetVal +3))
//    {
//	TempDisplay = TempMeasure - 3;
//    }
//    else TempDisplay = TempSetVal;
//}

/*******************************************************************************
	Function name:	      TempControl
	Descriptions:	      Control the temperature heat
	Parameters:	      void
	Returned value:       void
*******************************************************************************/
void HeatControl(void)
{
    s32 temp_a;
    s32 temp_b;
    
    if (Tflag.EnAT != 0) //自整定加热控制
    {
        if (At_Step.ATJR == 1)
        {
            HEAT_ON;
            Tflag.HeatDis = 1;
        }
        else
        {
            HEAT_OFF;
            Tflag.HeatDis = 0;
        }
        
        //无PID，参数初值
        PIDReset();
        
        return;
    }
    
    //温控关闭 or 温度溢出
    if ((Tflag.EnTempRun == 0) || (Tflag.TempOver != 0))
    {
        ALM_OFF;
        HEAT_OFF;
        Tflag.HeatDis = 0; //no "HEAT"
        
        //无PID，参数初值
        PIDReset();
    }
    else //正常加热控制
    {
        //加热周期
        if (TimS_HeatPeriod >= ParaTempPID.Temp_t)
        {
            TimS_HeatPeriod = 0;  //清空pwm计数
            TimMs_HeatCounter = 0;
            
            //输出功率控制
            PIDControl();
            temp_a = Tpid.Result * ParaTempPID.Temp_t;//PID_OutResult取值范围 0~1000
            temp_b = (u32)temp_a * ParaTempCtrl.np;
            Tpid.Result = (u16)(temp_b / 100);
        }
        else
        {
            if (Tflag.TempAlm != 0) //超温停止加热
            {
                HEAT_OFF;
                Tflag.HeatDis = 0;
            }
            else
            {
                if (TimMs_HeatCounter < Tpid.Result)
                {
                    HEAT_ON;
                    Tflag.HeatDis = 1;
                }
                else
                {
                    HEAT_OFF;
                    Tflag.HeatDis = 0;
                }
            }
        }
    }
}

/*******************************************************************************
*	Function name:	      PIDReset
*	Descriptions:	      PID parameters Reset
*	Parameters:	      void
*	Returned value:       void
*******************************************************************************/
void PIDReset(void)
{
    Tpid.ISum = 0;      //积分累积清零
    Tpid.DResult = TempMeasure;  //微分先行输出值为其输入（温度采样值）
    Tpid.LastMeasure = TempMeasure; //温度记录值更新
}

/*******************************************************************************
*	Function name:	      PIDControl
*	Descriptions:	      Control the PID out
*	Parameters:	      TempSetVal  TempMeasure  Tpid.LastMeasure
*                             微分：Tpid.DVal  Tpid.DResult  Tpid.TVal
*                                   TempMeasure  Tpid.LastMeasure
*                             积分：Tpid.ISum
*                                   Tpid.PkVal  Tpid.IVal
*	Returned value:       Tpid.Result
*******************************************************************************/
void PIDControl(void)
{
    s16 temp_a,temp_b;
    s32 temp_c,temp_d;
    
    Tpid.TVal = ParaTempPID.Temp_t; //微分周期时间
    Tpid.PkVal = ParaTempPID.Temp_p;//比例
    Tpid.IVal = ParaTempPID.Temp_i; //积分值
    Tpid.DVal = ParaTempPID.Temp_d; //得到微分的真实运行控制值
    
    if ((TempSetVal - Tpid.DResult) >= Tpid.PkVal) //下比例带外
    {
	Tpid.Result = 1000; //全功率输出
        
        //无PID，参数初值
        PIDReset();
    }
    else
    {
	if ((Tpid.DResult - TempSetVal) >= Tpid.PkVal) //上比例带外
	{
	    Tpid.Result = 0; //关闭输出
            
            //无PID，参数初值
            PIDReset();
	}
	else
	{
            /*************************** 微分先行(无禁止) *****************************/
            temp_c = (s32)(TempMeasure - Tpid.LastMeasure) * Tpid.DVal * 6;
            temp_c = temp_c + (s32)Tpid.DVal * Tpid.DResult;
            temp_c = temp_c + (s32)TempMeasure * Tpid.TVal * 6;
            temp_d = temp_c / (Tpid.DVal + Tpid.TVal * 6);
            
            //微分上下限比设定值差50.0℃
            if (temp_d >= TEMP_MAX + 500)
            {
                Tpid.DResult = TEMP_MAX + 500;
            }
            else if (temp_d <= TEMP_MIN - 500)
            {
                Tpid.DResult = TEMP_MIN - 500;
            }
            else
            {
                Tpid.DResult = (s16)temp_d;
            }
            
            //四舍五入处理
            temp_a = temp_c % (Tpid.DVal + Tpid.TVal * 6);
            temp_b = (Tpid.DVal + Tpid.TVal * 6) / 2;
            if (temp_a >= temp_b)
            {
                if (Tpid.DResult >= 0)
                {
                    Tpid.DResult = Tpid.DResult + 1;
                }
                else
                {
                    Tpid.DResult = Tpid.DResult - 1;
                }
            }
            Tpid.LastMeasure = TempMeasure;
            
            /******************* end of 微分 ，输出 Tpid.DResult **********************/
            
            /****************************** 变速积分 **********************************/
            //变速积分处理
	    if(TempMeasure < TempSetVal)
	    {
	        temp_a = Tpid.PkVal / 3;  //1/3比例带
	        if(TempSetVal - TempMeasure > temp_a)
	        {
	            temp_b = Tpid.PkVal;
	            temp_c = TempSetVal - Tpid.DResult;
	            temp_b = temp_c * (temp_b - (TempSetVal - TempMeasure)) / (temp_b - temp_a);
	        }
	        else temp_b = TempSetVal - Tpid.DResult;  //全速 输出
	    }
	    else temp_b = TempSetVal - Tpid.DResult;//将要下降，还未下降，加热输出
                                                     //温差小，全速，无变速
            
	    temp_c = (s32)temp_b * 1000;
	    temp_c = (temp_c * Tpid.TVal) / Tpid.PkVal;
	    Tpid.ISum = Tpid.ISum + temp_c;
	    if (Tpid.ISum < 0) Tpid.ISum = 0;/* 积分小于零，需制冷 加热无效 */
	    temp_a = Tpid.ISum / Tpid.IVal;
            
	    if(temp_a >= 1000)
	    {
	        temp_a = 1000;
                Tpid.ISum = (s32)Tpid.IVal * 1000;
	    }
            
            //比例环节
	    temp_c = (s32)(TempSetVal - Tpid.DResult) * 1000;
	    temp_b = temp_c / Tpid.PkVal;
            
	    temp_b = temp_a + temp_b;
	    if (temp_b < 0) Tpid.Result = 0;
	    else if (temp_b > 1000) Tpid.Result = 1000;
	    else Tpid.Result = temp_b;
	}
    }
    /***************** end of 变速积分 ，输出 Tpid.Result *******************/
}

/*******************************************************************************
                         -------- 自整定 --------
*******************************************************************************/

/*******************************************************************************
Function name: AT_ini
Descriptions: PID自整定初始化
*******************************************************************************/
void AT_ini(void)
{
    At_Step.ATN1 = 0;
    At_Step.ATN2 = 0;
    At_Step.ATN3 = 0;
    At_Step.ATN4 = 0;
    
    TimS_EnAT = 0;
    TimS_AtMax = 0; //自整定时间清零
    AtMax = TempSetVal;
    AtMin = TempSetVal;
    
    if (TempMeasure > TempSetVal) At_Step.ATN1 = 1;
}

/*******************************************************************************
Function name: AT_pid
Descriptions: PID自整定
*******************************************************************************/
void AT_pid(void)
{
    u16 temp;
    s32 a;
    
    if (Tflag.EnAT == 0) return;
    
    if (TimS_EnAT >= 1) //1秒钟进一次
    {
        TimS_EnAT = 0;
        TimS_AtMax++;
        if (TimS_AtMax >= 18000)
        {
            Tflag.EnAT = 0; //5小时 最大限时
        }
        else
        {
            if (At_Step.ATN1 == 0)
	    {
	        if (TempMeasure <= (TempSetVal + 3))
                {
                    At_Step.ATJR = 1;
                }
	        else
                {
                    At_Step.ATN1 = 1;
                    At_Step.ATJR = 0;
                }
	    }
	    else
	    {
	        if (At_Step.ATN2 == 1)
	        {
	            if (TempMeasure <= TempSetVal + 3) //第一次波谷
		    {
		        TimS_Bg++;
		        if (TempMeasure <= AtMin) AtMin = TempMeasure; //得到最小值
		    }
		    else
                    {
                        At_Step.ATN2=0; At_Step.ATN3=1; At_Step.ATJR=0; TimS_Bf=0; //波峰时间清零
                    }
	        }
	        else if (At_Step.ATN3 == 1)
	        {
	            if (TempMeasure >= TempSetVal-3) //第二次波峰
		    {
		        TimS_Bf++;
		        if (TempMeasure >= AtMax)  AtMax = TempMeasure; //得到最大值
		    }
		    else { At_Step.ATN3 = 0; At_Step.ATN4 = 1;}
	        }
	        else if (At_Step.ATN4 == 1)
	        {
	            ParaTempPID.Temp_p = (s32)(AtMax-AtMin)*4; //得到比例带值
		    if (ParaTempPID.Temp_p <= 5) ParaTempPID.Temp_p = 5;
		    if (ParaTempPID.Temp_p >= 3000) ParaTempPID.Temp_p = 3000;
                    
		    a = (s32)TimS_Bg * TimS_Bf;
		    temp = a / (TimS_Bg + TimS_Bf); //得到纯滞后时间
                    if (temp < 1) temp = 1;
                    if (temp > 1000) temp = 1000;
                    
                    ParaTempPID.Temp_i = temp * 5 / 2; //积分时间为2.5倍的纯滞后时间
                    ParaTempPID.Temp_d = temp * 5 / 3; //微分时间为1.67倍的纯滞后时间
                    if (ParaTempPID.Temp_d > 500) ParaTempPID.Temp_d = 500;
		    
                    Tflag.EnAT = 0; //允许自整定清零
                    Tflag.ATDis = 0;//自整定显示清零
                    
                    //更新EEPROM
		    EEpromWrite((u8 *)(&ParaTempPID), EE_TEMPARAONE, sizeof(ParaTempPID));
	        }
	        else
	        {
	            if (TempMeasure >= TempSetVal - 3) At_Step.ATJR = 0;
		    else { At_Step.ATN2 = 1; At_Step.ATJR = 1;  TimS_Bg = 0;} //波谷时间清零
	        }
            }
	}
    }
}

/*******************************************************************************
	Function name:	      TempAlarm
	Descriptions:	      Temperature alarm		
	Parameters:	      void
	Returned value:       void
*******************************************************************************/
void TempAlarm(void)
{
    /********************************* 温度溢出 *******************************/
    //温度溢出检测
    if ((TempDisplay > 4100) || (TempDisplay < -300)) Tflag.TempOver = 1;
    else Tflag.TempOver = 0;
    
    /* 温度无溢出时，超温检测 */
    if (Tflag.TempOver == 0)
    {
    /******************************** 超温检测 ********************************/
        if (TempDisplay > ParaTempCtrl.bl) Tflag.TempAlm = 1;
        else
        {
            if (TempDisplay > (TempSetVal + ParaTempPID.alh + 1)) Tflag.TempAlm = 1;
            if (TempDisplay < (TempSetVal + ParaTempPID.alh - 1)) Tflag.TempAlm = 0;
        }
        
        //加热继电器控制
        if ((Tflag.EnTempRun != 0) || (Tflag.EnAT != 0))
        {
            if (Tflag.TempAlm != 0)
            {
                ALM_OFF; //超温时 报警 关闭
            }
            else
            {
                ALM_ON;  //运行即开
            }
        }
        
        //超温蜂鸣器处理
        if (Tflag.TempAlm != 0)  //溫度报警
        {
            //超温下，非手动且非自整定下，蜂鸣器响
            if ((Tflag.ManlAlm == 0) && (Tflag.EnAT == 0))
            {
                Tflag.TAlmSpk = 1;
            }
            else
            {
                Tflag.TAlmSpk = 0;
            }
        }
        else  //无超温，清零
        {
            Tflag.ManlAlm = 0;
            Tflag.TAlmSpk = 0;
        }
    }
    else
    {
        //自整定清零
        Tflag.EnAT = 0;
    }
}

