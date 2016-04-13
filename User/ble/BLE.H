#ifndef __BLE_H
#define	__BLE_H

#include "stm32f10x.h"

/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
#define ON  0
#define OFF 1

/* 带参宏，可以像内联函数一样使用 */
#define BLE(a)	if (a)	\
					GPIO_ResetBits(GPIOA,GPIO_Pin_8);\
					else		\
					GPIO_SetBits(GPIOA,GPIO_Pin_8)



/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)			{p->BSRR=i;}			//设置为高电平		
#define digitalLo(p,i)			{p->BRR=i;}				//输出低电平
#define digitalToggle(p,i)		{p->ODR ^=i;}			//输出反转状态


/* 定义控制IO的宏 */
#define BLE_ON				digitalHi(GPIOA,GPIO_Pin_8)
#define BLE_OFF				digitalLo(GPIOA,GPIO_Pin_8)

#define BEN_ON				digitalLo(GPIOC,GPIO_Pin_2)
#define BEN_OFF				digitalHi(GPIOC,GPIO_Pin_2)

#define RTS3_ON				digitalHi(GPIOB,GPIO_Pin_14)
#define RTS3_OFF			digitalLo(GPIOB,GPIO_Pin_14)


void BLE_GPIO_Config(void);
uint8_t BLE_Init(void);
void BLE_TEST(void);
void Wait_Back(void);
void strclr(char *s);
uint8_t BLE_GetINI(void);
uint8_t Get_M1(void);
uint8_t Get_M2(void);
uint8_t Motor_RUN(void);
uint8_t BLE_Check(void);
uint8_t Type_Get();

#endif /* __LED_H */
