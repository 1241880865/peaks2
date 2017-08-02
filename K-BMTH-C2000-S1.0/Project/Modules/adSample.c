

/*------------------------------------------------------------------------------
***********************************ͷ�ļ�����***********************************
----Includes -----------------------------------------------------------------*/
#include "stm8s.h"
#include "adSample.h"
/*------------------------------------------------------------------------------
********************************* �������� *************************************
------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
Pt100_STRUCT Pt100Std;    //���� and ���
Smpl_STRUCT Pt100;

const s16 Pt_temp[9]={-500,40,680,1320,1960,2600,3240,3880,4090};
const s16 Pt_val[9]={0,1314,2818,4270,5673,7027,8335,9598,10000};

//�˲���������
u16 DataFilter_Buff[5];
/*******************************************************************************
	Function name:	      CalculateTemp
	Descriptions:	      Calculate the temperature
	Parameters:	      s16 ���������˲�ֵ��s16 �¶�������s16 б������
	Returned value:       s16 �¶Ȳ���ֵ
*******************************************************************************/
s16 CalculateTemp(s16 filterval, s16 pb, s16 pl)
{
    u8  i = 0;
    s32 temp_a = 0;
    s32 temp = 0;
    
    //����Pt100���¶ȡ�15λAD;5Vϵͳ��
    temp_a = ((s32)filterval - Pt100Std.Zero_TL) * 10000;
    filterval = Pt100Std.Full_TH - Pt100Std.Zero_TL;
    
    if (filterval > 0)
    {
        filterval = temp_a / filterval;
        
        if(filterval > 10000)
        {
            temp_a = (s32)(Pt_temp[8] - Pt_temp[7]) * (filterval - Pt_val[7]);
            temp = temp_a / (Pt_val[8] - Pt_val[7]);
            temp = temp + Pt_temp[7];
        }
        else
        {
            for(i = 0; i < 8; i++)
            {
                if(filterval <= Pt_val[i+1])
                {
                    temp_a = (s32)(Pt_temp[i+1] - Pt_temp[i]) * (filterval - Pt_val[i]);
                    temp = temp_a / (Pt_val[i+1] - Pt_val[i]);
                    temp = temp + Pt_temp[i];
                    break;
                }
            }
        }
    }
    else
    {
        temp = 4200;
    }
    
    //б������
    temp_a = temp + pb;
    temp_a = (s32)(pl + 1000) * temp_a;
    temp_a = temp_a / 1000;
    
    return(temp_a);
}

/*******************************************************************************
	Function name:	      DataFilter
	Descriptions:	      ȥ��ֵ����ƽ��ֵ�˲�
	Parameters:	      u16 sampleval
	Returned value:       s16
*******************************************************************************/
s16 DataFilter(u16 sampleval)
{
    u8  i;
    s16 temp = 0;
    u16 filtermax, filtermin;
    u32 filtersum = 0;
    
    for(i = 4; i > 0; i--)  //���� ��λ
    {
	DataFilter_Buff[i] = DataFilter_Buff[i-1];
    }
    
    filtermax = sampleval;
    filtermin = sampleval;
    DataFilter_Buff[0] = sampleval;
    
    for(i = 0; i < 5; i++)
    {
   	if(DataFilter_Buff[i] > filtermax)  //�����ֵ
	{
    	    filtermax = DataFilter_Buff[i];
	}
	if(DataFilter_Buff[i] < filtermin)  //����Сֵ
	{
	    filtermin = DataFilter_Buff[i];
	}
	
	filtersum = filtersum + DataFilter_Buff[i]; //���
    }
    
    temp = (s16)((filtersum - filtermax - filtermin) / 3);  //��ƽ��
    
    return(temp);
}

