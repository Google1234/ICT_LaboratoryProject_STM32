

#include "stm32f10x.h"
#include "static_init.h"	//串口结构体

int Sim_ini(void);

unsigned char GPSVLD();
void GPSDATA(void);
void GSMDATA(void);
void SIM_GPIO_Config(void);
uint8_t GetKeyCHG();
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin,uint8_t Down_state);
void USART2_DMAS(u8* Data);
void CBCDATA();
int USART2_DMASS(u8* Data,uint16_t BeTime,uint16_t AfTime);
