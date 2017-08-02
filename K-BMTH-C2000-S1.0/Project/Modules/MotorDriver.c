


/*------------------------------------------------------------------------------
***********************************头文件包含***********************************
------------------------------------------------------------------------------*/
#include "stm8s.h"
#include "MotorDriver.h"

//电机运行状态
CONTROL_ENUM     ControlStatus;
//结构体
SpeedFlag_STRUCT Mflag;        //电机运行（事件）标志位
SpeedPara_STRUCT ParaSpd;      //速度参数表
Ramp_STRUCT      Ramp;         //斜坡
MPID_STRUCT      Mpid;         //PID计算参数

//速度值计算量
u16 SpdSetVal;    //速度设定值
u16 SpdDisplay;   //速度显示值
u16 SpdMeasureVal;//速度测量值
u32 SpdFilter;    //速度滤波值

//PWM分辨率
u16 PWMResolution;

u32 TIM2_CaptureBuff[2] = {0xFFFFFF, 0xFFFFFF};

//时间变量
u8 TimMs_StepWait;      //斜坡一步时间
u8 TimMs_UpdatePWM;     //PWM赋值更新计时
u8 TimMs_SpeedCalculate;//计算速度延时
u16 TimMs_SpdToDisplay; //速度显示值500ms刷新
u8 TimS_HallDetect;     //霍尔故障检测3s累计计时
u8 TimMs_MotorTrig;     //2ms触发电机控制
/*------------------------------------------------------------------------------
********************************** 内部函数声明 ********************************
------------------------------------------------------------------------------*/
/* IDLE */
void PIDInit(void);      //PID初始化
void AccDecUpdate(void); //计算步长值
void PWMOutInit(void);   //PWM初始化
/* RUN */
s32 RampCalculate(s32 ramp_out, s32 ramp_hope, s32 ramp_acc, s32 ramp_dec);//斜坡值计算
s16 PIDRegulator(s16 speed_aim,s16 speed_present);//PID计算
/*******************************************************************************
         Function name:    MotorCtrl
	  Descriptions:	   Handle the motor control status
	    Parameters:	   void
        Returned value:    void
------------------------------------------------------------------------------*/
void MotorCtrl(void)
{
    /* 计算步长 */
    AccDecUpdate();
    
    switch(ControlStatus)
    {
        case IDLE:
            if(Mflag.EnMotorRun != 0) //启动
            {
                /* 初始化赋值 */
                PIDInit();
                
                /* pwm斜坡与占空比清零 */
                PWMOutInit();
                
                /* START */
                ControlStatus = START;
            }
            break;
            
        case START:
            TIM1_CtrlPWMOutputs(ENABLE); //开刹车
            
            /* 霍尔故障计时清零 */
            TimS_HallDetect = 0;
            
            /* RUN */
            ControlStatus = RUN;
            break;
            
        case RUN:
            /* 霍尔故障检测 */
            if(SpdMeasureVal == 0)
            {
                if(TimS_HallDetect >= 5)  //5s故障置位
                {
                    Mflag.EnMotorRun = 0;
                    Mflag.ErrHall = 1;
                }
            }
            else
            {
                if(TimS_HallDetect >= 3)  //3s故障置位
                {
                    Mflag.EnMotorRun = 0;
                    Mflag.ErrHall = 1;
                }
            }
            
            if(Mflag.EnMotorRun == 0) //停机
            {
                Ramp.DesiredVal = 0;
                if(SpdMeasureVal <= ParaSpd.sdl)
                {
                    /* STOP */
                    ControlStatus = STOP;
                }
            }
            else /* 运行 */
            {
                /* 斜坡函数期望值 */
                Ramp.DesiredVal = (u32)SpdSetVal << 10; //电机实际运行值赋值坡期望值
            }
            
            /* 斜坡计算 */
            if(TimMs_StepWait >= RAMP_TIME)
            {
                TimMs_StepWait = 0;
                Ramp.ActualVal = RampCalculate(Ramp.ActualVal,
                                               Ramp.DesiredVal,
                                               Ramp.StepForward * RAMP_TIME,
                                               Ramp.StepBack * RAMP_TIME);
            }
            
            /* 输出电压(PWM值)更新 */
            if(TimMs_UpdatePWM >= 5)
            {
                TimMs_UpdatePWM = 0;
                
                //PWM占空比计算
                Mpid.DutyRatio = PIDRegulator((s16)(Ramp.ActualVal>>10), SpdMeasureVal);
                
                //限占空比幅值
                if (Mpid.DutyRatio >= PWMResolution) Mpid.DutyRatio = PWMResolution;
                
                //PWM赋值输出
                TIM1_SetCompare1(Mpid.DutyRatio);
            }
            break;
            
        case STOP:
            TIM1_CtrlPWMOutputs(DISABLE);//刹车
            
            /* IDLE */
            ControlStatus = IDLE;
            break;
            
        default:
            break;
    }
}

