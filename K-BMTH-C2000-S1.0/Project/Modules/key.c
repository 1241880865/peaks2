

#include "stm8s.h"
#include "main.h"

#include "key.h"
#include "display.h"
#include "eepromCtrl.h"

#include "heatCtrl.h"
#include "MotorDriver.h"
/*******************************************************************************
                                ---- value ----
*******************************************************************************/
typedef struct
{
    u8 value;     //����ֵ
    u8 last_value;//��һ�ΰ���ֵ
}
KEYSTRUCT;

typedef struct
{
    PARARANGE* p_tab;//��ǰ���в������Ա�
    s16* p_para;     //��ǰ���в����ṹ
    u8  list_num;    //��ǰ���в��������
    u8  index_last;  //��һ�β���������
}
KEYLGSTUCT;

//����
KEYDESTRUCT m_key;//����״̬
u8 g_changing;    //����or��С��־
u8 g_moveBit;     //��λ
static u8 s_lgset_lock;
static KEYSTRUCT  ms_key;
static KEYLGSTUCT ms_lgset;
static s16 sSetBuff[2];

//ʱ�����
u16 TimMs_keyLgset;
u8  TimMs_Jitter;
u8  TimMs_KeyDelayAdd;

//����
static u8   getInputValue(void);
static void resetKeyRun(void);
static void funcIncDec(u8 flag_change);
static void funcSet(void);
static void funcMove(void);
static void funcRun(void);
static void funcKeySmpl(void);
//static void keyMenuSet(void);
static void keyMenuLc(void);
static void keyMenuLgset(void);
static void keyMenuSmplLo(void);
static void keyMenuSmpl(void);
static void setCycleList(PARARANGE* p_dis_tab, s16* p_dis_para, u8 para_num);
static void getNextCycPara(void);
static u8   getNextSglPara(u8 flag_get_buff);
static void saveDataLgset(void);
/*******************************************************************************
** Function name:  scanKey
** Descriptions:   ����ɨ��
** Input:          ��
** Output:         ��
** Return:         ��
*******************************************************************************/
void ScanKey(void)
{
    ms_key.value = getInputValue();
    
    if(ms_key.value == 0)
    {
        resetKeyRun();
        TimMs_Jitter = 0;
    }
    else
    {
        if(TimMs_Jitter < 20) return; //20ms����
        TimMs_Jitter = 0;
        CtrlFlag.Mute = 1;
        
        switch(ms_key.value)
        {
            case 0x80:
              funcSet();     //�趨��
              break;
              
            case 0x08:
              funcRun();     //��ͣ��
              break;
              
            case 0x40:
              funcMove();    //��λ��
              break;
              
            case 0x10:
              funcIncDec(1); //���Ӽ�
              break;
              
            case 0x20:
              funcIncDec(2); //��С��
              break;
              
            case 0x50:
              funcKeySmpl(); //�¶Ȳ�����ϼ�
              break;
              
            default:
              resetKeyRun();
              break;
        }
    }
}

void funcMove(void)
{
    if(ms_key.value == ms_key.last_value)
    {
        if(TimMs_keyLgset >= 3000)
        {
            TimMs_keyLgset = 0;
        }
    }
    else
    {
        ms_key.last_value = ms_key.value;
        TimMs_keyLgset = 0;
        
        g_moveBit++;
        if(g_moveBit >= 4)
        {
            g_moveBit = 0;
        }
    }
}

/*******************************************************************************
** Function name:  getInputValue
** Descriptions:   ��ȡIO�ڼ�ֵ
** Input:          ��
** Output:         ��
** Return:         IO�ڼ�ֵ
*******************************************************************************/
u8 getInputValue(void)
{
    u8 tempa = 0;
    
    tempa = GPIO_ReadInputData(GPIOC);
    tempa = tempa | 0x07;  //��ȡ�˿�C��ֵ ----δ�õ�λ��1
    tempa = tempa ^ 0xFF;  //��ö˿�C��ֵ ----����ֵȡ��
    
    return(tempa);  //���ذ���ֵ
}

