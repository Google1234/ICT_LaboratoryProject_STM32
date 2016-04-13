#ifndef __KEY_H
#define	__KEY_H

#include "stm32f10x.h"
 /*******
 *按键按下标置
 KEY_ON 0
 KEY_OFF 1
 ********/
#define KEY_ON	0
#define KEY_OFF	1

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin,uint8_t Down_state);
uint8_t GetKey();
uint8_t GetKey1L();
uint8_t GetKey1R();
uint8_t GetKey2L();
uint8_t GetKey2R();
uint8_t GetKey3L();
uint8_t GetKey3R();
uint8_t GetKey4L();
uint8_t GetKey4R();

#endif /* __LED_H */

