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
  * @brief  ������
  * @param  ��
  * @retval ��
  */
  
int main(void)
{
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	

	Debug_USART_Config();    

	/* ��ʼ�������� */
	GTP_Init_Panel(); 

	/*��ʼ��Һ����*/
	LCD_Init();
	LCD_LayerInit();
	LTDC_Cmd(ENABLE);

	/*�ѱ�����ˢ��ɫ*/
	LCD_SetLayer(LCD_BACKGROUND_LAYER);  
	LCD_Clear(LCD_COLOR_BLACK);

	/*��ʼ����Ĭ��ʹ��ǰ����*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*Ĭ�����ò�͸��	���ú�������Ϊ��͸���ȣ���Χ 0-0xff ��0Ϊȫ͸����0xffΪ��͸��*/
	LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BLACK);

	/*���û��庯��*/
	Palette_Init();



	
	/* ����������ų�ʼ�� */
	Motor_Config(CurrState.MotorState.MSDSpeed);
	
	/* ��ʼ�������Ķ�ʱ����Ĭ��һ�����ж�1000��*/
	Sample_TIM_Configuration(CurrState.DischargeCount.interruptFrequency);

	SpindleAndCoolant_GPIO_Config();
	while(1)
	{
		Scan_System();	
		//Delay(0xffffff);
	}


}




/*********************************************END OF FILE**********************/

