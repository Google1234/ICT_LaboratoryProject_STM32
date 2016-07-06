

#include <stddef.h>
#include <stdlib.h>

#include "USART2_init.h"
#include "stdio.h"
#include "SIM800L.h"

#define KEY_ON	0
#define KEY_OFF	1

extern unsigned char id[];
extern unsigned char tp[];
extern unsigned char lat[];
extern unsigned char lng[];
extern unsigned char lat_direction[];
extern unsigned char lng_direction[];
extern unsigned char cbc[];
extern unsigned char end[];

extern unsigned char ip[];
extern unsigned char pt[];

extern u8 Ub[500];
extern unsigned char lac_value[];
extern unsigned char cell_id[];
extern int process_uart2_dmass_data(int len);

static void Delay_MS(u32 nCount_temp);

int Sim_ini(void)
{

	int i;
	int RecLen=0;
	u8 Ubyte=0;
	
	USART2_DMASS("ATE1\r\n",500,100);	
	Delay_MS(500);
	USART2_DMASS("AT+ECHARGE=1\r\n",500,100);	
	Delay_MS(500);
	USART2_DMASS("AT&W\r\n",500,100);	
	Delay_MS(500);
	USART2_DMASS("AT+IFC=0,0\r\n",500,100);	
	Delay_MS(500);
	USART2_DMASS("AT+CSCLK=0\r\n",500,100);	
	Delay_MS(500);
	USART2_DMASS("AT+CFUN=1\r\n",500,100);
	Delay_MS(500);
	USART2_DMASS("AT+CBC\r\n",500,100);	
	Delay_MS(500);
	USART2_DMASS("AT+CHFA=0\r\n",500,100);
	Delay_MS(500);
	USART2_DMASS("AT+SIDET=0,10\r\n",500,100);
	Delay_MS(500);
	USART2_DMASS("AT+CLVL=90\r\n",500,100);
	Delay_MS(500);
	USART2_DMASS("AT+CMIC=0,15\r\n",500,100);
	Delay_MS(500);
	USART2_DMASS("AT+CPIN?\r\n",500,100);
	Delay_MS(500);
	USART2_DMASS("AT+CSQ\r\n",500,100);
	Delay_MS(500);
	USART2_DMASS("AT+CNMI=1,1,0,0,1\r\n",500,100);
	Delay_MS(500);
	USART2_DMASS("AT+CMGF=1\r\n",500,100);
	Delay_MS(500);
	USART2_DMASS("AT+CFGRI=0\r\n",500,100);
	Delay_MS(500);
	USART2_DMASS("AT+CIPRXGET=1\r\n",500,100);
	Delay_MS(500);
	USART2_DMASS("AT+CCLK?\r\n",500,100);
	Delay_MS(500);


//	USART2_DMASS("AT+FTPTYPE=\"I\"\r\n",2000,2000);
//	USART2_DMASS("AT+FTPCID=1\r\n",2000,2000);
//	USART2_DMASS("AT+FTPSERV=\"116.228.221.52\"\r\n",1000,1000);
//	USART2_DMASS("AT+FTPUN=\"customer\"\r\n",1000,1000);
//	USART2_DMASS("AT+FTPPW=\"111111\"\r\n",1000,1000);
//	USART2_DMASS("AT+FTPGETNAME=\"MTK30.EPO\"\r\n",1000,1000);
//	USART2_DMASS("AT+FTPGETPATH=\"/\"\r\n",1000,1000);
//	USART2_DMASS("AT+FTPGET=1\r\n",10000,10000);
//	USART2_DMASS("AT+FTPGET=2,10\r\n",5000,5000);
//	USART2_DMASS("AT+FTPGET=2,10\r\n",5000,5000);



//	USART2_DMASS("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n",500,1000);
//	USART2_DMASS("AT+SAPBR=3,1,\"APN\",\"CMNET\"\r\n",500,1000);
//	USART2_DMASS("AT+SAPBR=1,1\r\n",500,1000);
//	USART2_DMASS("AT+SAPBR=2,1\r\n",500,1000);
//	USART2_DMASS("AT+HTTPINIT\r\n",500,1000);
//	USART2_DMASS("AT+HTTPPARA=\"CID\",1\r\n",500,1000);
//	USART2_DMASS("AT+HTTPPARA=\"URL\",\"http://wm.sim.com/MTK7.EPO\"\r\n",500,1000);
////	USART2_DMASS("AT+HTTPPARA=\"BREAK\",64500\r\n",500,1000);
//	USART2_DMASS("AT+HTTPACTION=0\r\n",5000,1000);
//	USART2_DMASS("AT+HTTPSTATUS?\r\n",1000,1000);
//	USART2_DMASS("AT+HTTPSTATUS?\r\n",1000,2000);
//	USART2_DMASS("AT+HTTPSTATUS?\r\n",1000,2000);
//	USART2_DMASS("AT+HTTPREAD=0,10\r\n",1000,1000);
//	USART2_DMASS("AT+HTTPREAD=10,10\r\n",1000,1000);
//	
	Delay_MS(10000);
	USART2_DMASS("AT+CGATT=1\r\n",5000,1000);
	Delay_MS(500);
	USART2_DMASS("AT+CGREG?\r\n",5000,100);
	Delay_MS(500);
	USART2_DMASS("AT+CGATT?\r\n",5000,100);
	Delay_MS(500);
	USART2_DMASS("AT+CIPSHUT\r\n",1000,2000);
	Delay_MS(500);
	USART2_DMASS("AT+CSTT=\"CMNET\"\r\n",500,100);
	Delay_MS(500);
	USART2_DMASS("AT+CIICR\r\n",500,1000);
	Delay_MS(500);
	USART2_DMASS("AT+CIFSR\r\n",500,1000);
	Delay_MS(500);
	USART2_DMASS("AT+CIPSTATUS\r\n",500,100);
	Delay_MS(500);
	USART2_DMASS("AT+CIPQSEND=0\r\n",500,100);
	Delay_MS(500);
	USART2_DMASS("AT+CIPSTART=\"TCP\",\"114.215.99.66\",4666\r\n",500,100); //实验室阿里云
	//USART2_DMASS("AT+CIPSTART=\"TCP\",\"120.27.121.236\",50001\r\n",500,100); //我的阿里云
  Delay_MS(500);
	USART2_DMASS(NULL,5000,100);//发送一个NULL，用于读取USART2数据
	Delay_MS(500);
	USART2_DMASS("AT+CGPSPWR=1\r\n",500,100);//开启GPS供电
	Delay_MS(2000);
	USART2_DMASS("AT+CGPSRST=0\r\n",500,100);
	Delay_MS(2000);
	USART2_DMASS("AT+CGPSSTATUS?\r\n",500,100);
	Delay_MS(500);
	USART2_DMASS("AT+CGPSINF=32\r\n",500,100);
	Delay_MS(500);

//
	USART2_DMASS("AT+BTPOWER=1\r\n",500,100);
	Delay_MS(500);
	USART2_DMASS("AT+BTHOST=1\r\n",500,100);
	Delay_MS(500);


	USART2_DMASS("AT+CREG=2\n",100,100);//基站
	Delay_MS(500);

	return 0;
}


