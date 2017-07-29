

#ifndef BSP_IN_H
#define BSP_IN_H


#define BITBAND(addr, bitnum)    ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) //查找位段地址数值
#define MEM_ADDR(addr)           *((volatile unsigned long *)(addr)) //强转数值为地址并取地址内容
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))     //入口：外设物理地址，位偏移
//GPIO口物理地址
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 

//输出位段映射
#define A2()        BIT_ADDR(GPIOB_ODR_Addr, 11) //A2
#define A1()        BIT_ADDR(GPIOB_ODR_Addr, 10) //A1
#define A0()        BIT_ADDR(GPIOB_ODR_Addr, 2)  //A0

//时间变量
extern u16 TimMs_switch;
extern u16 TimMs_open1;
extern u16 TimMs_open2;
extern u16 TimMs_open3;
extern u16 TimMs_open4;
extern u16 TimMs_openTab;
extern u16 TimMs_openDly;
extern u16 TimMs_openKeep;

//函数
extern void Input(void);
extern void GetOpenCircuit(void);

#endif
