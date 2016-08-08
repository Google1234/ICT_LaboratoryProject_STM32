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

//uint8_t	uart3_buff[255];												//delete：not been used
//uint8_t CentL,CentR;//左右眼中心值存储单元				//delete：not been used

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
bool flag_call=0;
unsigned char BUFRX1[255];
unsigned char BUFRX2[255];
unsigned char BUFTX1[255];
unsigned char BUFTX2[255];


unsigned char hd[]="355020151215100";
unsigned char id[]="00000000";
unsigned char tp[]="I";
unsigned char lat[]="0000.0000";
unsigned char lng[]="00000.0000";
unsigned char lat_direction[]="N";
unsigned char lng_direction[]="E";
unsigned char cbc[]="000";
unsigned char end[]="END";


unsigned char ip[]="114.215.99.66";  //实验室阿里云
unsigned char pt[]="4666";
//unsigned char ip[]="120.27.121.236";  //我的阿里云
//unsigned char pt[]="50001";
unsigned char lac_value[]="111E";
unsigned char cell_id[]="AB52";

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
	
	unsigned char send_buff[150];
	unsigned char CMD_buff[50];
	int send_length=0;
	char num_str[10];
//tessst
int times=0;
//	
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
        if(GPSVLD()!='A')
					{
						flag_gps=0;
						GSMDATA();
					}
				else
					{flag_gps=1;
						GPSDATA();//GPS数据提取到lat[],lng[]		
				   }
						
			//TCP SEND
				//不要用sizeof，会造成多一位
        memset(send_buff, 0, sizeof(send_buff));
        send_length=0;
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, hd,15);send_length+=15; //设备号
				strncat(send_buff, "#\0",1);send_length+=1;																													  //用户名
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, "0\0",1);               send_length+=1; //状态位
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, "0000\0",4);            send_length+=4; //密码				
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, "ACT\0",3);             send_length+=3;	 //信息类型		
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, "46007\0",5);           send_length+=5;	 //GSM国家代码		
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, lac_value,4);strncat(send_buff, cell_id,4);send_length+=8;	 //基站信息																								
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, tp,1);               send_length+=1;	   //GPS状态
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff,lng,10);send_length+=10;	                 //GPS 经纬度：116.326400,E,39.990900,N	
        strncat(send_buff, ",\0",1);send_length+=1;strncat(send_buff,lng_direction,1);send_length+=1;
				strncat(send_buff, ",\0",1);send_length+=1;strncat(send_buff,lat,9);send_length+=9;
				strncat(send_buff, ",\0",1);send_length+=1;strncat(send_buff,lat_direction,1);send_length+=1;
				strncat(send_buff, ",\0",1);send_length+=1;strncat(send_buff, "000.00,0\0",8);        send_length+=8;//GPS 速度、方向：006.83,265	
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, "20160318\0",8);        send_length+=8;	 //时间	
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, "092955\0",6);          send_length+=6;	 //时间					
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, "000,0,0,0\0",9);       send_length+=9;	 //运动信息	
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, cbc,sizeof(cbc));       send_length+=3;	 //电量	
				strncat(send_buff, "#\0",1);send_length+=1;	 //结束	
				send_length+=1; //由于  USART2_SendByte(0x0a)算1位
				
				DebugPf(send_buff);printf("%d\n",sizeof(send_buff));

				memset(CMD_buff, 0, sizeof(CMD_buff));
				strcat(CMD_buff, "AT+CIPSEND=\0");
				sprintf(num_str, "%03d\0",send_length);
				strcat(CMD_buff, num_str);
				strcat(CMD_buff, "\n");
				USART2_DMASS(CMD_buff,100,1000);		//
				if(strstr(Ub,">")){					                  
					    USART2_DMAS(send_buff); 
							USART2_SendByte(0x0a);			//算1位		
							USART2_SendByte(0x1A);		  //					
				}
////						USART2_DMAS(id);
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
int process_uart2_dmass_data(int len)
{
	int i=0;
	for(i=0;i<len;i++){
		DebugPf("%c",Ub[i]);
	}
	if(strstr(Ub,"RING")){       //回复电话
		  memset(Ub, 0, sizeof(Ub));//清空避免再次进入循环
      USART2_DMASS("ATL9\n",100,100);//开启扬声器 ，没有效果
			USART2_DMASS("ATA\n",100,100);
      flag_call=1;
	}
	if(strstr(Ub,"Fix")){
      
	}
	if(strstr(Ub,"2D")){

	}
	if(strstr(Ub,"3D")){
	}
//+CREG: <stat>[,<lac>,<ci>] There is a change in the MT network
//registration status or a change of the AT+CREG=<n>
//network cell.	
}
/* ----------------------------------------end of file------------------------------------------------ */

//send sim message 
//		USART2_DMASS("AT+CMGD=1\n",100,100);
//		delay(1000);
//		//USART2_DMASS("AT+CSCS=\"GB2312\"\n",100,100);
//		delay(1000);
//		USART2_DMASS("AT+CNMI=2,1\n",100,100);
//		delay(1000);
//		memset(CMD_buff, 0, sizeof(CMD_buff));
//		strcat(CMD_buff, "AT+CMGS=\"+8617701309542\"\0");  //修改为指定号码
//		strcat(CMD_buff, "\n");
//		DebugPf(CMD_buff);
//		USART2_DMASS(CMD_buff,100,1000);		//
//		delay(1000);
//		if(strstr(Ub,">")){	
//					memset(send_buff, 0, sizeof(send_buff));     //修改为需要内容
//					strcat(send_buff, "message test\0");
//					USART2_DMAS(send_buff);		
//					USART2_SendByte(0x0a);			//算1位		
//					USART2_SendByte(0x1A);		  //		
//		}
	//end
