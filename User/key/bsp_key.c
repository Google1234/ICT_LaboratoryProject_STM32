/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����Ӧ��bsp��ɨ��ģʽ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "bsp_key.h" 
#include "bsp_led.h" 

/// ����ȷ����ʱ
static void Key_Delay(__IO u32 nCount)
{
	for(; nCount != 0; nCount--);
} 

/**
  * @brief  ���ð����õ���I/O��
  * @param  ��
  * @retval ��
  */
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*���������˿ڣ�PE����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	/*������λ���ض˿ڣ�PB/PD����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//����JTAG,�ͷ�PB3/4
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);
	GPIO_SetBits(GPIOD, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);

}


/**
  * @brief   ����Ƿ��а�������
  * @param   ����Ķ˿ںͶ˿�λ
  *		@arg GPIOx: x�����ǣ�A...G�� 
  *		@arg GPIO_PIN ������GPIO_PIN_x��x������1...16��
  *   @arg Down_state:��������ʱ�ĵ�ƽ��1Ϊ�ߵ�ƽ��0Ϊ�͵�ƽ
  * @retval  ������״̬
  *		@arg KEY_ON:��������
  *		@arg KEY_OFF:����û����
  */
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin,uint8_t Down_state)
{			
	/*����Ƿ��а������� */
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Down_state ) 
	{	   
		/*��ʱ����*/
		Key_Delay(10000);		
		if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Down_state )  
		{	 
			/*�ȴ������ͷ� */
//			while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Down_state);   
			return 	KEY_ON;	 
		}
		else
			return KEY_OFF;
	}
	else
		return KEY_OFF;
}
//��ȡ��ť״̬
uint8_t GetKey()
{	
	return Key_Scan(GPIOE,GPIO_Pin_1,0);
}

//��ȡ1L״̬
uint8_t GetKey1L()
{	
	return Key_Scan(GPIOB,GPIO_Pin_3,0);
}
//��ȡ1R״̬
uint8_t GetKey1R()
{	
	return Key_Scan(GPIOB,GPIO_Pin_4,0);
}
//��ȡ2L״̬
uint8_t GetKey2L()
{	
	return Key_Scan(GPIOB,GPIO_Pin_6,0);
}
//��ȡ2R״̬
uint8_t GetKey2R()
{	
	return Key_Scan(GPIOB,GPIO_Pin_5,0);
}
//��ȡ3L״̬
uint8_t GetKey3L()
{	
	return Key_Scan(GPIOD,GPIO_Pin_4,0);
}
//��ȡ3R״̬
uint8_t GetKey3R()
{	
	return Key_Scan(GPIOD,GPIO_Pin_5,0);
}
//��ȡ4L״̬
uint8_t GetKey4L()
{	
	return Key_Scan(GPIOD,GPIO_Pin_7,0);
}
//��ȡ4R״̬
uint8_t GetKey4R()
{	
	return Key_Scan(GPIOD,GPIO_Pin_6,0);
}

/*********************************************END OF FILE**********************/
