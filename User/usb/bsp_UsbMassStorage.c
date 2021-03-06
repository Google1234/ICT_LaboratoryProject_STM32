#include "bsp_UsbMassStorage.h"
#include "stm32f10x.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_usart1.h"	
#include "hw_config.h" 
#include "usb_lib.h"
#include "usb_pwr.h"
#include "bsp_led.h"  
#include "app_ui.h"

#ifdef LCD_DISPLAY
	#include "bsp_ili9341_lcd.h"
#endif

void USB_Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

void UsbMassStorage_Test(void)
{
	 /* USART1 config */
	//USART1_Config();
	
  LED_GPIO_Config();
  
	/*初始化SD卡*/
	Set_System();
  	
	/*设置USB时钟为48M*/
	Set_USBClock();
 	
	/*配置USB中断(包括SDIO中断)*/
	USB_Interrupts_Config();
 
	/*USB初始化*/
 	USB_Init();
 
 	while (bDeviceState != CONFIGURED);	 //等待配置完成
	   
	printf("\r\n 野火 ISO STM32 USB MASS STORAGE 实验\r\n");
	#ifdef LCD_DISPLAY
			LCD_Clear(20,195,206,32,BACKGROUND);			//清除屏幕
			LCD_DispEnCh(20,195,"USB 实验测试成功!",BLUE);		
	#endif
  while (1)
  {
    LED_TOGGLE;

    USB_Delay(0x0FFFFF);
  }
}
/* -------------------------------------------end of file------------------------------------------ */
