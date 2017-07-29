

#include "main.h"
#include "mid_flash.h"

//����
static u16* const spPara = (u16*)&m_para; //�������ַ
/*******************************************************************************
** Function name:  DataFirstInit
** Descriptions:   �״��ϵ����ݳ�ʼ��
** input:          ��
** output:         ��
** Return:         ��
*******************************************************************************/
void DataFirstInit(void)
{
	if(*((u16*)FLS_FIRSTDT_ADD) != 0xF100) //0xFF00
	{
		/* ���� FLASH ���ƿ� */
		FLASH_Unlock();
		/* ���һЩ��־λ */
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		/* ������ʼ��ַxx��FLASH ҳ */
		FLASH_ErasePage((u32)FLS_FIRSTDT_ADD); //������ʼ����ַ
		FLASH_ErasePage((u32)FLS_UARTDT_ADD);  //ͨѶ��������ҳ�׵�ַ
		/* ��0xff00д�� */
		FLASH_ProgramHalfWord((u32)FLS_FIRSTDT_ADD, 0xF100);
		/* ���� FLASH ���ƿ� */
		FLASH_Lock();

		/* ----������ֵ---- */
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
		//��ȡ����
		RdParaFromFlash();
	}
}
/*******************************************************************************
** Function name:  WrToFlash
** Descriptions:   д�������flash
** input:          ��
** output:         ��
** Return:         ��
*******************************************************************************/
void WrToFlash(void)
{
	u8  i = 0;
	
	/* ���� FLASH ���ƿ�*/
	FLASH_Unlock();
	/* ���һЩ��־λ */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	/* ������ʼ��ַΪpp �� FLASH ҳ */
	FLASH_ErasePage((u32)FLS_UARTDT_ADD);
	
	//__set_PRIMASK(1);
	
	for(i = 0; i < PARA_NUM; i++)
	{
		FLASH_ProgramHalfWord((u32)FLS_UARTDT_ADD+i*2, *(spPara+i)); //д��flash
	}
	
	//__set_PRIMASK(0);

	/* ���� FLASH ���ƿ�*/
	FLASH_Lock();
}

/*******************************************************************************
** Function name:  RdParaFromFlash
** Descriptions:   ��flash�ж�ȡ������
** input:          ��
** output:         ��
** Return:         ��
*******************************************************************************/
void RdParaFromFlash(void)
{
	u8 i;
	
	for(i = 0; i < PARA_NUM; i++)
	{
		*(spPara+i) = *(u16*)((u32)FLS_UARTDT_ADD + i*2);
	}
}
























