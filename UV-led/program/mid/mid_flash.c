

#include "main.h"
#include "mid_flash.h"

//变量
static u16* const spPara = (u16*)&m_para; //参数表地址
/*******************************************************************************
** Function name:  DataFirstInit
** Descriptions:   首次上电数据初始化
** input:          无
** output:         无
** Return:         无
*******************************************************************************/
void DataFirstInit(void)
{
	if(*((u16*)FLS_FIRSTDT_ADD) != 0xF100) //0xFF00
	{
		/* 解锁 FLASH 控制块 */
		FLASH_Unlock();
		/* 清除一些标志位 */
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		/* 擦除起始地址xx的FLASH 页 */
		FLASH_ErasePage((u32)FLS_FIRSTDT_ADD); //参数初始化地址
		FLASH_ErasePage((u32)FLS_UARTDT_ADD);  //通讯参数保存页首地址
		/* 将0xff00写入 */
		FLASH_ProgramHalfWord((u32)FLS_FIRSTDT_ADD, 0xF100);
		/* 锁定 FLASH 控制块 */
		FLASH_Lock();

		/* ----参数赋值---- */
		m_para.add = 1;
		m_para.duty = 20;
		m_para.order1 = 0;
		m_para.ntcAlm = 50;
		m_para.trigMd = 0;
		m_para.factor = 100;
		
		WrToFlash();
		IWDG_ReloadCounter();
	}
	else
	{
		//读取数据
		RdParaFromFlash();
	}
}
/*******************************************************************************
** Function name:  WrToFlash
** Descriptions:   写入参数表到flash
** input:          无
** output:         无
** Return:         无
*******************************************************************************/
void WrToFlash(void)
{
	u8  i = 0;
	
	/* 解锁 FLASH 控制块*/
	FLASH_Unlock();
	/* 清除一些标志位 */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	/* 擦除起始地址为pp 的 FLASH 页 */
	FLASH_ErasePage((u32)FLS_UARTDT_ADD);
	
	//__set_PRIMASK(1);
	
	for(i = 0; i < PARA_NUM; i++)
	{
		FLASH_ProgramHalfWord((u32)FLS_UARTDT_ADD+i*2, *(spPara+i)); //写入flash
	}
	
	//__set_PRIMASK(0);

	/* 锁定 FLASH 控制块*/
	FLASH_Lock();
}

/*******************************************************************************
** Function name:  RdParaFromFlash
** Descriptions:   从flash中读取参数表
** input:          无
** output:         无
** Return:         无
*******************************************************************************/
void RdParaFromFlash(void)
{
	u8 i;
	
	for(i = 0; i < PARA_NUM; i++)
	{
		*(spPara+i) = *(u16*)((u32)FLS_UARTDT_ADD + i*2);
	}
}
























