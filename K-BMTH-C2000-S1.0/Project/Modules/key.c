

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
    u8 value;     //按键值
    u8 last_value;//上一次按键值
}
KEYSTRUCT;

typedef struct
{
    PARARANGE* p_tab;//当前运行参数属性表
    s16* p_para;     //当前运行参数结构
    u8  list_num;    //当前运行参数表个数
    u8  index_last;  //上一次参数表索引
}
KEYLGSTUCT;

//变量
KEYDESTRUCT m_key;//按键状态
u8 g_changing;    //增加or减小标志
u8 g_moveBit;     //移位
static u8 s_lgset_lock;
static KEYSTRUCT  ms_key;
static KEYLGSTUCT ms_lgset;
static s16 sSetBuff[2];

//时间变量
u16 TimMs_keyLgset;
u8  TimMs_Jitter;
u8  TimMs_KeyDelayAdd;

//函数
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
** Descriptions:   按键扫描
** Input:          无
** Output:         无
** Return:         无
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
        if(TimMs_Jitter < 20) return; //20ms消抖
        TimMs_Jitter = 0;
        CtrlFlag.Mute = 1;
        
        switch(ms_key.value)
        {
            case 0x80:
              funcSet();     //设定键
              break;
              
            case 0x08:
              funcRun();     //起停键
              break;
              
            case 0x40:
              funcMove();    //移位键
              break;
              
            case 0x10:
              funcIncDec(1); //增加键
              break;
              
            case 0x20:
              funcIncDec(2); //减小键
              break;
              
            case 0x50:
              funcKeySmpl(); //温度采样组合键
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
** Descriptions:   获取IO口键值
** Input:          无
** Output:         无
** Return:         IO口键值
*******************************************************************************/
u8 getInputValue(void)
{
    u8 tempa = 0;
    
    tempa = GPIO_ReadInputData(GPIOC);
    tempa = tempa | 0x07;  //读取端口C键值 ----未用到位补1
    tempa = tempa ^ 0xFF;  //获得端口C键值 ----捕获值取反
    
    return(tempa);  //返回按键值
}

/*******************************************************************************
** Function name:  resetKeyRun
** Descriptions:   复位按键信息
** Input:          无
** Output:         无
** Return:         无
*******************************************************************************/
void resetKeyRun(void)
{
    ms_key.value = 0;
    ms_key.last_value = 0;
    
    s_lgset_lock = 0;
    
    //增加or减小
    g_changing = 0;
}

/*******************************************************************************
** Function name:  funcIncDec
** Descriptions:   按键功能函数，增加or减小
** Input:          增加减小标志
** Output:         无
** Return:         无
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
** Descriptions:   加热键
** Input:          无
** Output:         无
** Return:         无
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
                
                PwrSave(); //掉电保护
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
** Descriptions:   复位设定键
** Input:          无
** Output:         无
** Return:         无
*******************************************************************************/
void resetSetKey(void)
{
    m_key.password = 0;
    m_key.index = 0;
    
    g_moveBit = 0;
}

/*******************************************************************************
** Function name:  funcSet
** Descriptions:   设定键
** Input:          无
** Output:         无
** Return:         无
*******************************************************************************/
void funcSet(void)
{
    if(ms_key.value == ms_key.last_value)
    {
        if(TimMs_keyLgset >= 3000)
        {
            TimMs_keyLgset = 0;
            
            if((m_key.menu == MENU_NONE) || (m_key.menu == MENU_SET))//进入参数设定
            {
                m_key.menu = MENU_LC;
                resetSetKey();
            }
            else if(m_key.menu == MENU_LGSET) //退出参数设定
            {
                m_key.menu = MENU_NONE;
                saveDataLgset(); //保存至eeprom
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
                          Tflag.ManlAlm = 1;  //手动设定值改变
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
** Descriptions:   按键功能函数，采样
** Input:          无
** Output:         无
** Return:         无
*******************************************************************************/
void funcKeySmpl(void)
{
    if(ms_key.value == ms_key.last_value)
    {
        if(TimMs_keyLgset >= 3000)
        {
            TimMs_keyLgset = 0;
            
            if(m_key.menu == MENU_NONE)//进入smpl状态
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
** Descriptions:   一级菜单，lo显示
** Input:          无
** Output:         无
** Return:         无
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
** Descriptions:   退出lgset，参数保存eeprom
** Input:          无
** Output:         无
** Return:         无
*******************************************************************************/
void saveDataLgset(void)
{
    if(m_key.menu2 == MENU2_PARA_ONE) //eeprom保存
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
** Descriptions:   按键采样菜单处理
** Input:          无
** Output:         无
** Return:         无
*******************************************************************************/
void keyMenuSmpl(void)
{
    if(getNextSglPara(0) != 0)
    {
        m_key.menu = MENU_NONE;
        
        //eeprom保存
        WriteSmplToEE();
    }
}

/*******************************************************************************
** Function name:  keyMenuLc
** Descriptions:   按键功能函数，Lc状态
** Input:          无
** Output:         无
** Return:         无
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
** Descriptions:   配置cycle循环
** Input:          tab首地址；para首地址；para个数
** Output:         无
** Return:         无
*******************************************************************************/
void setCycleList(PARARANGE* p_dis_tab, s16* p_dis_para, u8 para_num)
{
    /* 赋值 */
    m_key.index = 0;
    ms_lgset.p_tab = p_dis_tab;
    ms_lgset.p_para = p_dis_para;
    ms_lgset.list_num = para_num;
    
    /* 取显示值 */
    m_disp.dis_buf = *ms_lgset.p_para;
    m_disp.p_tab = ms_lgset.p_tab;
}

/*******************************************************************************
** Function name:  getNextCycPara
** Descriptions:   获取cycle循环变量
** Input:          无
** Output:         无
** Return:         无
*******************************************************************************/
void getNextCycPara(void)
{
    /* 赋返回值 */
    *(ms_lgset.p_para + m_key.index) = m_disp.dis_buf;
    
    /* 偏移 */
    m_key.index++;
    if(m_key.index >= ms_lgset.list_num)
    {
        m_key.index = 0;
    }
    
    /* 取新值 */
    m_disp.dis_buf = *(ms_lgset.p_para + m_key.index);
    m_disp.p_tab = ms_lgset.p_tab + m_key.index;
}

/*******************************************************************************
** Function name:  getNextSglPara
** Descriptions:   获取list偏移变量
** Input:          无
** Output:         无
** Return:         单次循环结束标志
*******************************************************************************/
u8 getNextSglPara(u8 flag_get_buff)
{
    u8 flag_over = 0;
    
    /* 赋返回值 */
    *(ms_lgset.p_para + m_key.index) = m_disp.dis_buf;
    
    /* 偏移 */
    m_key.index++;
    if(m_key.index >= ms_lgset.list_num)
    {
        m_key.index = 0;
        flag_over = 1;
    }
    else/* 取新值 */
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
** Descriptions:   lgset状态
** Input:          无
** Output:         无
** Return:         无
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

