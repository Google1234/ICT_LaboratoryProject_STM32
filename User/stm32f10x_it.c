/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include <stdio.h>
#include "usb_lib.h"
#include "usb_istr.h"
#include "bsp_led.h"
#include "bsp_adc.h"
#include "sim800l.h"

// 摄像头-火眼ov7725
extern u8 Ov7725_vsync;

// iso 整板测试
volatile uint8_t rec_cmd;
extern void Soft_Reset(void);


extern vu16 CCR1_Val;					/* 声明输出比较通道1计数周期变量*/
extern vu16 CCR2_Val;					/* 声明输出比较通道2计数周期变量*/
extern vu16 CCR3_Val;					/* 声明输出比较通道3计数周期变量*/
extern vu16 CCR4_Val;					/* 声明输出比较通道4计数周期变量*/

extern unsigned char PK_Cnt;	//计时次数

extern bool TIMOK;

extern bool FLAG_BLE_CTS0;//CTS是否被拉低，拉低为1，否则为0
extern bool FLAG_BLE_CTS1;//CTS是否被拉高，拉高为1，否则为0

// 系统定时器
extern void TimingDelay_Decrement(void);

// rtc
extern uint32_t TimeDisplay;

extern uint8_t key_state;

extern bool flag_send;//TCP发送标识
extern bool flag_cbc;	//定时电量检测标识
extern bool flag_chg;//充电标识
extern bool flag_gps;//GPS OK标识

extern unsigned char T_SEND;	//TCP发送间隔，30s
extern unsigned char T_CBC;		//电量检测，10s

#define TM_SEND 20
#define TM_CBC	10

uint16_t cntt=0;

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
		printf("\r\n hardfault");
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	static uint32_t i = 0;
  TimingDelay_Decrement();
	
//	if(i<50000)
//	{
//		i++;
//	}
//	else
//	{
//		if( key_state == 0 )
//		{
//			LED1_TOGGLE;
//			LED2_TOGGLE;
//			LED3_TOGGLE;
//		}
//		i = 0;
//	}

}


// EXTI Line --> PF9 触摸屏中断
void EXTI4_IRQHandler(void)
{ 
  
}

/* ov7725 场中断 服务程序 */
void EXTI0_IRQHandler(void)
{
    if ( EXTI_GetITStatus(EXTI_Line0) != RESET ) 	//检查EXTI_Line0线路上的中断请求是否发送到了NVIC 
    {       
        EXTI_ClearITPendingBit(EXTI_Line0);		    //清除EXTI_Line0线路挂起标志位        
    }    
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)      //分析：这是个GPIO中断，但是我认为此段代码无用，因为相应的GPIO、NVIC、EXTI配置
{
	if(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13))//下降沿
	{
		FLAG_BLE_CTS0=1;//下降沿标志位置1
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13))//上升沿
	{
		if(FLAG_BLE_CTS0)//若出现过下降沿，则上升沿标志位置1，下降沿标志位清零
		{
			FLAG_BLE_CTS1=1;
			FLAG_BLE_CTS0=0;			
		}
	}
	EXTI_ClearFlag(EXTI_Line13);
}

//STEP MOTOR定时器2
void TIM2_IRQHandler(void)
{
	vu16  capture = 0; 			/* 当前捕获计数值局部变量 */

	/* 
	*	TIM2 时钟 = 72 MHz, 分频数 = 7299 + 1, TIM2 counter clock = 10KHz
	*	CC1 更新率 = TIM2 counter clock / CCRx_Val
	*/
		
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
	{
	
//LED			
		LED4_TOGGLE;
		if(flag_gps){
			LED3_ON;;
		}else{
			LED3_OFF;
		}
		if(GetKeyCHG()){
			LED1_ON;
		}else{
			LED1_OFF;
		}
			
//TCP SEND		
		T_SEND--;
		if(T_SEND==0){
			T_SEND=TM_SEND;
			flag_send=1;			
		}
//电量检测			
		T_CBC--;
		if(T_CBC==0){
			T_CBC=TM_CBC;
			flag_cbc=1;
		}		
	
		/* 读出当前计数值 */
		capture = TIM_GetCapture1(TIM2);
		/* 根据当前计数值更新输出捕获寄存器 */
		TIM_SetCompare1(TIM2, capture + CCR1_Val);

		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
	}
	else if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)//用于开关机按键检测
	{
		
		capture = TIM_GetCapture2(TIM2);
		TIM_SetCompare2(TIM2, capture + CCR2_Val);

		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
	}
	else if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
	{

		capture = TIM_GetCapture3(TIM2);
		TIM_SetCompare3(TIM2, capture + CCR3_Val);

		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
	}
	else
	{
		
		capture = TIM_GetCapture4(TIM2);
		TIM_SetCompare4(TIM2, capture + CCR4_Val);

		TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
	}
}

/*
 * 函数名：USB_LP_CAN1_RX0_IRQHandler
 * 描述  ：USB中断和CAN接收中断服务程序，USB跟CAN公用I/O，这里只用到CAN的中断。
 * 输入  ：无
 * 输出  : 无	 
 * 调用  ：无
 */
void USB_LP_CAN1_RX0_IRQHandler(void)
{

  
}

/**
  * @brief  This function handles RTC interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
	  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	  {
	    /* Clear the RTC Second interrupt */
	    RTC_ClearITPendingBit(RTC_IT_SEC);
	
	    /* Enable time update */
	
	    /* Wait until last write operation on RTC registers has finished */
	    RTC_WaitForLastTask();
	  }
}


/*
 * 函数名：SDIO_IRQHandler
 * 描述  ：在SDIO_ITConfig(）这个函数开启了sdio中断	，
 			数据传输结束时产生中断
 * 输入  ：无		 
 * 输出  ：无
 */
void SDIO_IRQHandler(void) 
{
  /* Process All SDIO Interrupt Sources */

}

/*
 * 函数名：USB_LP_CAN1_RX0_IRQHandler
 * 描述  ：USB低优先级中断请求
 * 输入  ：无		 
 * 输出  ：无
 */
#if 0
void USB_LP_CAN1_RX0_IRQHandler(void)
{

}
#endif
/*
 * 函数名：USB_HP_CAN1_TX_IRQHandler
 * 描述  ：USB高优先级中断请求 或发送中断
 * 输入  ：无		 
 * 输出  ：无
 */
void USB_HP_CAN1_TX_IRQHandler(void)
{

}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
