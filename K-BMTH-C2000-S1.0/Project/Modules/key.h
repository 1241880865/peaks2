

#ifndef KEY__H
#define KEY__H

//����״̬--menuȡֵ
#define MENU_NONE   0x00
#define MENU_SET    0x01
#define MENU_LGSET  0x02
#define MENU_LC     0x03
#define MENU_LO     0x04
#define MENU_SMPL   0x05

//����״̬--menu2ȡֵ����password��ͬ��
#define MENU2_PARA_NONE 0
#define MENU2_PARA_ONE  3
#define MENU2_PARA_TWO  9
#define MENU2_PARA_THR  567
#define MENU2_PARA_FOUR 27
#define MENU2_PARA_FIVE 103

//����״̬
#define SMPL_MENU_TEMP  11


typedef struct
{
    u8  menu;
    u16 menu2;
    u16 password;
    u8  index;
}
KEYDESTRUCT;

extern KEYDESTRUCT m_key;//����״̬
extern u8 g_changing;    //����or��С��־
extern u8 g_moveBit;     //��λ

//ʱ�����
extern u16 TimMs_keyLgset;
extern u8  TimMs_Jitter;
extern u8  TimMs_KeyDelayAdd;

//����
extern void ScanKey(void);

#endif