/**
  * @brief  ???????I/O?
  * @param  ?
  * @retval ?
  */
void SIM_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);

}


uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin,uint8_t Down_state)
{					   
		if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Down_state )  
		{	 
			return 	KEY_ON;	 
		}
		else{
			return KEY_OFF;
		}
}

uint8_t GetKeyCHG()
{	
	return Key_Scan(GPIOB,GPIO_Pin_8,0);
}



void USART2_DMAS(u8* Data)
{
	while(*Data)
	{
		USART2_SendByte(*Data++);
	}
}

int USART2_DMASS(u8* Data,uint16_t BeTime,uint16_t AfTime)
{

		int i=0;
		int RecLen=0;
		int x,y;
		u8 Ubyte=0;
		
		USART2_RX_Buffer_Clear();
		if(Data!=NULL){
			USART2_DMASendString(Data,strlen(Data));
		}
//等待BeTime	
		do{
			BeTime=BeTime-1;
			Delay_MS(1);
		}while(!(LookUSART2_GetBuffCount()) && BeTime>0);
//检测串口数据	
		do{
			
			AfTime=AfTime-1;
			while(LookUSART2_GetBuffCount()){
				USART2_GetByte(&Ubyte);
				Ub[RecLen]=Ubyte;
				RecLen=RecLen+1;			
			}
		
			Delay_MS(1);
			
		}while(LookUSART2_GetBuffCount() || AfTime>0);
					
		USART2_RX_Buffer_Clear();
			
//处理接收到的数据				
			process_uart2_dmass_data(RecLen);
		return RecLen;	
		
}


