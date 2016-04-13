
/********************************
1AI+——PE3
1AI-——PE4
1BI+——PE5
1BI-——PE6
2AI+——PB7
2AI-——PB8
2BI+——PB9
2BI-——PE0
3AI+——PC4
3AI-——PC5
3BI+——PB0
3BI-——PB1
4AI+——PA4
4AI-——PA5
4BI+——PA6
4BI-——PA7
**********************************/

#include "STEPMOTOR.h"
#include "bsp_iso_test.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include <string.h>
#include "bsp_usart1.h"


#define uint unsigned int 
#define uchar unsigned char 
	
static void delay(uint16_t t);
static void delayus(uint16_t t);

typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}
bool;

uint i,j,k ;
uint N;//运动次数
uint P_C;//数据指针
uint N_M;//模式数据长度
//B-,B+,A-,A+ 
uchar single_pos[4]={0x01,0x04,0x02,0x08};  /*单四拍正转表*/// 0001 0100 0010 1000
uchar single_rev[4]={0x08,0x02,0x04,0x01};  /*单四拍反转表*/// 1000 0010 0100 0001
uchar double_pos[4]={0x05,0x06,0x0a,0x09};  /*单四拍正转表*/// 0001 0100 0010 1000
uchar double_rev[4]={0x09,0x0a,0x06,0x05};  /*单四拍正转表*/// 0001 0100 0010 1000
uchar eight_pos[8] ={0x01,0x05,0x04,0x06,0x02,0x0a,0x08,0x09};  /*单四拍正转表*/// 0001 0100 0010 1000
uchar eight_rev[8] ={0x09,0x08,0x0a,0x02,0x06,0x04,0x05,0x01};  /*单四拍正转表*/// 0001 0100 0010 1000

uchar pc;//镜片中心点位置
uchar p1,p2,p3,p4;//1-4号马达当前脉冲表状态值
bool dl,dr;//左右眼镜片运行方向，以1/3镜片正向为正向，1为正，0反
uint sl,sr;//左右眼待运行总步数
uint STEP_L,STEP_R;//左右眼已运行步数
bool TIMOK;//定时完成标识，1为完成，定时器用于速度控制
uint v;//运行速度，单位（s/脉冲），最大速度为每个脉冲1秒，取值范围：1-30
bool over_l,over_r;//执行完成标识，1完成
uint x;		//丝杆的绝对坐标位置,靠近电机侧为起点0，最远端为终点



void MOTOR_GPIO_Config(void)
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;
		/*开启PWR的外设时钟*/
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOE, ENABLE); 

//GPIOA		
		/*选择要控制的引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化GPIOA*/			
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
//GPIOB
		/*选择要控制的引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;

		/*调用库函数，初始化GPIOB*/			
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
//GPIOC
		/*选择要控制的引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;

		/*调用库函数，初始化GPIOC*/			
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
//GPIOE
		/*选择要控制的引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;

		/*调用库函数，初始化GPIOE*/			
		GPIO_Init(GPIOE, &GPIO_InitStructure);	
				
}

 
//根据p1,p2,p3,p4值获取状态表位置i，用于确定下一个脉冲值
//输入：px--当前状态值，dx--正反转
uchar getbypx(uchar px,bool dx)
{
	if(dx)//正转
	{
		for(i=0;i<4;i++)
		{
			if(double_pos[i]==px){
				return i;
			}
		}
	}else//反转
	{
		for(i=0;i<4;i++)
		{
			if(double_rev[i]==px){
				return i;
			}
		}
	}
	return 0;
}

