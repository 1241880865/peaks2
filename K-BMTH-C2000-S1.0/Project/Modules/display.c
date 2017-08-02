

#include "stm8s.h"
#include "main.h"

#include "display.h"
#include "key.h"
#include "heatCtrl.h"
#include "adSample.h"
#include "motorDriver.h"
#include "eepromCtrl.h"

/*------------------------------------------------------------------------------
********************************** 范围数组定义 ********************************
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
********************************** 范围数组定义 ********************************
------------------------------------------------------------------------------*/
//参数表一
PARARANGE c_tab1[8] =
{
    {1,   0, 1000, DF__, DF_A, DF_L, DF_H},  //**** 0
    {0,   1,   60, DF__, DF_T, DF__, DF__},  //**** 1
    {1,   1, 3000, DF__, DF_P, DF__, DF__},
    {0,   1, 2000, DF__, DF_I, DF__, DF__},
    {0,   0, 2000, DF__, DF_d, DF__, DF__},
    {1,-999,  999, DF__, DF_P, DF_b, DF__},  //**** 5
    {0,-999,  999, DF__, DF_P, DF_L, DF__},
    {0,   1,   32, DF__, DF_A, DF_d, DF_r},  //**** 7
};

//参数表二
PARARANGE c_tab2[6] =
{
    {0, 0,    1, DF__, DF_P, DF_o, DF_n},  //**** 0
    {0, 0,   10, DF_r, DF_u, DF_T, DF__},
    {1, 0, 4000, DF__, DF_b, DF_L, DF__},
    {0, 0,  100, DF__, DF_n, DF_P, DF__},
    {1, 0, 4000, DF__, DF_S, DF_P, DF_L},
    {1, 0, 4000, DF__, DF_S, DF_P, DF_H},  //**** 6
};

//参数表三
PARARANGE c_tab3[1] =
{
    {0, 0, 1, DF__, DF_r, DF_S, DF_T},
};

//参数表四
PARARANGE c_tab4[1] =
{
    {0, 0, 1, DF__, DF_A, DF_T, DF__},
};

//参数表五
PARARANGE c_tab5[8] =
{
    {0, 1,  99, DF__, DF_P, DF_d, DF__},  //**** 0
    {0, 1,  99, DF__, DF_I, DF_d, DF__},  //**** 1
    {0, 5,  60, DF__, DF_I, DF_n, DF_T},
    {0, 5,  60, DF__, DF_d, DF_E, DF_T},
    {0,60,6000, DF__, DF_S, DF_d, DF_L},
    {0,60,6000, DF__, DF_S, DF_d, DF_H},  //**** 5
    {0, 1,  32, DF__, DF_P, DF_o, DF_L},
    {0, 0,  99, DF__, DF_d, DF_b, DF__},  //**** 7
};

//参数表采样
PARARANGE c_tab_smpl[2] =
{
    {0, 0, 9999, DF__, DF_T, DF_L, DF__},
    {0, 0, 9999, DF__, DF_T, DF_H, DF__},
};

//变量
DISPLAYBUF  m_disp;
static DISPLAYGNUM ms_disp_num;

//时间变量
u16 TimMs_DelaySetting;
u16 TimMs_FlashDelay;
u16 TimMs_LampFlash;
u16 TimMs_ALMFlash;  //电机故障闪烁
u16 TimMs_ATFlash;   //自整定闪烁

//函数
static void dispNum(DISPLAYGNUM* d, u8 row);
static void displayNone(void);
static void displayLamp(void);
static s16  updataAddOrSubb(s16 data, s16 limit_dn, s16 limit_up);
/*******************************************************************************
** Function name:  resetScreen
** Descriptions:   clear light in screen
** Input:          none
** Output:         none
** Return:         none
*******************************************************************************/
void resetScreen(void)
{
    u8 i;
    
    //清除标识符
    for(i = 0; i < BUFF_NUM; i++)
    {
        *(g_screen_buf + i) = 0;
    }
}

