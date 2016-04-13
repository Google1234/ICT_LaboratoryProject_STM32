
#ifndef __STEPMOTOR_H
#define	__STEPMOTOR_H


#include "stm32f10x.h"

void MOTOR_GPIO_Config(void);
void PinSet(unsigned char xx);
void Pin1Set(unsigned char xx);
void Pin2Set(unsigned char xx);
void Pin3Set(unsigned char xx);
void Pin4Set(unsigned char xx);
void PinRST(void);
void PinRSTL(void);
void PinRSTR(void);
void OPoint();
void EyePoidt();
void LastPoint();
void TEST_M();
void STEPMOTOR_Init(void);
void STEPMOTOR_Test(void);
void RunN();
void GetConfigL();
void GetConfigR();
void CalConfigL();
void CalConfigR();
uint8_t getN(uint8_t x);

#endif /* __STEPMOTOR_H */