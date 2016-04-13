#include "USART1_init.h"

//接收缓冲区容量
#define USART1_RX_BUFF_SIZEMAX 500
#define USART1_TX_BUFF_SIZEMAX 128

//内部用变量
static u8 USART1_GetChar=0;							//接收到的单个字符				中断中使用					
static u8 USART1RxBuffer[USART1_RX_BUFF_SIZEMAX];	//串口接收缓存区 
static u8 USART1TxBuffer[USART1_TX_BUFF_SIZEMAX];	//串口发送缓存区 

static void Delay_MS(u32 nCount_temp);

//串口配置变量
USART1_STRUCT USART1_Real = 
{
	115200,				//波特率
	
	//接收控制队列
	((void*)0),			//开始位置
	((void*)0),			//结束位置
	((void*)0),			//输入位置
	((void*)0),			//输出位置
	((void*)0),			//空间指针
	0,					//空间里数据个数
};

/****************************************************************************
* 名	称：void USART1_GPIO_Init(void)
* 功	能：串口引脚初始化
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/
void USART1_GPIO_Init(void)						//串口引脚初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;		//串口引脚结构
	
	//串口引脚分配时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//配置串口 Tx (PA.09) 为复用推挽输出
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;					//串口发送引脚
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;				//复用推挽输出
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//频率50MHz
  	GPIO_Init(GPIOA, &GPIO_InitStructure);						//初始化引脚
    
	// 配置串口 Rx (PA.10) 为浮空输入
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;					//串口接收引脚
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//浮空输入
  	GPIO_Init(GPIOA, &GPIO_InitStructure);						//初始化引脚
}

/****************************************************************************
* 名	称：void USART1_Init(void)
* 功	能：串口初始化
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/
void USART1_Init(void)
{		
	USART_InitTypeDef UART_InitStructure;		//串口结构

	//串口分配时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//串口初始化
	UART_InitStructure.USART_BaudRate            = USART1_Real.BaudRate;			//波特率
	UART_InitStructure.USART_WordLength          = USART_WordLength_8b;				//数据位8bit
	UART_InitStructure.USART_StopBits            = USART_StopBits_1;				//停止位个数
	UART_InitStructure.USART_Parity              = USART_Parity_No ;				//不进行奇偶效验
	UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//RTS和CTS使能(None不使用)
	UART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;	//发送和接收使能
	USART_Init(USART1, &UART_InitStructure);										//初始化串口
}

/****************************************************************************
* 名	称：void USART1_NVIC_Init(void)
* 功	能：串口中断向量表初始化
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/
void USART1_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 						//中断控制器变量

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//设置中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//主优先级设置
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//设置优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//打开串口中断
	NVIC_Init(&NVIC_InitStructure);								//初始化中断向量表
}

/****************************************************************************
* 名	称：void USART1_DMATxd_Init(void)
* 功	能：串口DMA初始化
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/
void USART1_DMATxd_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 						//中断控制器变量
	DMA_InitTypeDef DMA_InitStructure;							//DMA结构

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);			//使能DMA1时钟

	//DMA中断向量配置
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;	//设置DMA中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//主优先级设置
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//设置优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//打开中断
	NVIC_Init(&NVIC_InitStructure); 

	//DMA配置
	DMA_DeInit(DMA1_Channel4);  		   									//复位DMA1_Channel4通道为默认值
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_BASE + 4;				//DMA通道外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)USART1TxBuffer;				//DMA通道存储器基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						//DMA目的地	(DMA_DIR_PeripheralSRC源)
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//当前外设寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//当前存储寄存器增加
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据宽度为字节(8位)
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//存储器数据宽度为字节(8位)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//正常缓冲模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;					//DMA通道优先级非常高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//DMA通道未配置存储器到存储器传输
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);							//根据上诉设置初始化DMA
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);    						//开启DMA通道中断
}

/****************************************************************************
* 名	称：void USART1_RX_Buffer_init(void)
* 功	能：串口接收初始化
* 入口参数：无
* 出口参数：无
* 说	明：接收中断与接收缓冲区绑定
****************************************************************************/
void USART1_RX_Buffer_init(void)
{
	USART1_Real.QStart = USART1RxBuffer;							//开始位置
	USART1_Real.QEnd = &USART1RxBuffer[USART1_RX_BUFF_SIZEMAX - 1];	//结束位置
	USART1_Real.QIn = USART1_Real.QStart;							//输入位置=开始位置
	USART1_Real.QOut = USART1_Real.QStart;							//输出位置=开始位置
	USART1_Real.QDataBuf = USART1RxBuffer;							//指定接收缓冲区空间
	USART1_Real.QDataCount = 0;										//空间里数据个数
}