void STEPMOTOR_Init()
{
	vu16  capture = 0; 			/* 当前捕获计数值局部变量 */
	extern vu16 CCR1_Val;

	MOTOR_GPIO_Config();
	p1=0x05;		//状态初值
	p2=0x05;		//状态初值
	p3=0x05;		//状态初值
	p4=0x05;		//状态初值
	dl=dr=1;		//方向初值
	STEP_L=STEP_R==0;	//已运行步数初值
	sl=sr=0;			//运行总步数初值
	N_M=0;				//模式数据总数
	P_C=0;				//运动模式数据表指针，用于获取当前要执行的数据
	N=0;					//运动次数赋初值0
	v=1;//设置脉冲速度默认值为1s/个
	
	OPoint();	//校准镜片位置
	LastPoint();//回上次关机时运行的坐标点
//	N_M=getN_M();//获取模式数据长度
//
		GetConfigL();//获取要执行的数据
		CalConfigL();//参数计算：根据配置参数，求出运行方向d,步数s,速度v
		GetConfigR();//获取要执行的数据
		CalConfigR();//参数计算：根据配置参数，求出运行方向d,步数s,速度v
						
		sl=20;
		over_l=0;
		sr=20;
		over_r=0;
//速度节拍定时器初始化	
	CCR1_Val=(uint16_t)(1000*v);
	
	TIMOK=0;	//定时器完成标识初值	
	/* 读出当前计数值 */
	capture = TIM_GetCapture1(TIM2);
	/* 根据当前计数值更新输出捕获寄存器 */
	TIM_SetCompare1(TIM2, capture + CCR1_Val);
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
}
//镜片回原点，校准
void OPoint()
{
	uint8_t i;
	
//	MOTOR_GPIO_Config();

	LED(ON);
	i=0;
	while(GetKey1L() && i<255)
	{
		p1=double_rev[i%4];	//得到脉冲值，赋值到相关引脚，并更新p值			
		Pin1Set(p1);				//输出到引脚，驱动电机
		delay(30);
		PinRSTL();
		delay(150);
		i++;								//指向下一步数据					
	}	
	delay(1000);
	i=0;
	while(i<128)
	{
		p1=double_pos[i%4];	//得到脉冲值，赋值到相关引脚，并更新p值			
		Pin1Set(p1);				//输出到引脚，驱动电机
		delay(30);
		PinRSTL();
		delay(150);
		i++;								//指向下一步数据					
	}	
	LED(OFF);
	delay(1000);
	LED(ON);
	i=0;
	
	while(GetKey2L() && i<255)
	{
		p2=double_rev[i%4];	//得到脉冲值，赋值到相关引脚，并更新p值			
		Pin2Set(p2);				//输出到引脚，驱动电机
		delay(30);
		PinRSTL();
		delay(150);
		i++;								//指向下一步数据					
	}	
	delay(1000);
	i=0;
	while(i<131)
	{
		p2=double_pos[i%4];	//得到脉冲值，赋值到相关引脚，并更新p值			
		Pin2Set(p2);				//输出到引脚，驱动电机
		delay(30);
		PinRSTL();
		delay(150);
		i++;								//指向下一步数据					
	}	

	LED(OFF);
	delay(1000);
	LED(ON);

	i=0;
	while(GetKey1R() && i<255)
	{
		p3=double_rev[i%4];	//得到脉冲值，赋值到相关引脚，并更新p值			
		Pin3Set(p3);				//输出到引脚，驱动电机
		delay(30);
		PinRSTR();					//引脚电平拉高，降低功耗
		delay(150);
		i++;								//指向下一步数据					
	}			

	delay(1000);
	i=0;
	while(i<128)
	{
		p3=double_pos[i%4];	//得到脉冲值，赋值到相关引脚，并更新p值			
		Pin3Set(p3);				//输出到引脚，驱动电机
		delay(30);
		PinRSTR();
		delay(150);
		i++;								//指向下一步数据					
	}	
	
	LED(OFF);
	delay(1000);
	LED(ON);	
	i=0;
	while(GetKey2R() && i<255)
	{
		p4=double_rev[i%4];	//得到脉冲值，赋值到相关引脚，并更新p值			
		Pin4Set(p4);				//输出到引脚，驱动电机
		delay(30);
		PinRSTR();
		delay(150);
		i++;								//指向下一步数据					
	}		
	
	delay(1000);
	i=0;
	while(i<131)
	{
		p4=double_pos[i%4];	//得到脉冲值，赋值到相关引脚，并更新p值			
		Pin4Set(p4);				//输出到引脚，驱动电机
		delay(30);
		PinRSTR();
		delay(150);
		i++;								//指向下一步数据					
	}		
 		
	LED(OFF);

	x=0;
}
//镜片回瞳距坐标点
void EyePoidt()
{
	
}

