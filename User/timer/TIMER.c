
#include "TIMER.h"


/* 自定义同义关键字    --------------------------------------------------------*/

/* 自定义参数宏        --------------------------------------------------------*/

/* 自定义函数宏        --------------------------------------------------------*/

/* 自定义全局变量          ----------------------------------------------------*/

vu16 CCR1_Val = 10000;			
vu16 CCR2_Val = 400;			
vu16 CCR3_Val = 10000;			
vu16 CCR4_Val = 1000;			

/* 自定义函数声明      --------------------------------------------------------*/

/******************************************************************************
* 函数名		: RCC_Configuration
* 函数描述  	: 设置系统各部分时钟
* 输入参数  	: 无
* 输出结果  	: 无
* 返回值    	: 无
******************************************************************************/
void RCC_Configuration(void)
{
	/* 定义枚举类型变量 HSEStartUpStatus */
	ErrorStatus HSEStartUpStatus;

  	/* 复位系统时钟设置 */
  	RCC_DeInit();
  	/* 开启 HSE */
  	RCC_HSEConfig(RCC_HSE_ON);
  	/* 等待 HSE 起振并稳定 */
  	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	/* 判断 HSE 起是否振成功，是则进入if()内部 */
  	if(HSEStartUpStatus == SUCCESS)
  	{
    	/* 选择 HCLK（AHB）时钟源为SYSCLK 1分频 */
    	RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    	/* 选择 PCLK2 时钟源为 HCLK（AHB） 1分频 */
    	RCC_PCLK2Config(RCC_HCLK_Div1); 
    	/* 选择 PCLK1 时钟源为 HCLK（AHB） 2分频 */
    	RCC_PCLK1Config(RCC_HCLK_Div2);
			/* 设置FLASH延时周期数为2 */
    	FLASH_SetLatency(FLASH_Latency_2);
    	/* 使能FLASH预取缓存 */
    	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		  /* 选择锁相环（PLL）时钟源为HSE 1分频，倍频数为9，则输出频率为72MHz */
    	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    	/* 使能 PLL */ 
    	RCC_PLLCmd(ENABLE);
    	/* 等待 PLL 输出稳定 */
    	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    	/* 选择 SYSCLK 时钟源为 PLL */
    	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    	/* 等待 PLL 成为 SYSCLK 时钟源 */
    	while(RCC_GetSYSCLKSource() != 0x08);
  	}
  	/* 打开 TIM2 时钟 */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB,ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//禁用JTAG,释放PB3/4

}

/*******************************************************************************
* 函数名  		: TIM2_Configuration
* 函数描述    	: 设置TIM各通道
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/

void TIM2_Configuration(void)
{
	/* 定义 TIM_TimeBase 初始化结构体 TIM_TimeBaseStructure */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	/* 定义 TIM_OCInit 初始化结构体 TIM_OCInitStructure */
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	/* 	100us初值
	*  	计数重载值为65535
	*  	预分频值为(7199 + 1 = 7200)
	*  	时钟分割0
	*  	向上计数模式
	*/
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	
	TIM_TimeBaseInit(TIM2 , &TIM_TimeBaseStructure);
	
	/* 设置预分频值，且立即装入 */
	TIM_PrescalerConfig(TIM2 , 7200-1 , TIM_PSCReloadMode_Immediate);

	/* 	
	*	设置 OC1,OC2,OC3,OC4 通道
	*  	工作模式为计数器模式
	*  	使能比较匹配输出极性
	*  	时钟分割0
	*  	向上计数模式
	*/

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;	
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
		
	TIM_OCInitStructure.TIM_Pulse = CCR2_Val;	
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;	
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
		
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;	
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);

	/* 禁止预装载寄存器 */
	TIM_OC1PreloadConfig(TIM2 , TIM_OCPreload_Disable);
	TIM_OC2PreloadConfig(TIM2 , TIM_OCPreload_Disable);
	TIM_OC3PreloadConfig(TIM2 , TIM_OCPreload_Disable);
	TIM_OC4PreloadConfig(TIM2 , TIM_OCPreload_Disable);
		
	/* 使能 TIM 中断 */
	TIM_ITConfig(TIM2 , TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4 , ENABLE);
	
	/* 启动 TIM 计数 */
	TIM_Cmd(TIM2 , ENABLE);	
}

/*******************************************************************************
* 函数名  		: NVIC_Configuration
* 函数描述    	: 设置NVIC参数
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/

void NVIC_Configuration(void)
{								
	/* 定义 NVIC 初始化结构体 */
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 选择优先级分组0 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	/* 开启 TIM2 中断, 0级先占优先级，0级后占优先级 */	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
