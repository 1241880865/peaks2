


#ifndef __HEATCTRL_H
#define __HEATCTRL_H

#include "adSample.h" //����
/*------------------------------------------------------------------------------
*************************************�궨��*************************************
------------------------------------------------------------------------------*/
#define HEAT_ON      GPIO_WriteLow(GPIOC, GPIO_PIN_2) //���ȿ��궨��
#define HEAT_OFF     GPIO_WriteHigh(GPIOC, GPIO_PIN_2)//���ȹغ궨��
#define ALM_ON       GPIO_WriteLow(GPIOC, GPIO_PIN_3) //�������궨��
#define ALM_OFF      GPIO_WriteHigh(GPIOC, GPIO_PIN_3)//�����غ궨��

#define TEMP_MAX     4000 //��Ʒ��������(��С��)
#define TEMP_MIN     0    //��Ʒ��������
/*------------------------------------------------------------------------------
********************************* �ṹ�� ���� **********************************
------------------------------------------------------------------------------*/
typedef struct  //������ ����
{
    u8  ATNOW:  1;  //����������
    u8  ATN1:   1;
    u8  ATN2:   1;
    u8  ATN3:   1;
    
    u8  ATN4:   1;
    u8  ATJR:   1;  //������ʱ�������
    u8  RES4:   2;
}
ATSTEP_STRUCT;

typedef struct  //PID�¿���ʵ���в���
{
    s16 LastMeasure;//��һ���¶Ȳⶨֵ
    u16 TVal;       //PID��������
    s16 PkVal;      //����ֵ
    s32 ISum;       //�����ۼ�ֵ
    s16 IVal;       //����ֵ
    s16 DVal;       //΢��ֵ
    s16 DResult;    //��΢�����к�΢��ֵ
    s16 Result;     //��pid�������
}
TempPID_SRTUCT;

//�ڲ�������һ
typedef struct
{
    s16 alh;
    s16 Temp_t;
    s16 Temp_p;
    s16 Temp_i;
    s16 Temp_d;
    s16 pb;
    s16 pl;
    s16 adr;
}
ParaTone_STRUCT;

//�ڲ��������
typedef struct
{
    s16 pon;
    s16 rut;
    s16 bl;
    s16 np;
    s16 spl;
    s16 sph;
}
ParaTtwo_STRUCT;

//�¶ȱ�־λ
typedef struct
{
    u8 EnTempRun; //�¶���������
    u8 EnAT;      //����������
    
    u8 TempAlm;   //���±�����־λ
    u8 TempOver;  //�¶������־λ
    
    u8 HeatDis;   //�¶������ʾ��־λ
    u8 ATDis;     //��������ʾ
    u8 ManlAlm;   //�ֶ�����
    u8 TAlmSpk;   //���·���������
}
TFlag_STRUCT;

/*******************************************************************************
                    ---------------- ���� -----------------
*******************************************************************************/
//�¶ȱ�־λ
extern TFlag_STRUCT Tflag;

//�¶����в���
extern s16 TempSetVal;    //�¶��趨ֵ
extern s16 TempDisplay;   //�¶���ʾֵ

//�ڲ�������һ
extern ParaTone_STRUCT ParaTempPID;

//�ڲ��������
extern ParaTtwo_STRUCT ParaTempCtrl;

//ʱ�����
extern u16 TimS_HeatPeriod;  //�������ڼ�ʱ
extern u16 TimMs_HeatCounter;//��������ռ�ձȼ�ʱ
extern u8  TimS_EnAT;        //������ʱ���ʱ
extern u8  TimS_SpeakFlash;  //������1s��һ��
/*------------------------------------------------------------------------------
*********************************** �������� ***********************************
------------------------------------------------------------------------------*/
//�¶ȿ���
void HeatControl(void); //���ȿ���������
void TempADToDisp(void);//ad��������ʾֵ
void PIDReset(void);    //δ����PIDʱ���ֳ�ֵ���������⣺��΢�����У����޻����ۻ���

//�¶ȱ���
void TempAlarm(void);

//����������
void AT_pid(void);
void AT_ini(void);    //��������ʼ��

#endif