//镜片回上次关机时运行的坐标点，若无，回瞳距点
void LastPoint()
{
	
}


//获取参数：前进方向，步数，速度
void GetConfigL()
{
	STEP_L=0;
}
//获取参数：前进方向，步数，速度
void GetConfigR()
{
	STEP_R=0;
}

//参数计算：根据配置参数，求出运行方向d,步数s,速度v
void CalConfigL()
{
	
	
}

//参数计算：根据配置参数，求出运行方向d,步数s,速度v
void CalConfigR()
{
	
	
}

//从Flash运动模式数据表中获取运行次数
uint8_t getN(uint8_t x)
{
	uint8_t n;
	n=x+1;
	return n;
}

void STEPMOTOR_Test()
 { 
//		STEPMOTOR_Init();
		N_M=3;
	 	if(P_C<N_M){					
			 
			if(N>0){
				if(over_l && over_r){	////执行完成标志位初值为1，表示完成，将加载新数据
					GetConfigL();//获取要执行的数据
					CalConfigL();//参数计算：根据配置参数，求出运行方向d,步数s,速度v
					GetConfigR();//获取要执行的数据
					CalConfigR();//参数计算：根据配置参数，求出运行方向d,步数s,速度v
									
					sl=20;
					over_l=0;
					sr=20;
					over_r=0;
					
					N--;
				}
				
				RunN(); 					
			
			}else{		//已执行到数据尾
				N=getN(P_C);//获取当前运动数据中的运动次数，若已经执行到数据尾，N返回0
				N=2*N+1;	//将运动次数值乘以2，表示一个周期（运动——返回）为1次
				printf("\r\nP_C=%d\r\n",P_C);
				P_C++;
				dl=1;
				dr=1;
			}						
		}else{
				
			P_C=0;	//数据指针指向开头
			
			while(0)
			{
				delay(8000);
				LED_TOGGLE;
				delay(8000);
				printf("\r\n%s","Test OVER\r\n");
			};
		}
}

