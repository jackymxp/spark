#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./key/bsp_exti.h"
#include "./lcd/bsp_lcd.h"
#include "./usart/bsp_debug_usart.h"
#include "./touch/palette.h"
#include "./touch/gt9xx.h"


#include "./system/system.h"
#include "./motor/MicroStepDriver.h" 
#include "./sample_status/sample_status.h"


extern SystemState CurrState;

void Delay(uint32_t ms)
{
	while(--ms);
}
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
  
int main(void)
{
	/* LED 端口初始化 */
	LED_GPIO_Config();	

	Debug_USART_Config();    

	/* 初始化触摸屏 */
	GTP_Init_Panel(); 

	/*初始化液晶屏*/
	LCD_Init();
	LCD_LayerInit();
	LTDC_Cmd(ENABLE);

	/*把背景层刷黑色*/
	LCD_SetLayer(LCD_BACKGROUND_LAYER);  
	LCD_Clear(LCD_COLOR_BLACK);

	/*初始化后默认使用前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
	LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BLACK);

	/*调用画板函数*/
	Palette_Init();



	
	/* 步进电机引脚初始化 */
	Motor_Config(CurrState.MotorState.MSDSpeed);
	
	/* 初始化采样的定时器，默认一秒钟中断1000次*/
	Sample_TIM_Configuration(CurrState.DischargeCount.interruptFrequency);

	SpindleAndCoolant_GPIO_Config();
	while(1)
	{
		Scan_System();	
		//Delay(0xffffff);
	}


}




/*********************************************END OF FILE**********************/

