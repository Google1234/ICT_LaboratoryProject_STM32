/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   Ұ�� ISO STM32 �������������
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
#include "bsp_led.h"
#include "bsp_adc.h"
#include "TIMER.h"
#include "FLASH.h"
#include "SIM800L.h"
#include <string.h>
#include "USART2_init.h"

static void delay(uint16_t t);
void GetId();

//uint8_t	uart3_buff[255];												//delete��not been used
//uint8_t CentL,CentR;//����������ֵ�洢��Ԫ				//delete��not been used

unsigned char PK_Cnt=0;//���ػ���ť���º󣬼�ʱ����

typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}
bool;

bool FLAG_BLE_CTS0=0;//CTS�Ƿ����ͣ�����Ϊ1������Ϊ0
bool FLAG_BLE_CTS1=0;//CTS�Ƿ����ߣ�����Ϊ1������Ϊ0
bool FLAG_LONG=0;		//�ְ������ݽ��ձ�ʶ��׼������Ϊ1����Ϊ0

bool flag_send;//TCP���ͱ�ʶ
bool flag_cbc;	//��ʱ��������ʶ
bool flag_chg;//����ʶ
bool flag_gps;//GPS OK��ʶ

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


unsigned char ip[]="114.215.99.66";  //ʵ���Ұ�����
unsigned char pt[]="4666";
//unsigned char ip[]="120.27.121.236";  //�ҵİ�����
//unsigned char pt[]="50001";

unsigned char T_SEND=20;	//TCP���ͼ����30s
unsigned char T_CBC=10;		//������⣬10s


u8 Ub[500];

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
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
	/* ����ϵͳʱ�� */
	RCC_Configuration();
	/* ���� NVIC */
	NVIC_Configuration();
	
	LED_GPIO_Config();
	SIM_GPIO_Config();
	
	ISO_Init();//��ʼ������1/2
//	Flash_ini();//Flash��ʼ��
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
////GSM�õ�λ��
//				USART2_DMASS("AT+CipGSMLOC=1,1\r\n",500,100);
//				GSMDATA();
////		
				flag_send=0;
				USART2_DMASS("AT+CGPSINF=32\n",100,100);//��ѯGPRMC����					
//				while(GPSVLD()!='A'){
//					tp[0]='I';
//					flag_gps=0;
//					USART2_DMASS("AT+CGPSINF=32\n",100,100);//��ѯGPRMC����
//					delay(10000);
//				}
//tesst
times++;
//
				flag_gps=1;		
				GPSDATA();			//GPS������ȡ��lat[],lng[]
						
			//TCP SEND
        memset(send_buff, 0, sizeof(send_buff));
        send_length=0;
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, hd,sizeof(hd));send_length+=sizeof(hd); //�豸��
				strncat(send_buff, "#\0",1);send_length+=1;																													  //�û���
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, "0\0",1);               send_length+=1; //״̬λ
				strncat(send_buff, "#\0",1);send_length+=1;
																									//tesst
																										sprintf(num_str, "%04d\0",times);strncat(send_buff, num_str,4);            send_length+=4; //����
																										//strncat(send_buff, "0000\0",4);            send_length+=4; //����				
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, "ACT\0",3);             send_length+=3;	 //��Ϣ����		
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, "46007\0",5);           send_length+=5;	 //GSM���Ҵ���		
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, "580A4807\0",8);        send_length+=8;	 //��վ��Ϣ			
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, tp,1);               send_length+=1;	 //GPS״̬
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff,lng,10);send_length+=10;	 //GPS ��γ�ȣ�116.326400,E,39.990900,N	
        strncat(send_buff, ",\0",1);send_length+=1;strncat(send_buff,lng_direction,1);send_length+=1;
				strncat(send_buff, ",\0",1);send_length+=1;strncat(send_buff,lat,9);send_length+=9;
				strncat(send_buff, ",\0",1);send_length+=1;strncat(send_buff,lat_direction,1);send_length+=1;
				strncat(send_buff, ",\0",1);send_length+=1;strncat(send_buff, "000.00,0\0",8);        send_length+=8;//GPS �ٶȡ�����006.83,265	
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, "20160318\0",8);        send_length+=8;	 //ʱ��	
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, "092955\0",6);          send_length+=6;	 //ʱ��					
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, "000,0,0,0\0",9);       send_length+=9;	 //�˶���Ϣ	
				strncat(send_buff, "#\0",1);send_length+=1;strncat(send_buff, cbc,sizeof(cbc));       send_length+=sizeof(cbc);	 //����	
				strncat(send_buff, "#\0",1);send_length+=1;	 //����	
				send_length+=1; //����  USART2_SendByte(0x0a)��1λ
				
				memset(CMD_buff, 0, sizeof(CMD_buff));
				strcat(CMD_buff, "AT+CIPSEND=\0");
				sprintf(num_str, "%03d\0",send_length);
				strcat(CMD_buff, num_str);
				strcat(CMD_buff, "\n");
				USART2_DMASS(CMD_buff,100,1000);		//
				if(strstr(Ub,">")){					                  
					    USART2_DMAS(send_buff); 
							USART2_SendByte(0x0a);			//��1λ		
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
//��ȡID
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
	if(strstr(Ub,"Fix")){

	}
	if(strstr(Ub,"2D")){

	}
	if(strstr(Ub,"3D")){
	}
	
}
/* ----------------------------------------end of file------------------------------------------------ */