/****************************************************************************
* 名	称：void Delay_MS(u32 nCount_temp)
* 功	能：毫秒级
* 入口参数：u32 nCount_temp	延时量
* 出口参数：无
* 说	明：无
****************************************************************************/
void Delay_MS(u32 nCount_temp)
{
	u32 nCount=nCount_temp*8000;
	while(nCount--);
}





unsigned char GPSVLD()
{
	unsigned char *gps;

	gps=strstr(Ub,"+CGPSINF:");

	gps+=24;
		
	return gps[0];
}

void GPSDATA(void)
{
	unsigned char *gps;
	unsigned char i;
  float num=0.0;
	int high=0;
	gps=strstr(Ub,"+CGPSINF:");
	gps+=26;
	for(i=0;i<9;i++)
	{
		lat[i]=gps[i];
	}
	sscanf(lat, "%f", &num);
	high=(int)(num/100);
	num=(num-high*100)/60+high;
	sprintf(lat, "%09f\0",num);
	//DebugPf(lat);printf("%f\n",num);
	lat_direction[0]=gps[10];
	gps+=12;
	for(i=0;i<10;i++)
	{
		lng[i]=gps[i];
	}
	sscanf(lng, "%f", &num);
	high=(int)(num/100);
	num=(num-high*100)/60+high;
	sprintf(lng, "%10f\0",num);
	//DebugPf(lng);printf("%f\n",num);
	lng_direction[0]=gps[11];
	//DebugPf(lng);
	tp[0]='V';
}
void GSMDATA(void)
{
	unsigned char *gsm;
	unsigned char i;
	USART2_DMASS("AT+CREG?\n",100,100);//基站
	Delay_MS(500);
	gsm=strstr(Ub,"+CREG:");
	gsm+=12;
	for(i=0;i<4;i++)
	{
		lac_value[i]=gsm[i];
	}
	gsm+=7;
	for(i=0;i<4;i++)
	{
		cell_id[i]=gsm[i];
	}
	DebugPf(lac_value);DebugPf(cell_id);
}
//获取电量，存入cbc[]
void CBCDATA()
{
	unsigned char *bc;
	unsigned char i;

	bc=strstr(Ub,"+CBC:");

	bc+=8;
	
	i=0;
	do{		
		i++;
	}while(bc[i]!=',');
		
	switch (i){
		case 3:
			cbc[0]=bc[0];
			cbc[1]=bc[1];
			cbc[2]=bc[2];	
			break;
		
		case 2:
			cbc[0]=0x30;
			cbc[1]=bc[0];
			cbc[2]=bc[1];	
			break;
		
		case 1:
			cbc[0]=0x30;
			cbc[1]=0x30;
			cbc[2]=bc[0];	
			break;
		
		default:
			break;
	}
	
//	DebugPf(cbc);

}
