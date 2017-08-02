


/*------------------------------------------------------------------------------
***********************************ͷ�ļ�����***********************************
------------------------------------------------------------------------------*/
#include "stm8s.h"
#include "MotorDriver.h"

//�������״̬
CONTROL_ENUM     ControlStatus;
//�ṹ��
SpeedFlag_STRUCT Mflag;        //������У��¼�����־λ
SpeedPara_STRUCT ParaSpd;      //�ٶȲ�����
Ramp_STRUCT      Ramp;         //б��
MPID_STRUCT      Mpid;         //PID�������

//�ٶ�ֵ������
u16 SpdSetVal;    //�ٶ��趨ֵ
u16 SpdDisplay;   //�ٶ���ʾֵ
u16 SpdMeasureVal;//�ٶȲ���ֵ
u32 SpdFilter;    //�ٶ��˲�ֵ

//PWM�ֱ���
u16 PWMResolution;

u32 TIM2_CaptureBuff[2] = {0xFFFFFF, 0xFFFFFF};

//ʱ�����
u8 TimMs_StepWait;      //б��һ��ʱ��
u8 TimMs_UpdatePWM;     //PWM��ֵ���¼�ʱ
u8 TimMs_SpeedCalculate;//�����ٶ���ʱ
u16 TimMs_SpdToDisplay; //�ٶ���ʾֵ500msˢ��
u8 TimS_HallDetect;     //�������ϼ��3s�ۼƼ�ʱ
u8 TimMs_MotorTrig;     //2ms�����������
/*------------------------------------------------------------------------------
********************************** �ڲ��������� ********************************
------------------------------------------------------------------------------*/
/* IDLE */
void PIDInit(void);      //PID��ʼ��
void AccDecUpdate(void); //���㲽��ֵ
void PWMOutInit(void);   //PWM��ʼ��
/* RUN */
s32 RampCalculate(s32 ramp_out, s32 ramp_hope, s32 ramp_acc, s32 ramp_dec);//б��ֵ����
s16 PIDRegulator(s16 speed_aim,s16 speed_present);//PID����
/*******************************************************************************
         Function name:    MotorCtrl
	  Descriptions:	   Handle the motor control status
	    Parameters:	   void
        Returned value:    void
------------------------------------------------------------------------------*/
void MotorCtrl(void)
{
    /* ���㲽�� */
    AccDecUpdate();
    
    switch(ControlStatus)
    {
        case IDLE:
            if(Mflag.EnMotorRun != 0) //����
            {
                /* ��ʼ����ֵ */
                PIDInit();
                
                /* pwmб����ռ�ձ����� */
                PWMOutInit();
                
                /* START */
                ControlStatus = START;
            }
            break;
            
        case START:
            TIM1_CtrlPWMOutputs(ENABLE); //��ɲ��
            
            /* �������ϼ�ʱ���� */
            TimS_HallDetect = 0;
            
            /* RUN */
            ControlStatus = RUN;
            break;
            
        case RUN:
            /* �������ϼ�� */
            if(SpdMeasureVal == 0)
            {
                if(TimS_HallDetect >= 5)  //5s������λ
                {
                    Mflag.EnMotorRun = 0;
                    Mflag.ErrHall = 1;
                }
            }
            else
            {
                if(TimS_HallDetect >= 3)  //3s������λ
                {
                    Mflag.EnMotorRun = 0;
                    Mflag.ErrHall = 1;
                }
            }
            
            if(Mflag.EnMotorRun == 0) //ͣ��
            {
                Ramp.DesiredVal = 0;
                if(SpdMeasureVal <= ParaSpd.sdl)
                {
                    /* STOP */
                    ControlStatus = STOP;
                }
            }
            else /* ���� */
            {
                /* б�º�������ֵ */
                Ramp.DesiredVal = (u32)SpdSetVal << 10; //���ʵ������ֵ��ֵ������ֵ
            }
            
            /* б�¼��� */
            if(TimMs_StepWait >= RAMP_TIME)
            {
                TimMs_StepWait = 0;
                Ramp.ActualVal = RampCalculate(Ramp.ActualVal,
                                               Ramp.DesiredVal,
                                               Ramp.StepForward * RAMP_TIME,
                                               Ramp.StepBack * RAMP_TIME);
            }
            
            /* �����ѹ(PWMֵ)���� */
            if(TimMs_UpdatePWM >= 5)
            {
                TimMs_UpdatePWM = 0;
                
                //PWMռ�ձȼ���
                Mpid.DutyRatio = PIDRegulator((s16)(Ramp.ActualVal>>10), SpdMeasureVal);
                
                //��ռ�ձȷ�ֵ
                if (Mpid.DutyRatio >= PWMResolution) Mpid.DutyRatio = PWMResolution;
                
                //PWM��ֵ���
                TIM1_SetCompare1(Mpid.DutyRatio);
            }
            break;
            
        case STOP:
            TIM1_CtrlPWMOutputs(DISABLE);//ɲ��
            
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
    Mpid.DutyRatio = 0;  //ռ�ձ����㣨��PID�������
    Ramp.ActualVal = 0;  //ֹͣ״̬��RAMP��ǰ���������
    Ramp.DesiredVal = 0; //б������
//    Ramp.StepForward = 0;//б�¼���
//    Ramp.StepBack = 0;   //б�¼���
}

/*******************************************************************************
         Function name:    PIDInit
	  Descriptions:	   The PID initial
	    Parameters:	   void
        Returned value:    void
------------------------------------------------------------------------------*/
void PIDInit(void)
{
    /* �����ۻ������� */
    Mpid.ISumVaule = 0;
    
    /* ������������ */
    Mpid.KpFactor = SPEED_KP_DIVISOR;  //128
    Mpid.KiFactor = SPEED_KI_DIVISOR;  //512
    
    /* PID��������� */
    Mpid.OutLowLimit = SPEED_OUT_MIN;
    Mpid.OutUpLimit = ((u32)PWMResolution - 1);
    
    /* �����ۼӺ������� */
    Mpid.ISumLowLimit = SPEED_KISUM_MIN;
    Mpid.ISumUpLimit = SPEED_KI_DIVISOR * ((u32)PWMResolution - 1);
}

/*******************************************************************************
         Function name:    PIDRegulator
	  Descriptions:	   Regulator the PID 	
	    Parameters:	   s32 speed_aim,s32 speed_present
        Returned value:    s16�������ۻ��� ȫ�֣�
Mpid.ISumVaule   
Mpid.ISumUpLimit
Mpid.ISumLowLimit
Mpid.OutUpLimit
Mpid.OutLowLimit
PID.pd id
Mpid.KpFactor ��������
Mpid.KiFactor ��������
------------------------------------------------------------------------------*/
s16 PIDRegulator(s16 speed_aim, s16 speed_present) //�������  ʵ��ת��
{
    s32 speed_distance;//PID����ƫ����
    s32 pid_out;       //PID������
    s32 pid_out_p;     //���������
    s32 pid_out_i;     //���������
    s32 pid_i_single;  //���λ���ֵ
    
    speed_distance = (s32)(speed_aim - speed_present); //�����ٶ�ƫ��
    
    pid_out_p = ((s32)ParaSpd.pd * speed_distance); //������������
    
    if(ParaSpd.id == 0)
    {
        Mpid.ISumVaule = 0;
    }
    else
    {
        pid_i_single = ((s32)ParaSpd.id * speed_distance);//���������
        pid_out_i = Mpid.ISumVaule + pid_i_single;    //������������
	
        /* ��ֹ���ֺ����������s32��Χ */
	if (pid_out_i > 0) //��ֹ�����ۼӺ������
	{
	    if (Mpid.ISumVaule < 0)
	    {
	        if(pid_i_single < 0)
                {
                    pid_out_i = S32_MIN;
                }
	    }
	}
	else //��ֹ�����ۼӺ������
	{
	    if(Mpid.ISumVaule > 0)
	    {
		if (pid_i_single > 0)
                {
                    pid_out_i = S32_MAX;
                }
	    }
	}
        
        /* ���ֺ��޷� Mpid.ISumUpLimit */
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
    
    /* PID������������ո�ֵ�������ۻ������˴������ۻ������������� */
    pid_out = (pid_out_p / Mpid.KpFactor) + (Mpid.ISumVaule / Mpid.KiFactor);
    
    //PID����޷�
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
    
    //ÿ��5mS����һ���ٶ�
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
            
            //��ʾ������
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
	    Parameters:	   �����ޣ��Ӽ���ʱ�䣬�Ӽ��ٲ���
        Returned value:    void
------------------------------------------------------------------------------*/
void AccDecUpdate(void)
{
    u32 temp;
    u16 speedval_range; //�ٶ��趨�����޷�Χ������ʾ��Χ�õ����ʵ���з�Χ
    
    //Ϊ��������㾫�ȣ�����趨�ٶȱ仯��*1024��(����10λ)
    //ÿ1mS���ӡ����ٵ�ת��ֵ=((����������ֵ - ��Сֵ)<<10)/(1000*AccelerateTime)
    speedval_range = ParaSpd.sdh - ParaSpd.sdl;
    if (speedval_range < 2000)
    {
        speedval_range = 2000;  //�޶����㲽��ʱ�������С���з�Χ1500RPM
    }
    
    temp = (u32)speedval_range << 10;
    Ramp.StepForward = temp / (1000 * (u32)ParaSpd.intt);//ÿ1mS���ӵ�ת��ֵ
    Ramp.StepBack = temp / (1000 * (u32)ParaSpd.det);    //ÿ1mS���ٵ�ת��ֵ
}

/*******************************************************************************
 ��� = (��������������ٲ��������ٲ���)
------------------------------------------------------------------------------*/
s32 RampCalculate(s32 ramp_out, s32 ramp_hope, s32 ramp_acc, s32 ramp_dec)
{
    //��� != ���������� or ����
    if (ramp_out != ramp_hope)
    {
        if (ramp_out < ramp_hope) //����
        {
            ramp_out += ramp_acc;
            
            if (ramp_out >= ramp_hope)
            {
                ramp_out = ramp_hope;
            }
        }
        else  //����
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