/*******************************************************************************
** Function name:  Display
** Descriptions:   the main function of display
** Input:          none
** Output:         none
** Return:         none
*******************************************************************************/
void displaySet(void)
{
    //一排
    if(m_key.index == 0) //显示温度
    {
        DisplaySglABC(DF__, DF_S, DF_P, DF__, 1); //"SP"
        ms_disp_num.dot = 1;
        m_disp.dis_buf = updataAddOrSubb(m_disp.dis_buf, ParaTempCtrl.spl,\
                                                              ParaTempCtrl.sph);
    }
    else if(m_key.index == 1) //显示速度
    {
        DisplaySglABC(DF__, DF_S, DF_d, DF__, 1); //"Sd"
        ms_disp_num.dot = 0;
        m_disp.dis_buf = updataAddOrSubb(m_disp.dis_buf, ParaSpd.sdl-1, ParaSpd.sdh);
        if(m_disp.dis_buf == (ParaSpd.sdl-1)) //最小值为0
        {
            m_disp.dis_buf = 0;
        }
        else if(m_disp.dis_buf < ParaSpd.sdl)
        {
            if((m_disp.dis_buf == 1) || (m_disp.dis_buf == 10) || (m_disp.dis_buf == 100))
            {
                m_disp.dis_buf = ParaSpd.sdl;
            }
        }
    }
    
    //二排
    ms_disp_num.num = m_disp.dis_buf;
    ms_disp_num.flag_flash = 1;
    ms_disp_num.flashBit = g_moveBit;
    ms_disp_num.flag_hide_0 = 0;
    dispNum(&ms_disp_num, 2);
    
    //标识符
    DisplayLit(DFL_SET);
}

void Display(void)
{
    /* 清屏 */
    resetScreen();
    
    /* 显示内容 */
    if(m_key.menu == MENU_SET)
    {
        displaySet();
    }
    else if(m_key.menu == MENU_LGSET)
    {
        //一排
        DisplayABC(m_disp.p_tab, 1);
        
        //二排
        ms_disp_num.num = m_disp.dis_buf;
        ms_disp_num.dot = m_disp.p_tab->dis_dot;
        ms_disp_num.flag_flash = 1;
        ms_disp_num.flashBit = g_moveBit;
        ms_disp_num.flag_hide_0 = 0;
        dispNum(&ms_disp_num, 2);
        
        m_disp.dis_buf = updataAddOrSubb(m_disp.dis_buf,m_disp.p_tab->limit_dn,\
                                                        m_disp.p_tab->limit_up);
        
        //Lamp
        DisplayLit(DFL_SET);
        if(m_key.menu2 == MENU2_PARA_FOUR)
        {
            DisplayLit(DFL_AT);
        }
    }
    else if((m_key.menu == MENU_LC) || (m_key.menu == MENU_LO))
    {
        //一排
        if(m_key.menu == MENU_LC)
        {
            DisplaySglABC(DF__, DF_L, DF_c, DF__, 1); //"Lc"
        }
        else
        {
            DisplaySglABC(DF__, DF_L, DF_o, DF__, 1); //"Lo"
        }
        
        //二排
        ms_disp_num.num = m_key.password;
        ms_disp_num.dot = 0;
        ms_disp_num.flag_flash = 1;
        ms_disp_num.flashBit = g_moveBit;
        ms_disp_num.flag_hide_0 = 0;
        dispNum(&ms_disp_num, 2);
        
        m_key.password = updataAddOrSubb(m_key.password, 0, 9999);
        
        //Lamp
        DisplayLit(DFL_SET);
    }
    else if(m_key.menu == MENU_SMPL)
    {
        //一排
        DisplayABC(m_disp.p_tab , 1);
        
        //二排
        if(m_key.menu2 == SMPL_MENU_TEMP)
        {
            m_disp.dis_buf = Pt100.SmplVal;
        }
        ms_disp_num.num = m_disp.dis_buf;
        ms_disp_num.dot = m_disp.p_tab->dis_dot;
        ms_disp_num.flag_flash = 0;
        ms_disp_num.flag_hide_0 = 1;
        dispNum(&ms_disp_num, 2);
    }
    else
    {
        displayNone();
    }
    
    /* Lamp（alarm） */
    displayLamp();
    
    /* 发送至屏 */
    SendToScreen();
}