/*******************************************************************************
         Function name:    PWMOutInit
	  Descriptions:	   PWMOutInit
	    Parameters:	   void
        Returned value:    void
------------------------------------------------------------------------------*/
void PWMOutInit(void)
{
    Mpid.DutyRatio = 0;  //占空比清零（仅PID环输出）
    Ramp.ActualVal = 0;  //停止状态下RAMP当前输出项清零
    Ramp.DesiredVal = 0; //斜坡期望
//    Ramp.StepForward = 0;//斜坡加速
//    Ramp.StepBack = 0;   //斜坡减速
}

/*******************************************************************************
         Function name:    PIDInit
	  Descriptions:	   The PID initial
	    Parameters:	   void
        Returned value:    void
------------------------------------------------------------------------------*/
void PIDInit(void)
{
    /* 积分累积和清零 */
    Mpid.ISumVaule = 0;
    
    /* 比例积分因子 */
    Mpid.KpFactor = SPEED_KP_DIVISOR;  //128
    Mpid.KiFactor = SPEED_KI_DIVISOR;  //512
    
    /* PID输出上下限 */
    Mpid.OutLowLimit = SPEED_OUT_MIN;
    Mpid.OutUpLimit = ((u32)PWMResolution - 1);
    
    /* 积分累加和上下限 */
    Mpid.ISumLowLimit = SPEED_KISUM_MIN;
    Mpid.ISumUpLimit = SPEED_KI_DIVISOR * ((u32)PWMResolution - 1);
}

/*******************************************************************************
         Function name:    PIDRegulator
	  Descriptions:	   Regulator the PID 	
	    Parameters:	   s32 speed_aim,s32 speed_present
        Returned value:    s16（积分累积量 全局）
Mpid.ISumVaule   
Mpid.ISumUpLimit
Mpid.ISumLowLimit
Mpid.OutUpLimit
Mpid.OutLowLimit
PID.pd id
Mpid.KpFactor 比例因子
Mpid.KiFactor 积分因子
------------------------------------------------------------------------------*/
s16 PIDRegulator(s16 speed_aim, s16 speed_present) //期望输出  实际转速
{
    s32 speed_distance;//PID输入偏差量
    s32 pid_out;       //PID最后输出
    s32 pid_out_p;     //比例输出项
    s32 pid_out_i;     //积分输出项
    s32 pid_i_single;  //单次积分值
    
    speed_distance = (s32)(speed_aim - speed_present); //计算速度偏差
    
    pid_out_p = ((s32)ParaSpd.pd * speed_distance); //计算比例输出项
    
    if(ParaSpd.id == 0)
    {
        Mpid.ISumVaule = 0;
    }
    else
    {
        pid_i_single = ((s32)ParaSpd.id * speed_distance);//计算积分项
        pid_out_i = Mpid.ISumVaule + pid_i_single;    //计算积分输出项
	
        /* 防止积分和溢出，超过s32范围 */
	if (pid_out_i > 0) //防止积分累加和下溢出
	{
	    if (Mpid.ISumVaule < 0)
	    {
	        if(pid_i_single < 0)
                {
                    pid_out_i = S32_MIN;
                }
	    }
	}
	else //防止积分累加和上溢出
	{
	    if(Mpid.ISumVaule > 0)
	    {
		if (pid_i_single > 0)
                {
                    pid_out_i = S32_MAX;
                }
	    }
	}
        
        /* 积分和限幅 Mpid.ISumUpLimit */
        if (pid_out_i > Mpid.ISumUpLimit)
        {
            Mpid.ISumVaule = Mpid.ISumUpLimit;
        }
        else if (pid_out_i < Mpid.ISumLowLimit)
        {
            Mpid.ISumVaule = Mpid.ISumLowLimit;
        }
        else Mpid.ISumVaule = pid_out_i;
    }
    
    /* PID积分输出项最终赋值给积分累积量，此处积分累积量替代积分输出 */
    pid_out = (pid_out_p / Mpid.KpFactor) + (Mpid.ISumVaule / Mpid.KiFactor);
    
    //PID输出限幅
    if (pid_out > Mpid.OutUpLimit)
    {
        pid_out = Mpid.OutUpLimit;
    }
    else if (pid_out < Mpid.OutLowLimit)
    {
        pid_out = Mpid.OutLowLimit;
    }
    
    return((s16)(pid_out));
}