/****************************************************************************
* 名	称：void USART1_Config(void)
* 功	能：串口设置
* 入口参数：无
* 出口参数：无
* 说	明：默认为包数据接收					 
****************************************************************************/
void USART1_Config(void)
{
	USART1_Init();				//串口初始化
	USART1_GPIO_Init();			//串口引脚初始化
	USART1_NVIC_Init();			//中断初始化
	USART1_DMATxd_Init();		//DMA发送初始化
	USART1_RX_Buffer_init();	//接收中断与接收缓冲区绑定

	USART_ClearITPendingBit(USART1, USART_IT_RXNE);				//清接收标志
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);				//开启接收中断
	
	USART_Cmd(USART1, ENABLE);  								//使能失能串口外设	
}

/****************************************************************************
* 名	称：void USART1_RX_Buffer_Clear(void)	
* 功	能：清空接收缓冲区
* 入口参数：无
* 出口参数：无
* 说	明：无						
****************************************************************************/
void USART1_RX_Buffer_Clear(void)
{
	USART1_Real.QIn=USART1_Real.QStart;							//输入位置=开始位置
	USART1_Real.QOut=USART1_Real.QStart;						//输出位置=开始位置
	USART1_Real.QDataCount=0;									//空间里数据个数
}

/****************************************************************************
* 名	称：u32 LookUSART1_GetBuffMax(void)	
* 功	能：获取接收缓冲区大小
* 入口参数：无
* 出口参数：无
* 说	明：无			  
****************************************************************************/
u32 LookUSART1_GetBuffMax(void)
{
	return (u32)USART1_RX_BUFF_SIZEMAX;
}

/****************************************************************************
* 名	称：u32 LookUSART1_GetBuffCount(void)
* 功	能：获取缓冲区中接收到的数据个数
* 入口参数：无
* 出口参数：无
* 说	明：无					  
****************************************************************************/
u32 LookUSART1_GetBuffCount(void)
{
	return USART1_Real.QDataCount;
}

/****************************************************************************
* 名	称：u8 USART1_GetByte(u8* Data)	
* 功	能：缓冲区中提取单字符数据
* 入口参数：u8* Data 接收到的数据
* 出口参数：u8	接收成功标志
			0 	接收成功
			1 	接收失败
* 说	明：从接收缓冲区中获取数据，不会持续等待	 
***************************************************************************/
u8 USART1_GetByte(u8* Data)
{
	if(USART1_Real.QDataCount!=0)					//有未处理数据存在
	{
		*Data=*USART1_Real.QOut;					//从队列中取数据
		USART1_Real.QOut++;							//指向下一个未处理数据点
		if(USART1_Real.QOut>USART1_Real.QEnd)		//到了最后一个判断点则跳回开始点
			USART1_Real.QOut=USART1_Real.QStart;	
		USART1_Real.QDataCount--;					//未处理数据个数递减

		return 1;
	}
	return 0;
}

