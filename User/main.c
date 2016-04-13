/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   野火 ISO STM32 开发板整板测试
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
#include "bsp_led.h"
#include "bsp_adc.h"
#include "TIMER.h"
#include "FLASH.h"
#include "SIM800L.h"
#include <string.h>
#include "USART2_init.h"

static void delay(uint16_t t);
void GetId();

uint8_t	uart3_buff[255];
uint8_t CentL,CentR;//左右眼中心值存储单元

unsigned char PK_Cnt=0;//开关机按钮按下后，计时次数

typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}
bool;

bool FLAG_BLE_CTS0=0;//CTS是否被拉低，拉低为1，否则为0
bool FLAG_BLE_CTS1=0;//CTS是否被拉高，拉高为1，否则为0
bool FLAG_LONG=0;		//分包长数据接收标识，准备接收为1，无为0

bool flag_send;//TCP发送标识
bool flag_cbc;	//定时电量检测标识
bool flag_chg;//充电标识
bool flag_gps;//GPS OK标识

unsigned char BUFRX1[255];
unsigned char BUFRX2[255];
unsigned char BUFTX1[255];
unsigned char BUFTX2[255];


unsigned char hd[]="TE004:";
unsigned char id[]="00000000";
unsigned char tp[]="0";
unsigned char lat[]="0000.0000";
unsigned char lng[]="00000.0000";
unsigned char cbc[]="000";
unsigned char end[]="END";

unsigned char ip[]="position.iego.net";
unsigned char pt[]="10001";

unsigned char T_SEND=20;	//TCP发送间隔，30s
unsigned char T_CBC=10;		//电量检测，10s


u8 Ub[500];

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{ 
	char *s;
	
	/* 设置系统时钟 */
	RCC_Configuration();
	/* 设置 NVIC */
	NVIC_Configuration();
	
	LED_GPIO_Config();
	SIM_GPIO_Config();
	
	ISO_Init();//初始化串口1/2
//	Flash_ini();//Flash初始化
	GetId();
	LED1(ON);
	LED2(ON);
	LED3(ON);
	LED4(ON);
	delay(5000);
	LED1(OFF);
	LED2(OFF);
	LED3(OFF);
	LED4(OFF);
	delay(20000);
	
	DebugPf("USART2 Receive=%d\r\n",1);
//	printf("%s","1234");
	Sim_ini();
	
	
		flag_send=0;
		flag_cbc=0;
		flag_gps=0;
		flag_cbc=0;
		flag_chg=0;
	
    while(1)
		{
			
			if(flag_send){
				flag_send=0;
				
				USART2_DMASS("AT+CGPSINF=32\n",100,100);//查询GPRMC数据
				
				while(GPSVLD()!='A')
				{
					tp[0]='0';
					flag_gps=0;
					USART2_DMASS("AT+CGPSINF=32\n",100,100);//查询GPRMC数据
					delay(10000);
				}
				flag_gps=1;		
				GPSDATA();			//GPS数据提取到lat[],lng[]
						
			//TCP SEND
				USART2_DMASS("AT+CIPSEND=46\n",100,1000);		//
				if(strstr(Ub,">")){
						
						USART2_DMAS(hd);
						USART2_DMAS(id);
						USART2_DMAS(",");
						USART2_DMAS(tp);
						USART2_DMAS(",");
						USART2_DMAS(lat);
						USART2_DMAS(",");
						USART2_DMAS(lng);
						USART2_DMAS(",");
						USART2_DMAS(cbc);
						USART2_DMAS(",");
						USART2_DMAS(end);
						USART2_DMAS("\r");
						USART2_SendByte(0x0D);		//
						USART2_SendByte(0x1A);		//
					
						DebugPf(hd);
						DebugPf(id);
						DebugPf(",");
						DebugPf(tp);
						DebugPf(",");
						DebugPf(lat);
						DebugPf(",");
						DebugPf(lng);
						DebugPf(",");
						DebugPf(cbc);
						DebugPf(",");
						DebugPf(end);
						DebugPf("\r\n");
				}
					delay(1000);
					USART2_DMASS(NULL,2000,1000);		//
			}
			
			if(flag_cbc){
				flag_cbc=0;
				
				USART2_DMASS("AT+CBC\n",100,100);		//
				
				if(strstr(Ub,"+CBC")){
					CBCDATA();
				}
			}
		}
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
//获取ID
*/
void GetId()
{
	u32 CpuID[1];
	u8 i_d[9];
	u8 i;
	CpuID[0]=*(vu32*)(0x1ffff7e8);
	
	for(i=0;i<4;i++)
	{
		if(((CpuID[0]>>(2*i*4))&0x0f)>0x09){
			id[i*2]=((CpuID[0]>>(2*i*4))&0x0f)+0x37;
		}else{
			id[i*2]=((CpuID[0]>>(2*i*4))&0x0f)+0x30;
		}
		if(((CpuID[0]>>(2*i+1)*4)&0x0f)>0x09){
			id[i*2+1]=((CpuID[0]>>((2*i+1)*4))&0x0f)+0x37;
		}else{
			id[i*2+1]=((CpuID[0]>>((2*i+1)*4))&0x0f)+0x30;
		}
	}

	USART1_SendByte('S');
	USART1_SendByte('N');
	USART1_SendByte(':');

	for(i=0;i<8;i++)
	{
	USART1_SendByte(id[i]);
	}

}
/* ----------------------------------------end of file------------------------------------------------ */

