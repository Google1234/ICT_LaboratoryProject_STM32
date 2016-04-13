
#include "stm32f10x.h"
#include "flash.h"
#include <string.h>
#include <stdio.h>
uint32_t StartAddress=0x0807e800;
uint32_t M1Address = 0x0807e800+0x800;
uint32_t M2Address = 0x0807e800+0x1000;
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;


/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 */
void Flash_ini(void)
{	
	uint8_t i;
	
	Readflash(0);
	Readflash(4);
	Readflash(8);
	Readflash(12);
//	Writeflash();

	if(FLASHStatus == FLASH_COMPLETE)
	{
		for(i=0;i<22;i++)
		{
			Readflash(i);
		}
	}
	
}

void Readflash(char i)
{
	uint8_t data;
	uint8_t *p = (uint8_t*)(M1Address+i);
	data = *p;
	printf("%x\r\n", data);	

}

void Writeflash(uint32_t StartAddr,char *src,uint8_t n)
{
	uint8_t i;
	
/* ���� FLASH ���ƿ�*/
	FLASH_Unlock();
	/* ���һЩ��־λ */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	/* ������ʼ��ַΪ StartAddress �� FLASH ҳ */
  FLASHStatus = FLASH_ErasePage(StartAddr);

	if(FLASHStatus == FLASH_COMPLETE)
	{
		for(i=0;i<n/2;i++)
		{
			USART_SendData(USART1, src[2*i]);
			while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
			USART_SendData(USART1, src[2*i+1]);
			while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
			
			FLASHStatus = FLASH_ProgramHalfWord(StartAddr+i*2, (src[2*i+1]<<8)+src[2*i]);
		}
	}	
	/* ���� FLASH ���ƿ�*/
	FLASH_Lock();
}


/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