/*****************************步进电机运行*************************************/  
//描述：前进s步
//返回：当前脉冲参数p
void RunN()
{
		uchar i;
		uint16_t j;
		uint16_t n;
	
		if(TIMOK)							//定时完成后才运行一次	     
		{  
			TIMOK=0;						//清零
//									
			if(dl){
				i=getbypx(p1,dl);			//获取当前位置对应的数组下标i
				i++;								//指向下一步数据
				p1=double_pos[i%4];	//得到脉冲值，赋值到相关引脚，并更新p值			
				
				i=getbypx(p2,!dl);	//获取当前位置对应的数组下标i
				i++;								//指向下一步数据
				p2=double_rev[i%4];	//得到脉冲值，赋值到相关引脚，并更新p值		
														
			}else if(!dl){
				i=getbypx(p1,dl);			//获取当前位置对应的数组下标i
				i++;								//指向下一步数据
				p1=double_rev[i%4];	//得到脉冲值，赋值到相关引脚，并更新p值			
				
				i=getbypx(p2,!dl);	//获取当前位置对应的数组下标i
				i++;								//指向下一步数据
				p2=double_pos[i%4];	//得到脉冲值，赋值到相关引脚，并更新p值			
						
			}
			
			STEP_L++;
			if(STEP_L<sl){	
				over_l=0;//当前动作未完成				
			}else{
				over_l=1;//当前动作已完成
				dl=!dl;
			}
					
//			
			if(dr){
				i=getbypx(p3,dr);			//获取当前位置对应的数组下标i
				i++;								//指向下一步数据
				p3=double_pos[i%4];	//得到脉冲值，赋值到相关引脚，并更新p值			
				
				i=getbypx(p4,!dr);	//获取当前位置对应的数组下标i
				i++;								//指向下一步数据
				p4=double_rev[i%4];	//得到脉冲值，赋值到相关引脚，并更新p值			
									
			}else if(!dr){
				i=getbypx(p3,dr);			//获取当前位置对应的数组下标i
				i++;								//指向下一步数据
				p3=double_rev[i%4];	//得到脉冲值，赋值到相关引脚，并更新p值			
				
				i=getbypx(p4,!dr);	//获取当前位置对应的数组下标i
				i++;								//指向下一步数据
				p4=double_pos[i%4];	//得到脉冲值，赋值到相关引脚，并更新p值							
				
			}
			
			STEP_R++;
				
			if(STEP_R<sr){				
				over_r=0;//当前动作未完成				
			}else{
				over_r=1;//当前动作已完成
				dr=!dr;
			}
//更新电机位置
				Pin1Set(p1);					//输出到引脚，驱动电机	
				delay(30);						//适当延时，保证转子转动时间，延时过短会丢拍				
				PinRSTL();
				Pin2Set(p2);					//输出到引脚，驱动电机
				delay(30);						//适当延时，保证转子转动时间，延时过短会丢拍				
				PinRSTL();
				Pin3Set(p3);					//输出到引脚，驱动电机
				delay(30);						//适当延时，保证转子转动时间，延时过短会丢拍				
				PinRSTR();
				Pin4Set(p4);					//输出到引脚，驱动电机			
				delay(30);						//适当延时，保证转子转动时间，延时过短会丢拍				
				PinRSTR();
				

//			
//			n=4;
//			j=1;
//			do{
//				Pin1Set(p1);					//输出到引脚，驱动电机			
//				Pin2Set(p2);					//输出到引脚，驱动电机
//				delayus(j);					//适当延时，保证转子转动时间，延时过短会丢拍		
//				
//				PinRSTL();						
//				delayus(200-j);
//	
//				j=j+40;
//		
//			}while(n--);
//						
//			n=4;
//			j=1;
//			do{
//	
//				Pin3Set(p3);					//输出到引脚，驱动电机			
//				Pin4Set(p4);					//输出到引脚，驱动电机
//				delayus(j);					//适当延时，保证转子转动时间，延时过短会丢拍				
//				
//				PinRSTR();				
//				delayus(200-j);
//	
//				j=j+40;
//		
//			}while(n--);
//				
		}
}

//节拍参数输出到引脚
void PinSet(uchar xx)
{
	switch (xx){
		
		case 0x01:
//			PinRST();//
		
			GPIO_SetBits(GPIOE, GPIO_Pin_3);//1A+
			GPIO_SetBits(GPIOB, GPIO_Pin_7);//2A+
			GPIO_SetBits(GPIOC, GPIO_Pin_4);//3A+
			GPIO_SetBits(GPIOA, GPIO_Pin_4);//4A+
			
			break;
		case 0x05:
//			PinRST();//
			
			GPIO_SetBits(GPIOE, GPIO_Pin_3);//1A+
			GPIO_SetBits(GPIOB, GPIO_Pin_7);//2A+
			GPIO_SetBits(GPIOC, GPIO_Pin_4);//3A+
			GPIO_SetBits(GPIOA, GPIO_Pin_4);//4A+
		
			GPIO_SetBits(GPIOE, GPIO_Pin_5);//1B+
			GPIO_SetBits(GPIOB, GPIO_Pin_9);//2B+
			GPIO_SetBits(GPIOB, GPIO_Pin_0);//3B+
			GPIO_SetBits(GPIOA, GPIO_Pin_6);//4B+
			break;
		case 0x04:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_5);//1B+
			GPIO_SetBits(GPIOB, GPIO_Pin_9);//2B+
			GPIO_SetBits(GPIOB, GPIO_Pin_0);//3B+
			GPIO_SetBits(GPIOA, GPIO_Pin_6);//4B+
			break;
		case 0x06:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_5);//1B+
			GPIO_SetBits(GPIOB, GPIO_Pin_9);//2B+
			GPIO_SetBits(GPIOB, GPIO_Pin_0);//3B+
			GPIO_SetBits(GPIOA, GPIO_Pin_6);//4B+
		
			GPIO_SetBits(GPIOE, GPIO_Pin_4);//1A-
			GPIO_SetBits(GPIOB, GPIO_Pin_8);//2A-
			GPIO_SetBits(GPIOC, GPIO_Pin_5);//3A-
			GPIO_SetBits(GPIOA, GPIO_Pin_5);//4A-
			break;
		case 0x02:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_4);//1A-
			GPIO_SetBits(GPIOB, GPIO_Pin_8);//2A-
			GPIO_SetBits(GPIOC, GPIO_Pin_5);//3A-
			GPIO_SetBits(GPIOA, GPIO_Pin_5);//4A-
			break;
		case 0x0a:
//			PinRST();//
		
			GPIO_SetBits(GPIOE, GPIO_Pin_4);//1A-
			GPIO_SetBits(GPIOB, GPIO_Pin_8);//2A-
			GPIO_SetBits(GPIOC, GPIO_Pin_5);//3A-
			GPIO_SetBits(GPIOA, GPIO_Pin_5);//4A-
		
			GPIO_SetBits(GPIOE, GPIO_Pin_6);//1B-
			GPIO_SetBits(GPIOE, GPIO_Pin_0);//2B-
			GPIO_SetBits(GPIOB, GPIO_Pin_1);//3B-
			GPIO_SetBits(GPIOA, GPIO_Pin_7);//4B-
			break;
		case 0x08:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_6);//1B-
			GPIO_SetBits(GPIOE, GPIO_Pin_0);//2B-
			GPIO_SetBits(GPIOB, GPIO_Pin_1);//3B-
			GPIO_SetBits(GPIOA, GPIO_Pin_7);//4B-
			break;
		case 0x09:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_3);//1A+
			GPIO_SetBits(GPIOB, GPIO_Pin_7);//2A+
			GPIO_SetBits(GPIOC, GPIO_Pin_4);//3A+
			GPIO_SetBits(GPIOA, GPIO_Pin_4);//4A+
		
			GPIO_SetBits(GPIOE, GPIO_Pin_6);//1B-
			GPIO_SetBits(GPIOE, GPIO_Pin_0);//2B-
			GPIO_SetBits(GPIOB, GPIO_Pin_1);//3B-
			GPIO_SetBits(GPIOA, GPIO_Pin_7);//4B-
			break;
		default:
			PinRST();//
			break;
	}
}
			
//节拍参数输出到引脚
void Pin1Set(uchar xx)
{
	switch (xx){
		
		case 0x01:
//			PinRST();//
		
			GPIO_SetBits(GPIOE, GPIO_Pin_3);//1A+
			
			break;
		case 0x05:
//			PinRST();//
			
			GPIO_SetBits(GPIOE, GPIO_Pin_3);//1A+		
			GPIO_SetBits(GPIOE, GPIO_Pin_5);//1B+
			break;
		case 0x04:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_5);//1B+
			break;
		case 0x06:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_5);//1B+	
			GPIO_SetBits(GPIOE, GPIO_Pin_4);//1A-
			break;
		case 0x02:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_4);//1A-
			break;
		case 0x0a:
//			PinRST();//
		
			GPIO_SetBits(GPIOE, GPIO_Pin_4);//1A-		
			GPIO_SetBits(GPIOE, GPIO_Pin_6);//1B-
			break;
		case 0x08:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_6);//1B-
			break;
		case 0x09:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_3);//1A+		
			GPIO_SetBits(GPIOE, GPIO_Pin_6);//1B-
			break;
		default:
