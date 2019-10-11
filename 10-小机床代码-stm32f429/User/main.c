#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./sdram/bsp_sdram.h"
#include "./lcd/bsp_lcd.h"
#include "./touch/gt9xx.h"
#include "./touch/palette.h"
#include "./system/system.h"
#include "./key/bsp_exti.h"
#include "./motor/MicroStepDriver.h" 

#include "./samplestatus/samplestatus.h"



/*
主要修改的文件
palett.c 绘制图形文件
system.c 系统状态变量
*/

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
	printf("\r\n野火STM3F429 触摸画板测试例程\r\n");

	//	LED_BLUE;
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

	/* LED 端口初始化 */
	LED_GPIO_Config();
  	
	
	/* 步进电机引脚初始化 */
	Motor_Config(CurrState.MotorState.MSDSpeed);
	
	/* 初始化采样的定时器，默认一秒钟中断1000次*/
	Sample_TIM_Configuration(CurrState.DischargeCount.interruptFrequency);

	SpindleAndCoolant_GPIO_Config();
	while(1)
	{
		LCD_DispString_EN_CH(50, 560, "电压：33V");
		Scan_System();	
		//Delay(0xffffff);
	}


}




/*********************************************END OF FILE**********************/

