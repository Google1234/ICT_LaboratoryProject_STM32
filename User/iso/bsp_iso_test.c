/**
  ******************************************************************************
  * @file    bsp_iso_test.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ISO ������Գ��� bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "bsp_iso_test.h"
#include "stm32f10x.h"
#include "bsp_led.h" 
#include "bsp_SysTick.h"
#include "TIMER.h"
#include "USART1_init.h"
#include "USART2_init.h"
#include "printf_init.h"


extern volatile uint8_t rec_cmd;

static void delay(uint16_t t);

/*
 * ϵͳ�����λ
 */
void Soft_Reset(void)
{
  __set_FAULTMASK(1);   /* �ر������ж� */  
  NVIC_SystemReset();   /* ϵͳ��λ */
}

static void delay(uint16_t t)
{
	uint16_t i;
	while(t--)
	{
		for(i=0;i<1000;i++);
	}
}

/*
 * ��ʼ��Һ�������ڣ���������flash��sd�� * 
 */
void ISO_Init(void)
{
	uint8_t k;
  
  /* ����USART1 */
  USART1_Config();		
	/* ����USART2 */
  USART2_Config();
	/* ���� TIM */
	TIM2_Configuration();

	USART2_DMATxd_Init();
	
	Set_PrintfPort(1);		//����DebugPf�Ӵ���1���
}




/* ---------------------------------------end of file------------------------------------- */