//			PinRST();//
			break;
	}
}

//节拍参数输出到引脚
void Pin2Set(uchar xx)
{
	switch (xx){
		
		case 0x01:
//			PinRST();//
		
			GPIO_SetBits(GPIOB, GPIO_Pin_7);//2A+
			
			break;
		case 0x05:
//			PinRST();//
			
			GPIO_SetBits(GPIOB, GPIO_Pin_7);//2A+
			GPIO_SetBits(GPIOB, GPIO_Pin_9);//2B+
			break;
		case 0x04:
//			PinRST();//

			GPIO_SetBits(GPIOB, GPIO_Pin_9);//2B+
			break;
		case 0x06:
//			PinRST();//

			GPIO_SetBits(GPIOB, GPIO_Pin_9);//2B+
			GPIO_SetBits(GPIOB, GPIO_Pin_8);//2A-
			break;
		case 0x02:
//			PinRST();//

			GPIO_SetBits(GPIOB, GPIO_Pin_8);//2A-
			break;
		case 0x0a:
//			PinRST();//
		
			GPIO_SetBits(GPIOB, GPIO_Pin_8);//2A-
			GPIO_SetBits(GPIOE, GPIO_Pin_0);//2B-
			break;
		case 0x08:
//			PinRST();//

			GPIO_SetBits(GPIOE, GPIO_Pin_0);//2B-
			break;
		case 0x09:
//			PinRST();//

			GPIO_SetBits(GPIOB, GPIO_Pin_7);//2A+
			GPIO_SetBits(GPIOE, GPIO_Pin_0);//2B-
			break;
		default:
//			PinRST();//
			break;
	}
}


//节拍参数输出到引脚
void Pin3Set(uchar xx)
{
	switch (xx){
		
		case 0x01:
//			PinRST();//
		
			GPIO_SetBits(GPIOC, GPIO_Pin_4);//3A+
			
			break;
		case 0x05:
//			PinRST();//
			
			GPIO_SetBits(GPIOC, GPIO_Pin_4);//3A+
			GPIO_SetBits(GPIOB, GPIO_Pin_0);//3B+
			break;
		case 0x04:
//			PinRST();//

			GPIO_SetBits(GPIOB, GPIO_Pin_0);//3B+
			break;
		case 0x06:
//			PinRST();//

			GPIO_SetBits(GPIOB, GPIO_Pin_0);//3B+
			GPIO_SetBits(GPIOC, GPIO_Pin_5);//3A-
			break;
		case 0x02:
//			PinRST();//

			GPIO_SetBits(GPIOC, GPIO_Pin_5);//3A-
			break;
		case 0x0a:
//			PinRST();//
		
			GPIO_SetBits(GPIOC, GPIO_Pin_5);//3A-
			GPIO_SetBits(GPIOB, GPIO_Pin_1);//3B-
			break;
		case 0x08:
//			PinRST();//

			GPIO_SetBits(GPIOB, GPIO_Pin_1);//3B-
			break;
		case 0x09:
//			PinRST();//

			GPIO_SetBits(GPIOC, GPIO_Pin_4);//3A+
			GPIO_SetBits(GPIOB, GPIO_Pin_1);//3B-
			break;
		default:
//			PinRST();//
			break;
	}
}


