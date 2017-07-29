

#ifndef MID_FLASH_H
#define MID_FLASH_H



#define   FLS_FIRSTDT_ADD	  (u16 *)0x08017000	//初始参数数据保存起始地址
#define   FLS_UARTDT_ADD	  (u16 *)0x08019000	//参数数据保存起始地址





//函数
extern void DataFirstInit(void);
extern void WrToFlash(void);
extern void RdParaFromFlash(void);

#endif