/****************************************************************************
* 名	称：u8 USART1_GetByte_WaitTime(u8* Data,u32 TimeLater)	
* 功	能：u8* Data 接收到的数据
			u32 TimeLater	等待时间 7=1us
* 入口参数：u8	是否有接收到数据
				0 没有接收到数据
				1 接收到数据
* 出口参数：接收到的单字符数据
* 说	明：从接收缓冲区中获取数据，时长等待
***************************************************************************/
u8 USART1_GetByte_WaitTime(u8* Data,u32 TimeLater)
{
	u8 flag=0;					//获取标志
	u32 err=0;					//错误标志
	u8 val=0;					//获取的数据存放点
	while(1)
	{
		flag=USART1_GetByte(&val);
		if (flag==1)
		{
			*Data=val;
			return 1;
		}
		err++;
		if(err>=TimeLater)	//10毫秒都没获取到数据，错误退出
			return 0;
	}
}

/****************************************************************************
* 名	称：u8 USART1_WaitTime_GetString(u8* Data,u32 DataLen,u32 TimeLater)	
* 功	能：在延时之后开始接收指定个数以下的数据
* 入口参数：u8* Data 接收到的数据
			u32 DataLen		希望接收的数据个数
			u32 TimeLater	接收等待时间 7=1us
* 出口参数：u32 接收到数据个数	0没有接收到数据
* 说	明：无
***************************************************************************/
u32 USART1_WaitTime_GetString(u8* Data,u32 DataLen,u32 TimeLater)
{
	u8 G = 0;					//获得的值
	u32 i = 0;					//接收的个数

	Delay_MS(TimeLater);	 	//接收等待

	if(LookUSART1_GetBuffCount() == 0)			//没有接收到数据
		return 0;

	while(LookUSART1_GetBuffCount() != 0)		//查看缓冲区数据接收个数
	{
		if(i >= DataLen - 1)		//超过希望接收的大小
			break;					//退出接收

		if(USART1_GetByte(&G) == 1)		//取值成功
		{
			Data[i] = G;			//数据缓存
			i++;
			//DebugPf("%c",G);		//终端显示
		}
	}
	return i;
}

/****************************************************************************
* 名	称：u8 USART1_GetByte_WaitLong(void)	
* 功	能：无
* 入口参数：u8	接收到的数据
* 出口参数：接收到的单字符数据
* 说	明：从接收缓冲区中获取数据，持续等待
***************************************************************************/
u8 USART1_GetByte_WaitLong(void)
{
	u8 flag=0;					//获取标志
	u8 val=0;					//获取的数据存放点
	while(1)
	{
		flag=USART1_GetByte(&val);
		if (flag==1)
			return val;
	}
}

