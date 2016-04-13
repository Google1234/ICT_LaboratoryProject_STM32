#ifndef __FLASH_H
#define	__FLASH_H

#include "stm32f10x.h"


void Flash_ini(void);
void Readflash(char i);
//void Writeflash(void);
void Writeflash(uint32_t StartAddr,char *src,uint8_t n);

#endif /* __FLASH_H */
