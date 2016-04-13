/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   按键应用bsp（扫描模式）
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "bsp_key.h" 
#include "bsp_led.h" 

/// 不精确的延时
static void Key_Delay(__IO u32 nCount)
{
	for(; nCount != 0; nCount--);
} 

/**
  * @brief  配置按键用到的I/O口
  * @param  无
  * @retval 无
  */
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*开启按键端口（PE）的时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	/*开启限位开关端口（PB/PD）的时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//禁用JTAG,释放PB3/4
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);
	GPIO_SetBits(GPIOD, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);

}


/**
  * @brief   检测是否有按键按下
  * @param   具体的端口和端口位
  *		@arg GPIOx: x可以是（A...G） 
  *		@arg GPIO_PIN 可以是GPIO_PIN_x（x可以是1...16）
  *   @arg Down_state:按键按下时的电平，1为高电平，0为低电平
  * @retval  按键的状态
  *		@arg KEY_ON:按键按下
  *		@arg KEY_OFF:按键没按下
  */
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin,uint8_t Down_state)
{			
	/*检测是否有按键按下 */
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Down_state ) 
	{	   
		/*延时消抖*/
		Key_Delay(10000);		
		if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Down_state )  
		{	 
			/*等待按键释放 */
//			while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Down_state);   
			return 	KEY_ON;	 
		}
		else
			return KEY_OFF;
	}
	else
		return KEY_OFF;
}
//获取按钮状态
uint8_t GetKey()
{	
	return Key_Scan(GPIOE,GPIO_Pin_1,0);
}

//获取1L状态
uint8_t GetKey1L()
{	
	return Key_Scan(GPIOB,GPIO_Pin_3,0);
}
//获取1R状态
uint8_t GetKey1R()
{	
	return Key_Scan(GPIOB,GPIO_Pin_4,0);
}
//获取2L状态
uint8_t GetKey2L()
{	
	return Key_Scan(GPIOB,GPIO_Pin_6,0);
}
//获取2R状态
uint8_t GetKey2R()
{	
	return Key_Scan(GPIOB,GPIO_Pin_5,0);
}
//获取3L状态
uint8_t GetKey3L()
{	
	return Key_Scan(GPIOD,GPIO_Pin_4,0);
}
//获取3R状态
uint8_t GetKey3R()
{	
	return Key_Scan(GPIOD,GPIO_Pin_5,0);
}
//获取4L状态
uint8_t GetKey4L()
{	
	return Key_Scan(GPIOD,GPIO_Pin_7,0);
}
//获取4R状态
uint8_t GetKey4R()
{	
	return Key_Scan(GPIOD,GPIO_Pin_6,0);
}

/*********************************************END OF FILE**********************/
