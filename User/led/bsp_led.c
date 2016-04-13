/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ledӦ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� ISO-MINI STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_led.h"   
#include "bsp_key.h"   
#include "bsp_iso_test.h"

static void delay(uint16_t t);
/**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
void LED_GPIO_Config(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;
		/*����LED������ʱ��*/
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE); 

//PB4,5,6,7--LED
		/*ѡ��Ҫ���Ƶ�GPIOB����--LED*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;	

		/*��������ģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*������������Ϊ50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 

		/*���ÿ⺯������ʼ��GPIOB0*/
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
		/* �ر�����led��	*/
		GPIO_SetBits(GPIOB, GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);
		
		
}



static void delay(uint16_t t)
{
	uint16_t i;
	while(t--)
	{
		for(i=0;i<1000;i++);
	}
}

/*********************************************END OF FILE**********************/