/*******************************************************************************
** Function name:  displayNone
** Descriptions:   无按键时显示
** Input:          none
** Output:         none
** Return:         none
*******************************************************************************/
void displayNone(void)
{
    /* 一排显示 */
    if (Tflag.TempOver != 0)     //超温 "----"
    {
        DisplaySglABC(DF_1, DF_1, DF_1, DF_1, 1);
    }
    else
    {
        ms_disp_num.num = TempDisplay;
        ms_disp_num.dot = 1;
        ms_disp_num.flag_flash = 0;
        ms_disp_num.flag_hide_0 = 1;
        dispNum(&ms_disp_num, 1); //测量值
    }
    
    /* 二排显示 */
    if (Mflag.EnMotorRun != 0)
    {
        ms_disp_num.num = SpdDisplay;
        ms_disp_num.dot = 0;
        ms_disp_num.flag_flash = 0;
        ms_disp_num.flag_hide_0 = 1;
        dispNum(&ms_disp_num, 2); //设定中间值
    }
    else
    {
        DisplaySglABC(DF__, DF_o, DF_F, DF_F, 2);
    }
    
    /* Lamp显示 */
    DisplayLit(DFL_MAIN);
    DisplayLit(DFL_PV);
    DisplayLit(DFL_C1);
    DisplayLit(DFL_SV2);
    DisplayLit(DFL_RPM2);
    if(CtrlFlag.flgRun != 0) //起停
    {
        DisplayLit(DFL_RUN);
    }
    else
    {
        DisplayLit(DFL_STOP);
    }
}

/*******************************************************************************
** Function name:  displayLamp
** Descriptions:   指示灯显示
** Input:          none
** Output:         none
** Return:         none
*******************************************************************************/
void displayLamp(void)
{
    //标识符
    if (Tflag.EnAT != 0)  //自整定 "At"闪烁
    {
        // "Time" 闪烁
        if (TimMs_ATFlash <= 500)
        {
            DisplayLit(DFL_AT);
        }
        else if (TimMs_ATFlash >= 1000)
        {
            TimMs_ATFlash = 0;
        }
    }
    if (Tflag.HeatDis != 0)   //加热有输出，点亮 "HEAT"
    {
        DisplayLit(DFL_HEAT);
    }
    if (Mflag.ErrHall != 0)/* 电机故障 */
    {
        // "ALM!" 闪烁
        if (TimMs_ALMFlash <= 500)
        {
            DisplayLit(DFL_ALM);
        }
        else if (TimMs_ALMFlash >= 1000)
        {
            TimMs_ALMFlash = 0;
        }
    }
    if (Tflag.TempOver != 0)  //溢出 "ALM!"
    {
        DisplayLit(DFL_ALM);
    }
    if ((Tflag.TempAlm != 0) && (Tflag.EnAT == 0))
    {
        DisplayLit(DFL_ALM);  //非自整定状态，超温报警，显示 "ALM!"
    }
}

/*******************************************************************************
** Function name:  dispNum
** Descriptions:   指示灯显示
** Input:          num结构
** Output:         none
** Return:         none
*******************************************************************************/
void dispNum(DISPLAYGNUM* d, u8 row)
{
    if(d->num <= 9999)
    {
        DisplayNum(d, row);
    }
    else
    {
        DisplayHCD(d->num, row);
    }
}

/*******************************************************************************
** Function name:  DisplayPwr
** Descriptions:   display before power on
** Input:          none
** Output:         none
** Return:         none
*******************************************************************************/
void DisplayPwr(void)
{
    //上排显示窗显示"P1-C"
    DisplaySglABC(DF_P, 0xA0, DF_1, DF_C, 1);
    //下排显示窗显示"P1C6"
    DisplaySglABC(DF_P, 0xA0, DF_C, 0x6F, 2);
    
    SendToScreen();  //赋值发送
}

/*******************************************************************************
** Function name:  updataAddOrSubb
** Descriptions:   改变参数值
** Input:          数值，下限，上限
** Output:         none
** Return:         改变后数值
*******************************************************************************/
s16 updataAddOrSubb(s16 data, s16 limit_dn, s16 limit_up)
{
    /* default data s16 limit */
    if(g_changing == 1) //增加
    {
        g_changing = 0;
        
        switch(g_moveBit)
        {
            case 0:
              data++;
              break;
              
            case 1:
              data += 10;
              break;
              
            case 2:
              data += 100;
              break;
              
            case 3:
              data += 1000;
              break;
              
            default:
              break;
        }
        
        if(data >= limit_up)
        {
            data = limit_up;
        }
    }
    else if(g_changing == 2) //减小
    {
        g_changing = 0;
        
        switch(g_moveBit)
        {
            case 0:
              data--;
              break;
              
            case 1:
              data -= 10;
              break;
              
            case 2:
              data -= 100;
              break;
              
            case 3:
              data -= 1000;
              break;
              
            default:
              break;
        }
        if(data <= limit_dn)
        {
            data = limit_dn;
        }
    }
    
    //protect
//    if(data <= limit_dn)
//    {
//        data = limit_dn;
//    }
    
    return(data);
}


/*******************************************************************************
                         -------- end --------
*******************************************************************************/