/****************************************************************************
* 名	称：void USART1_SendByte(u8 Data)
* 功	能：单字符发送
* 入口参数：Data 	发送单字符数据
* 出口参数：无
* 说	明：无				   
****************************************************************************/
void USART1_SendByte(u8 Data)		   //单字符数据输出
{
	USART_SendData(USART1, Data);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/****************************************************************************
* 名	称：void USART1_SendString(u8* Data,u32 Len)
* 功	能：多字符输出
* 入口参数：Data 	输出的单字符数据
			Len		字符个数
* 出口参数：无
* 说	明：无					 
****************************************************************************/
void USART1_SendString(u8* Data,u32 Len)		   //多字符输出
{
	u32 i=0;
	for(i=0;i<Len;i++)
    {    
		USART_SendData(USART1, Data[i]);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
}

/****************************************************************************
* 名	称：void USART1_DMASendString(u8* Data,u32 Len)
* 功	能：DMA方式多字符输出
* 入口参数：Data 	输出的单字符数据
			Len		字符个数
* 出口参数：无
* 说	明：必须在USART1_DMATxd_Init初始化之后才能使用
			DMA无需CPU发送 和 用CPU发送 会有发送冲突	 
****************************************************************************/
void USART1_DMASendString(u8* Data,u32 Len)		   //多字符输出
{
	memcpy(USART1TxBuffer, Data, Len);			   //拷贝数据到发送缓冲区
    DMA1_Channel4->CNDTR = Len;					   //发送字节数量
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); //开启
	DMA_Cmd(DMA1_Channel4, ENABLE);				   //始能DMA通道
}

/****************************************************************************
* 名	称：void USART1_IRQHandler(void)	
* 功	能：中断机制
* 入口参数：无
* 出口参数：无
* 说	明：接收到的数据存入接收缓冲区
	 USART_GetITStatus		检查指定的USART中断发生与否
	 USART_GetFlagStatus	检查指定的USART标志位设置与否
****************************************************************************/
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)				// 串口接收数据触发中断
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);				//清空接收中断标志
		USART1_GetChar = USART_ReceiveData(USART1);					//接收到的字符数据
		
		if(USART1_Real.QDataCount < USART1_RX_BUFF_SIZEMAX)			//如果空间未满,接收保存数据 
		{
			if(USART1_Real.QIn > USART1_Real.QEnd)					//如果有空间可存数据，但输入位置在末尾位置
				USART1_Real.QIn = USART1_Real.QStart;				//将输入位置跳到开始位置
			*USART1_Real.QIn= USART1_GetChar;						//接收到的字符数据存入缓冲区输入位置
			USART1_Real.QIn++;										//输入位置增加
			USART1_Real.QDataCount++;								//空间里数据个数增加
		}
	}

	else if(USART_GetFlagStatus(USART1, USART_IT_ORE) == SET)		//检测是否有接收溢出
    {
	    USART_ReceiveData(USART1);									//清接收溢出标志，只能用读数据的方式来清溢出标志
    }

	else if(USART_GetITStatus(USART1, USART_IT_TXE) == SET)				//串口发送数据触发中断
	{
		;
	}
}

/****************************************************************************
* 名	称：void DMA1_Channel4_IRQHandler(void)	
* 功	能：DMA中断机制
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/
void DMA1_Channel4_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC4)) //如果发送完成
	{
		USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);    //关闭DMA发送
		DMA_Cmd(DMA1_Channel4, DISABLE);	       			//关闭DMA通道  
	}

	DMA_ClearFlag(DMA1_FLAG_GL4| DMA1_FLAG_TC4 | DMA1_FLAG_HT4 | DMA1_FLAG_TE4);  //清除DMA相关标志
}

/****************************************************************************
* 名	称：USART1_STRUCT* USART1GetDCB(void)
* 功	能：获取配置
* 入口参数：无
* 出口参数：配置结构体
* 说	明：无
****************************************************************************/
USART1_STRUCT* USART1GetDCB(void)
{
	return ((USART1_STRUCT*)(&USART1_Real));
}

/*	串口测试
	u8 c[10]="你好";
	u8 l='a';
	u8 v=0;
	u8 f=0;
	u8 len=0;

	DebugPf("开始\r\n");				 //printf测试
	USART1_DMASendString(c,10);		 //DMA发送测试
	USART1_SendString(c,10);		 //多字符发送测试
	USART1_DMASendString(&l,1);		 //DMA发送单字符测试

	f=1;
	while(f)						 //持续接收
	{
		;	
	}
	len = LookUSART1_GetBuffCount(); //查看接收个数测试
	len =0;

	f=USART1_GetByte(&v);			 //取一字节测试
	if(f==1)
		USART1_SendByte(v);
	f=0;

	USART1_RX_Buffer_Clear();		 //清空缓冲区测试

	len=1;
	while(len)
	{
		f=USART1_GetByte_WaitTime(&v,7000000);		 //时长等待测试
		if(f==1)
			USART1_SendByte(v);
		f=0;

		v=USART1_GetByte_WaitLong();				 //一直等待测试
		USART1_SendByte(v);
	}
	
	len = LookUSART1_GetBuffMax();					 //缓冲区大小查看测试
*/

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