/*******************************************************************************
** Function name:  resetKeyRun
** Descriptions:   ��λ������Ϣ
** Input:          ��
** Output:         ��
** Return:         ��
*******************************************************************************/
void resetKeyRun(void)
{
    ms_key.value = 0;
    ms_key.last_value = 0;
    
    s_lgset_lock = 0;
    
    //����or��С
    g_changing = 0;
}

/*******************************************************************************
** Function name:  funcIncDec
** Descriptions:   �������ܺ���������or��С
** Input:          ���Ӽ�С��־
** Output:         ��
** Return:         ��
*******************************************************************************/
void funcIncDec(u8 flag_change)
{
    if(ms_key.value == ms_key.last_value)
    {
        if(TimMs_KeyDelayAdd >= 200)
        {
            TimMs_KeyDelayAdd = 0;
            g_changing = flag_change;
        }
    }
    else
    {
        ms_key.last_value = ms_key.value;
        TimMs_KeyDelayAdd = 0;
        
        g_changing = flag_change;
    }
}

/*******************************************************************************
** Function name:  funcRun
** Descriptions:   ���ȼ�
** Input:          ��
** Output:         ��
** Return:         ��
*******************************************************************************/
void funcRun(void)
{
    if(s_lgset_lock != 0) return;
    
    if(ms_key.value == ms_key.last_value)
    {
        if(m_key.menu == MENU_NONE)
        {
            if((TimMs_keyLgset >= ParaTempCtrl.rut * 1000) && (s_lgset_lock == 0))
            {
                TimMs_keyLgset = 0;
                s_lgset_lock = 1;
                
                if(CtrlFlag.flgRun != 0)
                {
                    CtrlFlag.flgRun = 0;
                    Tflag.EnAT = 0;
                }
                else
                {
                    CtrlFlag.flgRun = 1;
                    Mflag.ErrHall = 0;
                }
                
                PwrSave(); //���籣��
            }
        }
    }
    else
    {
        ms_key.last_value = ms_key.value;
        TimMs_keyLgset = 0;
    }
}

/*******************************************************************************
** Function name:  resetSetKey
** Descriptions:   ��λ�趨��
** Input:          ��
** Output:         ��
** Return:         ��
*******************************************************************************/
void resetSetKey(void)
{
    m_key.password = 0;
    m_key.index = 0;
    
    g_moveBit = 0;
}

/*******************************************************************************
** Function name:  funcSet
** Descriptions:   �趨��
** Input:          ��
** Output:         ��
** Return:         ��
*******************************************************************************/
void funcSet(void)
{
    if(ms_key.value == ms_key.last_value)
    {
        if(TimMs_keyLgset >= 3000)
        {
            TimMs_keyLgset = 0;
            
            if((m_key.menu == MENU_NONE) || (m_key.menu == MENU_SET))//��������趨
            {
                m_key.menu = MENU_LC;
                resetSetKey();
            }
            else if(m_key.menu == MENU_LGSET) //�˳������趨
            {
                m_key.menu = MENU_NONE;
                saveDataLgset(); //������eeprom
            }
        }
    }
    else
    {
        ms_key.last_value = ms_key.value;
        TimMs_keyLgset = 0;
        g_moveBit = 0;
        
        switch(m_key.menu)
        {
            case MENU_NONE:
              {
                  m_key.menu = MENU_SET;
                  sSetBuff[0] = TempSetVal;
                  sSetBuff[1] = SpdSetVal;
                  m_disp.dis_buf = sSetBuff[0];
                  m_key.index = 0;
              }
              break;
              
            case MENU_SET:
              {
                  m_key.index++;
                  if(m_key.index == 1)
                  {
                      sSetBuff[0] = m_disp.dis_buf;
                      m_disp.dis_buf = sSetBuff[1];
                  }
                  else if(m_key.index == 2)
                  {
                      if(TempSetVal != sSetBuff[0])
                      {
                          Tflag.ManlAlm = 1;  //�ֶ��趨ֵ�ı�
                          TempSetVal = sSetBuff[0];
                          WriteTempSet();
                      }
                      if(SpdSetVal != m_disp.dis_buf)
                      {
                          SpdSetVal = m_disp.dis_buf;
                          WriteSpeedSet();
                      }
                      m_key.menu = MENU_NONE;
                  }
              }
              break;
              
            case MENU_LC:
              keyMenuLc();
              break;
              
            case MENU_LO:
              keyMenuSmplLo();
              break;
              
            case MENU_LGSET:
              keyMenuLgset();
              break;
              
            case MENU_SMPL:
              keyMenuSmpl();
              break;
              
            default:
              break;
        }
    }
}