/*******************************************************************************
         Function name:    SpeedCalculate
	  Descriptions:	   Calculate the speed value				
	    Parameters:	   void
        Returned value:    void							 
------------------------------------------------------------------------------*/ 
void SpeedCalculate(void)
{   
    u32 temp1, temp2;
    
    //每隔5mS计算一次速度
    if (TimMs_SpeedCalculate >= 5)
    {
        TimMs_SpeedCalculate = 0;
        
        temp1 = 60000000 >> 4;
        temp2 = TIM2_CaptureBuff[0] + TIM2_CaptureBuff[1];
        temp2 = temp2 * ParaSpd.pol;
        
        temp1 = (u16)(temp1 / temp2);
        SpdFilter  = ((SpdFilter * 63) + ((u32)temp1 * 64)) >> 6;
        SpdMeasureVal = (u16)(SpdFilter >> 6);
        
        if (TimMs_SpdToDisplay >= 500)
        {
            TimMs_SpdToDisplay = 0;
            
            //显示防抖动
            if(SpdMeasureVal <= (SpdSetVal - ParaSpd.db)) SpdDisplay = SpdMeasureVal + ParaSpd.db;
            else if(SpdMeasureVal >= (SpdSetVal + ParaSpd.db)) SpdDisplay = SpdMeasureVal - ParaSpd.db; 
            else SpdDisplay = SpdSetVal;
            
            if(SpdDisplay < (ParaSpd.sdl - (ParaSpd.db + 5))) SpdDisplay = 0;
        }
    }
}

/*******************************************************************************
         Function name:    AccDecUpdate
	  Descriptions:	   Update the SpeedAccelerate and SpeedDecelerate
	    Parameters:	   上下限，加减速时间，加减速步长
        Returned value:    void
------------------------------------------------------------------------------*/
void AccDecUpdate(void)
{
    u32 temp;
    u16 speedval_range; //速度设定上下限范围，由显示范围得电机真实运行范围
    
    //为了提高运算精度，电机设定速度变化量*1024倍(左移10位)
    //每1mS增加、减少的转速值=((电机运行最大值 - 最小值)<<10)/(1000*AccelerateTime)
    speedval_range = ParaSpd.sdh - ParaSpd.sdl;
    if (speedval_range < 2000)
    {
        speedval_range = 2000;  //限定计算步长时，电机最小运行范围1500RPM
    }
    
    temp = (u32)speedval_range << 10;
    Ramp.StepForward = temp / (1000 * (u32)ParaSpd.intt);//每1mS增加的转速值
    Ramp.StepBack = temp / (1000 * (u32)ParaSpd.det);    //每1mS减少的转速值
}

/*******************************************************************************
 输出 = (输出，期望，加速步长，减速步长)
------------------------------------------------------------------------------*/
s32 RampCalculate(s32 ramp_out, s32 ramp_hope, s32 ramp_acc, s32 ramp_dec)
{
    //输出 != 期望，加速 or 减速
    if (ramp_out != ramp_hope)
    {
        if (ramp_out < ramp_hope) //加速
        {
            ramp_out += ramp_acc;
            
            if (ramp_out >= ramp_hope)
            {
                ramp_out = ramp_hope;
            }
        }
        else  //减速
        {
            ramp_out -= ramp_dec;
            
            if (ramp_out <= 0)
            {
                ramp_out = 0;
            }
        }
    }
    
    return (ramp_out);
}

