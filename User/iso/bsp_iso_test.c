/**
  ******************************************************************************
  * @file    bsp_iso_test.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ISO 整板测试程序 bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
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
 * 系统软件复位
 */
void Soft_Reset(void)
{
  __set_FAULTMASK(1);   /* 关闭所有中断 */  
  NVIC_SystemReset();   /* 系统复位 */
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
 * 初始化液晶，串口，触摸屏，flash及sd卡 * 
 */
void ISO_Init(void)
{
	uint8_t k;
  
  /* 配置USART1 */
  USART1_Config();		
	/* 配置USART2 */
  USART2_Config();
	/* 设置 TIM */
	TIM2_Configuration();

	USART2_DMATxd_Init();
	
	Set_PrintfPort(1);		//设置DebugPf从串口1输出
}




/* ---------------------------------------end of file------------------------------------- */