//节拍参数输出到引脚
void Pin4Set(uchar xx)
{
	switch (xx){
		
		case 0x01:
//			PinRST();//
		
			GPIO_SetBits(GPIOA, GPIO_Pin_4);//4A+
			
			break;
		case 0x05:
//			PinRST();//
			
			GPIO_SetBits(GPIOA, GPIO_Pin_4);//4A+
			GPIO_SetBits(GPIOA, GPIO_Pin_6);//4B+
			break;
		case 0x04:
//			PinRST();//

			GPIO_SetBits(GPIOA, GPIO_Pin_6);//4B+
			break;
		case 0x06:
//			PinRST();//

			GPIO_SetBits(GPIOA, GPIO_Pin_6);//4B+
			GPIO_SetBits(GPIOA, GPIO_Pin_5);//4A-
			break;
		case 0x02:
//			PinRST();//

			GPIO_SetBits(GPIOA, GPIO_Pin_5);//4A-
			break;
		case 0x0a:
//			PinRST();//
		
			GPIO_SetBits(GPIOA, GPIO_Pin_5);//4A-
			GPIO_SetBits(GPIOA, GPIO_Pin_7);//4B-
			break;
		case 0x08:
//			PinRST();//

			GPIO_SetBits(GPIOA, GPIO_Pin_7);//4B-
			break;
		case 0x09:
//			PinRST();//

			GPIO_SetBits(GPIOA, GPIO_Pin_4);//4A+
			GPIO_SetBits(GPIOA, GPIO_Pin_7);//4B-
			break;
		default:
//			PinRST();//
			break;
	}
}

////引脚电平拉高，降低功耗
//void PinRST()
//{
//		/* 关闭所有*/
//		GPIO_SetBits(GPIOA, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
//		GPIO_SetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
//		GPIO_SetBits(GPIOC, GPIO_Pin_4|GPIO_Pin_5);
//		GPIO_SetBits(GPIOE, GPIO_Pin_0|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);	
//}

//引脚电平拉高，降低功耗
void PinRST(void)
{
		/* 关闭所有*/
		GPIO_ResetBits(GPIOA, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
		GPIO_ResetBits(GPIOB, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
		GPIO_ResetBits(GPIOC, GPIO_Pin_4|GPIO_Pin_5);
		GPIO_ResetBits(GPIOE, GPIO_Pin_0|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);	
}

//引脚电平拉高，降低功耗
void PinRSTL(void)
{
		/* 关闭所有*/
		GPIO_ResetBits(GPIOE, GPIO_Pin_3);//1A+		
		GPIO_ResetBits(GPIOE, GPIO_Pin_5);//1B+
		GPIO_ResetBits(GPIOE, GPIO_Pin_4);//1A-		
		GPIO_ResetBits(GPIOE, GPIO_Pin_6);//1B-	
	
		GPIO_ResetBits(GPIOB, GPIO_Pin_7);//2A+
		GPIO_ResetBits(GPIOB, GPIO_Pin_9);//2B+
		GPIO_ResetBits(GPIOB, GPIO_Pin_8);//2A-
		GPIO_ResetBits(GPIOE, GPIO_Pin_0);//2B-
		
}

//引脚电平拉高，降低功耗
void PinRSTR(void)
{
		/* 关闭所有*/
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);//3A+
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);//3B+
		GPIO_ResetBits(GPIOC, GPIO_Pin_5);//3A-
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);//3B-

		GPIO_ResetBits(GPIOA, GPIO_Pin_4);//4A+
		GPIO_ResetBits(GPIOA, GPIO_Pin_6);//4B+
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);//4A-
		GPIO_ResetBits(GPIOA, GPIO_Pin_7);//4B-
}

void TEST_M()
{	
	uint i,j;
	uchar xxx;
	MOTOR_GPIO_Config();
	PinRST();
	
		for(i=0;i<4;i++)//前进160个脉冲，共10圈
		{
			for(j=0;j<4;j++)
			{

				xxx=double_pos[j];
				PinSet(xxx);
				delay(50);
				PinRST();
				delay(100);
			}
			
		}
		for(i=0;i<4;i++)//后退160个脉冲，共10圈
		{
			for(j=0;j<4;j++)
			{

				xxx=double_rev[j];
				PinSet(xxx);
				delay(50);
				PinRST();
				delay(100);
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

static void delayus(uint16_t t)
{
	uint16_t i;
	while(t--)
	{
		for(i=0;i<200;i++);
	}
}