/*******************************************************************************
** Function name:  funcKenSmpl
** Descriptions:   �������ܺ���������
** Input:          ��
** Output:         ��
** Return:         ��
*******************************************************************************/
void funcKeySmpl(void)
{
    if(ms_key.value == ms_key.last_value)
    {
        if(TimMs_keyLgset >= 3000)
        {
            TimMs_keyLgset = 0;
            
            if(m_key.menu == MENU_NONE)//����smpl״̬
            {
                m_key.menu = MENU_LO;
                m_key.password = 0;
                m_key.index = 0;
            }
        }
    }
    else
    {
        ms_key.last_value = ms_key.value;
        TimMs_keyLgset = 0;
        g_moveBit = 0;
    }
}

/*******************************************************************************
** Function name:  keyMenuSmplLo
** Descriptions:   һ���˵���lo��ʾ
** Input:          ��
** Output:         ��
** Return:         ��
*******************************************************************************/
void keyMenuSmplLo(void)
{
    m_key.menu  = MENU_SMPL;
    m_key.menu2 = m_key.password;
    
    switch(m_key.password)
    {
        case SMPL_MENU_TEMP:
          setCycleList(c_tab_smpl, (s16*)&Pt100Std, 2);
          break;
          
        default:
          m_key.menu = MENU_NONE;
          break;
    }
}

/*******************************************************************************
** Function name:  saveDataLgset
** Descriptions:   �˳�lgset����������eeprom
** Input:          ��
** Output:         ��
** Return:         ��
*******************************************************************************/
void saveDataLgset(void)
{
    if(m_key.menu2 == MENU2_PARA_ONE) //eeprom����
    {
        WritePara1ToEE();
    }
    else if(m_key.menu2 == MENU2_PARA_TWO)
    {
        WritePara2ToEE();
    }
    else if(m_key.menu2 == MENU2_PARA_THR)
    {
        if(m_para3.rST != 0)
        {
            m_para3.rST = 0;
            FactoryInit();
        }
    }
}

/*******************************************************************************
** Function name:  keyMenuSmpl
** Descriptions:   ���������˵�����
** Input:          ��
** Output:         ��
** Return:         ��
*******************************************************************************/
void keyMenuSmpl(void)
{
    if(getNextSglPara(0) != 0)
    {
        m_key.menu = MENU_NONE;
        
        //eeprom����
        WriteSmplToEE();
    }
}

/*******************************************************************************
** Function name:  keyMenuLc
** Descriptions:   �������ܺ�����Lc״̬
** Input:          ��
** Output:         ��
** Return:         ��
*******************************************************************************/
void keyMenuLc(void)
{
    m_key.menu  = MENU_LGSET;
    m_key.menu2 = m_key.password;
    
    switch(m_key.password)
    {
        case MENU2_PARA_ONE:
          setCycleList(c_tab1, (s16*)&ParaTempPID, sizeof(ParaTempPID) >> 1);
          break;
          
        case MENU2_PARA_TWO:
          setCycleList(c_tab2, (s16*)&ParaTempCtrl, sizeof(ParaTempCtrl) >> 1);
          break;
          
        case MENU2_PARA_THR:
          setCycleList(c_tab3, (s16*)&m_para3, sizeof(m_para3) >> 1);
          break;
          
        case MENU2_PARA_FOUR:
          if(Tflag.EnTempRun != 0)
          {
              setCycleList(c_tab4, (s16*)&m_para4, sizeof(m_para4) >> 1);
          }
          else
          {
              m_key.menu = MENU_NONE;
              s_lgset_lock = 1;
          }
          break;
          
        case MENU2_PARA_FIVE:
          setCycleList(c_tab5, (s16*)&ParaSpd, sizeof(ParaSpd) >> 1);
          break;
          
        default:
          m_key.menu = MENU_NONE;
          s_lgset_lock = 1;
          break;
    }
}

