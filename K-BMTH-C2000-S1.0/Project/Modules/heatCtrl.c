


/*------------------------------------------------------------------------------
***********************************ͷ�ļ�����***********************************
------------------------------------------------------------------------------*/
#include "stm8s.h"
#include "heatCtrl.h"
#include "eepromCtrl.h" //���������󣬲�������
/*------------------------------------------------------------------------------
********************************** ȫ�� ���� ***********************************
------------------------------------------------------------------------------*/
//��־λ
TFlag_STRUCT Tflag;//�¶ȱ�־λ�ṹ�嶨��

//�¶Ȳ������
ParaTone_STRUCT ParaTempPID;  //�������һ����
ParaTtwo_STRUCT ParaTempCtrl; //������������

//�¶�ֵ
s16 TempSetVal;     //�¶��趨ֵ
s16 TempMeasure;    //�¶Ȳ���ֵ
s16 TempDisplay;    //�¶���ʾֵ

//PID�¿���ʵ���в���
TempPID_SRTUCT Tpid;

//����������
ATSTEP_STRUCT At_Step;//������ ����
u16  TimS_AtMax;  //�����������ʱ��
s16  AtMax;       //�������õ�������¶Ȳ���ֵ
s16  AtMin;       //�������õ�����С�¶Ȳ���ֵ
u16  TimS_Bg;     //����ʱ��
u16  TimS_Bf;     //����ʱ��
//ʱ�����
u16 TimS_HeatPeriod;  //�������ڼ�ʱ
u8  TimS_EnAT;        //������ʱ���ʱ
u16 TimMs_HeatCounter;//��������ռ�ձȼ�ʱ
/*------------------------------------------------------------------------------
********************************* �ڲ��������� *********************************
------------------------------------------------------------------------------*/
void PIDControl(void);//PID���㺯��
/*******************************************************************************
	Function name:	      TempMesToDisp
	Descriptions:	      Control the temperature heat
	Parameters:	      void
	Returned value:       void
*******************************************************************************/
void TempADToDisp(void)
{
    s16 temp_filter;   //�ȵ�ż�˲�ֵ
    
    if (Pt100.FlagOK != 0)
    {
        Pt100.FlagOK  = 0;
        
        temp_filter = DataFilter(Pt100.SmplVal);//�����˲�
        TempMeasure = CalculateTemp(temp_filter, ParaTempPID.pb, ParaTempPID.pl);//����ֵ����
        
        //��ʾ������
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
//    //��ʾ������
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
    
    if (Tflag.EnAT != 0) //���������ȿ���
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
        
        //��PID��������ֵ
        PIDReset();
        
        return;
    }
    
    //�¿عر� or �¶����
    if ((Tflag.EnTempRun == 0) || (Tflag.TempOver != 0))
    {
        ALM_OFF;
        HEAT_OFF;
        Tflag.HeatDis = 0; //no "HEAT"
        
        //��PID��������ֵ
        PIDReset();
    }
    else //�������ȿ���
    {
        //��������
        if (TimS_HeatPeriod >= ParaTempPID.Temp_t)
        {
            TimS_HeatPeriod = 0;  //���pwm����
            TimMs_HeatCounter = 0;
            
            //������ʿ���
            PIDControl();
            temp_a = Tpid.Result * ParaTempPID.Temp_t;//PID_OutResultȡֵ��Χ 0~1000
            temp_b = (u32)temp_a * ParaTempCtrl.np;
            Tpid.Result = (u16)(temp_b / 100);
        }
        else
        {
            if (Tflag.TempAlm != 0) //����ֹͣ����
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
    Tpid.ISum = 0;      //�����ۻ�����
    Tpid.DResult = TempMeasure;  //΢���������ֵΪ�����루�¶Ȳ���ֵ��
    Tpid.LastMeasure = TempMeasure; //�¶ȼ�¼ֵ����
}

/*******************************************************************************
*	Function name:	      PIDControl
*	Descriptions:	      Control the PID out
*	Parameters:	      TempSetVal  TempMeasure  Tpid.LastMeasure
*                             ΢�֣�Tpid.DVal  Tpid.DResult  Tpid.TVal
*                                   TempMeasure  Tpid.LastMeasure
*                             ���֣�Tpid.ISum
*                                   Tpid.PkVal  Tpid.IVal
*	Returned value:       Tpid.Result
*******************************************************************************/
void PIDControl(void)
{
    s16 temp_a,temp_b;
    s32 temp_c,temp_d;
    
    Tpid.TVal = ParaTempPID.Temp_t; //΢������ʱ��
    Tpid.PkVal = ParaTempPID.Temp_p;//����
    Tpid.IVal = ParaTempPID.Temp_i; //����ֵ
    Tpid.DVal = ParaTempPID.Temp_d; //�õ�΢�ֵ���ʵ���п���ֵ
    
    if ((TempSetVal - Tpid.DResult) >= Tpid.PkVal) //�±�������
    {
	Tpid.Result = 1000; //ȫ�������
        
        //��PID��������ֵ
        PIDReset();
    }
    else
    {
	if ((Tpid.DResult - TempSetVal) >= Tpid.PkVal) //�ϱ�������
	{
	    Tpid.Result = 0; //�ر����
            
            //��PID��������ֵ
            PIDReset();
	}
	else
	{
            /*************************** ΢������(�޽�ֹ) *****************************/
            temp_c = (s32)(TempMeasure - Tpid.LastMeasure) * Tpid.DVal * 6;
            temp_c = temp_c + (s32)Tpid.DVal * Tpid.DResult;
            temp_c = temp_c + (s32)TempMeasure * Tpid.TVal * 6;
            temp_d = temp_c / (Tpid.DVal + Tpid.TVal * 6);
            
            //΢�������ޱ��趨ֵ��50.0��
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
            
            //�������봦��
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
            
            /******************* end of ΢�� ����� Tpid.DResult **********************/
            
            /****************************** ���ٻ��� **********************************/
            //���ٻ��ִ���
	    if(TempMeasure < TempSetVal)
	    {
	        temp_a = Tpid.PkVal / 3;  //1/3������
	        if(TempSetVal - TempMeasure > temp_a)
	        {
	            temp_b = Tpid.PkVal;
	            temp_c = TempSetVal - Tpid.DResult;
	            temp_b = temp_c * (temp_b - (TempSetVal - TempMeasure)) / (temp_b - temp_a);
	        }
	        else temp_b = TempSetVal - Tpid.DResult;  //ȫ�� ���
	    }
	    else temp_b = TempSetVal - Tpid.DResult;//��Ҫ�½�����δ�½����������
                                                     //�²�С��ȫ�٣��ޱ���
            
	    temp_c = (s32)temp_b * 1000;
	    temp_c = (temp_c * Tpid.TVal) / Tpid.PkVal;
	    Tpid.ISum = Tpid.ISum + temp_c;
	    if (Tpid.ISum < 0) Tpid.ISum = 0;/* ����С���㣬������ ������Ч */
	    temp_a = Tpid.ISum / Tpid.IVal;
            
	    if(temp_a >= 1000)
	    {
	        temp_a = 1000;
                Tpid.ISum = (s32)Tpid.IVal * 1000;
	    }
            
            //��������
	    temp_c = (s32)(TempSetVal - Tpid.DResult) * 1000;
	    temp_b = temp_c / Tpid.PkVal;
            
	    temp_b = temp_a + temp_b;
	    if (temp_b < 0) Tpid.Result = 0;
	    else if (temp_b > 1000) Tpid.Result = 1000;
	    else Tpid.Result = temp_b;
	}
    }
    /***************** end of ���ٻ��� ����� Tpid.Result *******************/
}

/*******************************************************************************
                         -------- ������ --------
*******************************************************************************/

/*******************************************************************************
Function name: AT_ini
Descriptions: PID��������ʼ��
*******************************************************************************/
void AT_ini(void)
{
    At_Step.ATN1 = 0;
    At_Step.ATN2 = 0;
    At_Step.ATN3 = 0;
    At_Step.ATN4 = 0;
    
    TimS_EnAT = 0;
    TimS_AtMax = 0; //������ʱ������
    AtMax = TempSetVal;
    AtMin = TempSetVal;
    
    if (TempMeasure > TempSetVal) At_Step.ATN1 = 1;
}

/*******************************************************************************
Function name: AT_pid
Descriptions: PID������
*******************************************************************************/
void AT_pid(void)
{
    u16 temp;
    s32 a;
    
    if (Tflag.EnAT == 0) return;
    
    if (TimS_EnAT >= 1) //1���ӽ�һ��
    {
        TimS_EnAT = 0;
        TimS_AtMax++;
        if (TimS_AtMax >= 18000)
        {
            Tflag.EnAT = 0; //5Сʱ �����ʱ
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
	            if (TempMeasure <= TempSetVal + 3) //��һ�β���
		    {
		        TimS_Bg++;
		        if (TempMeasure <= AtMin) AtMin = TempMeasure; //�õ���Сֵ
		    }
		    else
                    {
                        At_Step.ATN2=0; At_Step.ATN3=1; At_Step.ATJR=0; TimS_Bf=0; //����ʱ������
                    }
	        }
	        else if (At_Step.ATN3 == 1)
	        {
	            if (TempMeasure >= TempSetVal-3) //�ڶ��β���
		    {
		        TimS_Bf++;
		        if (TempMeasure >= AtMax)  AtMax = TempMeasure; //�õ����ֵ
		    }
		    else { At_Step.ATN3 = 0; At_Step.ATN4 = 1;}
	        }
	        else if (At_Step.ATN4 == 1)
	        {
	            ParaTempPID.Temp_p = (s32)(AtMax-AtMin)*4; //�õ�������ֵ
		    if (ParaTempPID.Temp_p <= 5) ParaTempPID.Temp_p = 5;
		    if (ParaTempPID.Temp_p >= 3000) ParaTempPID.Temp_p = 3000;
                    
		    a = (s32)TimS_Bg * TimS_Bf;
		    temp = a / (TimS_Bg + TimS_Bf); //�õ����ͺ�ʱ��
                    if (temp < 1) temp = 1;
                    if (temp > 1000) temp = 1000;
                    
                    ParaTempPID.Temp_i = temp * 5 / 2; //����ʱ��Ϊ2.5���Ĵ��ͺ�ʱ��
                    ParaTempPID.Temp_d = temp * 5 / 3; //΢��ʱ��Ϊ1.67���Ĵ��ͺ�ʱ��
                    if (ParaTempPID.Temp_d > 500) ParaTempPID.Temp_d = 500;
		    
                    Tflag.EnAT = 0; //��������������
                    Tflag.ATDis = 0;//��������ʾ����
                    
                    //����EEPROM
		    EEpromWrite((u8 *)(&ParaTempPID), EE_TEMPARAONE, sizeof(ParaTempPID));
	        }
	        else
	        {
	            if (TempMeasure >= TempSetVal - 3) At_Step.ATJR = 0;
		    else { At_Step.ATN2 = 1; At_Step.ATJR = 1;  TimS_Bg = 0;} //����ʱ������
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
    /********************************* �¶���� *******************************/
    //�¶�������
    if ((TempDisplay > 4100) || (TempDisplay < -300)) Tflag.TempOver = 1;
    else Tflag.TempOver = 0;
    
    /* �¶������ʱ�����¼�� */
    if (Tflag.TempOver == 0)
    {
    /******************************** ���¼�� ********************************/
        if (TempDisplay > ParaTempCtrl.bl) Tflag.TempAlm = 1;
        else
        {
            if (TempDisplay > (TempSetVal + ParaTempPID.alh + 1)) Tflag.TempAlm = 1;
            if (TempDisplay < (TempSetVal + ParaTempPID.alh - 1)) Tflag.TempAlm = 0;
        }
        
        //���ȼ̵�������
        if ((Tflag.EnTempRun != 0) || (Tflag.EnAT != 0))
        {
            if (Tflag.TempAlm != 0)
            {
                ALM_OFF; //����ʱ ���� �ر�
            }
            else
            {
                ALM_ON;  //���м���
            }
        }
        
        //���·���������
        if (Tflag.TempAlm != 0)  //�ضȱ���
        {
            //�����£����ֶ��ҷ��������£���������
            if ((Tflag.ManlAlm == 0) && (Tflag.EnAT == 0))
            {
                Tflag.TAlmSpk = 1;
            }
            else
            {
                Tflag.TAlmSpk = 0;
            }
        }
        else  //�޳��£�����
        {
            Tflag.ManlAlm = 0;
            Tflag.TAlmSpk = 0;
        }
    }
    else
    {
        //����������
        Tflag.EnAT = 0;
    }
}

