

#ifndef MID_FLASH_H
#define MID_FLASH_H



#define   FLS_FIRSTDT_ADD	  (u16 *)0x08017000	//��ʼ�������ݱ�����ʼ��ַ
#define   FLS_UARTDT_ADD	  (u16 *)0x08019000	//�������ݱ�����ʼ��ַ





//����
extern void DataFirstInit(void);
extern void WrToFlash(void);
extern void RdParaFromFlash(void);

#endif