/*******************************************************************************
** Function name:  setCycleList
** Descriptions:   ����cycleѭ��
** Input:          tab�׵�ַ��para�׵�ַ��para����
** Output:         ��
** Return:         ��
*******************************************************************************/
void setCycleList(PARARANGE* p_dis_tab, s16* p_dis_para, u8 para_num)
{
    /* ��ֵ */
    m_key.index = 0;
    ms_lgset.p_tab = p_dis_tab;
    ms_lgset.p_para = p_dis_para;
    ms_lgset.list_num = para_num;
    
    /* ȡ��ʾֵ */
    m_disp.dis_buf = *ms_lgset.p_para;
    m_disp.p_tab = ms_lgset.p_tab;
}

/*******************************************************************************
** Function name:  getNextCycPara
** Descriptions:   ��ȡcycleѭ������
** Input:          ��
** Output:         ��
** Return:         ��
*******************************************************************************/
void getNextCycPara(void)
{
    /* ������ֵ */
    *(ms_lgset.p_para + m_key.index) = m_disp.dis_buf;
    
    /* ƫ�� */
    m_key.index++;
    if(m_key.index >= ms_lgset.list_num)
    {
        m_key.index = 0;
    }
    
    /* ȡ��ֵ */
    m_disp.dis_buf = *(ms_lgset.p_para + m_key.index);
    m_disp.p_tab = ms_lgset.p_tab + m_key.index;
}

/*******************************************************************************
** Function name:  getNextSglPara
** Descriptions:   ��ȡlistƫ�Ʊ���
** Input:          ��
** Output:         ��
** Return:         ����ѭ��������־
*******************************************************************************/
u8 getNextSglPara(u8 flag_get_buff)
{
    u8 flag_over = 0;
    
    /* ������ֵ */
    *(ms_lgset.p_para + m_key.index) = m_disp.dis_buf;
    
    /* ƫ�� */
    m_key.index++;
    if(m_key.index >= ms_lgset.list_num)
    {
        m_key.index = 0;
        flag_over = 1;
    }
    else/* ȡ��ֵ */
    {
        if(flag_get_buff != 0)
        {
            m_disp.dis_buf = *(ms_lgset.p_para + m_key.index);
        }
        m_disp.p_tab = ms_lgset.p_tab + m_key.index;
    }
    
    return(flag_over);
}

/*******************************************************************************
** Function name:  keyMenuLgset
** Descriptions:   lgset״̬
** Input:          ��
** Output:         ��
** Return:         ��
*******************************************************************************/
void keyMenuLgset(void)
{
    switch(m_key.menu2)
    {
        case MENU2_PARA_ONE:
        case MENU2_PARA_TWO:
        case MENU2_PARA_FIVE:
          getNextCycPara();
          break;
          
        case MENU2_PARA_THR:
        case MENU2_PARA_FOUR:
          if(getNextSglPara(1) != 0)
          {
              if(m_key.menu2 == MENU2_PARA_FOUR)
              {
                  Tflag.EnAT = m_para4.AT;
                  
                  if(Tflag.EnAT != 0)
                  {
                      AT_ini();
                      m_para4.AT = 0;
                  }
              }
              else if(m_key.menu2 == MENU2_PARA_THR)
              {
                  saveDataLgset();
              }
              
              m_key.menu = MENU_NONE;
              s_lgset_lock = 1;
          }
          break;
          
        default:
          break;
    }
}

