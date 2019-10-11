/**
  ******************************************************************************
  * @file    palette.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   触摸画板应用函数
  ******************************************************************************
  * @attention
  *
  */

#include "./touch/palette.h"
#include "./touch/gt9xx.h"
#include "./lcd/bsp_lcd.h"
#include "./display/display.h"
#include "./system/system.h"
#include "./led/bsp_led.h"
#include "./motor/MicroStepDriver.h" 

extern SystemState  CurrState ;

/*按钮结构体数组*/
Touch_Button button[BUTTON_NUM];





void Draw_Spindle_Btn(void* btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;

	if((ptr->touch_num & 0x01)  )
	{
		LCD_SetTextColor(LCD_COLOR_GREEN);
	}
	else
	{
		LCD_SetTextColor(LCD_COLOR_RED);
	}		
	LCD_DrawFullRect(ptr->start_x,ptr->start_y,ptr->end_x - ptr->start_x,ptr->end_y - ptr->start_y);
	int x = (ptr->start_x + ptr->end_x)/2;
	int y = (ptr->start_y + ptr->end_y)/2;
	LCD_DispString_EN_CH(y ,x - 24,"主轴");
}


void Spindle_Command(void* btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;	
	//主轴开
	if((ptr->touch_num & 0x01)  )
	{
		CurrState.SpindleState = ENABLE;
	}
	//主轴停止
	else
	{
		CurrState.SpindleState = DISABLE;
	}
}



void Draw_Coolant_Btn(void* btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;

	if((ptr->touch_num & 0x01)  )
	{
		LCD_SetTextColor(LCD_COLOR_YELLOW);
	}
	else
	{
		LCD_SetTextColor(LCD_COLOR_BLUE);
	}		
	LCD_DrawFullRect(ptr->start_x,ptr->start_y,ptr->end_x - ptr->start_x,ptr->end_y - ptr->start_y);
	int x = (ptr->start_x + ptr->end_x)/2;
	int y = (ptr->start_y + ptr->end_y)/2;
	LCD_DispString_EN_CH(y ,x-24,"水泵");
}







void Coolant_Command(void* btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;
	//水泵开
	if((ptr->touch_num & 0x01) == 1  )
	{
		CurrState.CoolantState = ENABLE;
	}
	//水泵停止
	else
	{
		CurrState.CoolantState = DISABLE;
	}
}




void Draw_StepMotor_Up_Btn(void* btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;
	
	if(ptr->touch_flag)
	{
		LCD_SetTextColor(LCD_COLOR_GREEN);
	}
	else
	{
		LCD_SetTextColor(LCD_COLOR_RED);
	}	
	LCD_DrawFullRect(ptr->start_x,ptr->start_y,ptr->end_x - ptr->start_x,ptr->end_y - ptr->start_y);
	int x = (ptr->start_x + ptr->end_x)/2;
	int y = (ptr->start_y + ptr->end_y)/2;
	LCD_DispString_EN_CH(y ,x - 24,"上升");
}


void StepMotor_Up_Command(void* btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;
	
	//上升按钮被触摸，改变步进电机的驱动方向，并且使能步进电机
	if(ptr->touch_flag)
	{
		//LED_GREEN;
		CurrState.MotorState.MSDState = MSD_RUNNING;
		CurrState.MotorState.MSDdir = MSD_UP;
		CurrState.MotorState.MSDMode = MSD_LASTING;
		CurrState.MotorState.MSDStep = ADJUSTSTEP;//在LASTING模式下，只要配置大于0即可
		//CurrState.MotorState.MSDSpeed = ADJUSTSPEED;
	}
	//上升按钮被触摸，改变步进电机的驱动方向，停止步进电机
	else
	{
		//LED_RED;
		CurrState.MotorState.MSDState = MSD_STOP;
	}	
}

void Draw_StepMotor_Down_Btn(void* btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;
	
	if(ptr->touch_flag)
	{
		LCD_SetTextColor(LCD_COLOR_GREEN);
	}
	else
	{
		LCD_SetTextColor(LCD_COLOR_RED);
	}	
	LCD_DrawFullRect(ptr->start_x,ptr->start_y,ptr->end_x - ptr->start_x,ptr->end_y - ptr->start_y);
	int x = (ptr->start_x + ptr->end_x)/2;
	int y = (ptr->start_y + ptr->end_y)/2;
	LCD_DispString_EN_CH(y ,x - 24,"下降");
}

void StepMotor_Down_Command(void* btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;
	//下降按钮被触摸，改变步进电机的驱动方向，并且使能步进电机
	if(ptr->touch_flag)
	{
		//LED_GREEN;
		CurrState.MotorState.MSDState = MSD_RUNNING;
		CurrState.MotorState.MSDdir = MSD_DOWN;
		CurrState.MotorState.MSDMode = MSD_LASTING;
		CurrState.MotorState.MSDStep = ADJUSTSTEP;//在LASTING模式下，只要配置大于0即可
		//CurrState.MotorState.MSDSpeed = ADJUSTSPEED;
	}
	//下降按钮被触摸，改变步进电机的驱动方向，停止步进电机
	else
	{
		//LED_RED;
		CurrState.MotorState.MSDState = MSD_STOP;
	}
}


void Draw_V_Btn(void* btn)
{
		Touch_Button *ptr = (Touch_Button *)btn;

	LCD_DrawFullRect(ptr->start_x,ptr->start_y,ptr->end_x - ptr->start_x,ptr->end_y - ptr->start_y);
	int x = (ptr->start_x + ptr->end_x) / 2;
	int y = (ptr->start_y + ptr->end_y) / 2;
	if((ptr->touch_num & 0x01)  )
	{
	//	LCD_SetTextColor(LCD_COLOR_GREEN);
	//	LCD_DispString_EN_CH(y ,x - 24, "电压:33V");
	}
	else
	{
	//	LCD_SetTextColor(LCD_COLOR_RED);
		//LCD_DispString_EN_CH(y ,x - 24, "电压:33V");	
	}
}
void V_Command(void* btn)
{}
	
//停止按钮
void Draw_Stop_Btn(void* btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;
	
	LCD_DrawFullRect(ptr->start_x,ptr->start_y,ptr->end_x - ptr->start_x,ptr->end_y - ptr->start_y);
	int x = (ptr->start_x + ptr->end_x) / 2;
	int y = (ptr->start_y + ptr->end_y) / 2;
	if((ptr->touch_num & 0x01)  )
	{
		LCD_SetTextColor(LCD_COLOR_GREEN);
		LCD_DispString_EN_CH(y ,x - 24, "停止");
	}
	else
	{
		LCD_SetTextColor(LCD_COLOR_RED);
		LCD_DispString_EN_CH(y ,x - 24, "开始");	
	}		
}

void Stop_Command(void* btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;
	//开始加工
	if((ptr->touch_num & 0x01)  )
	{
		CurrState.AutoWorking = ENABLE;

		CurrState.MotorState.MSDState = MSD_RUNNING;
		CurrState.MotorState.MSDMode = MSD_PLUSE;
		CurrState.MotorState.MSDStep = OPENSTEP;
		CurrState.MotorState.MSDSpeed = OPENSPEED;
		
	}
	//停止加工
	else
	{
		CurrState.AutoWorking = DISABLE;
		
		CurrState.MotorState.MSDState = MSD_STOP;
		CurrState.MotorState.MSDMode = MSD_PLUSE;
	}	
	
}

/**
* @brief  Touch_Button_Init 初始化按钮参数
* @param  无
* @retval 无
*/
void Touch_Button_Init(void)
{
	/*主轴按钮*/
	button[0].start_x = BUTTON_START_X;
	button[0].start_y = 0;
	button[0].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH ;
	button[0].end_y = button[0].start_y + 1 * COLOR_BLOCK_HEIGHT;
	button[0].touch_num = 0; 
	button[0].touch_flag = 0;  
	button[0].draw_btn = Draw_Spindle_Btn ;//指向颜色按钮所对应的画面
	button[0].btn_command = Spindle_Command ;//指向此按键按下去所要执行的功能
	
	/*水泵按钮*/
	button[1].start_x = BUTTON_START_X;
	button[1].start_y = button[0].start_y + 1 * COLOR_BLOCK_HEIGHT;
	button[1].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH ;
	button[1].end_y = button[0].start_y + 2 * COLOR_BLOCK_HEIGHT;
	button[1].touch_num = 0; 
	button[1].touch_flag = 0;  
	button[1].draw_btn = Draw_Coolant_Btn ;//指向颜色按钮所对应的画面
	button[1].btn_command = Coolant_Command ;//指向此按键按下去所要执行的功能
	
	/*步进电机上升按钮*/
	button[2].start_x = BUTTON_START_X;
	button[2].start_y = button[0].start_y + 2 * COLOR_BLOCK_HEIGHT;
	button[2].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH ;
	button[2].end_y = button[0].start_y + 3 * COLOR_BLOCK_HEIGHT;
	button[2].touch_num = 0; 
	button[2].touch_flag = 0;  
	button[2].draw_btn = Draw_StepMotor_Up_Btn ;//指向颜色按钮所对应的画面
	button[2].btn_command = StepMotor_Up_Command ;//指向此按键按下去所要执行的功能
	
	/*步进电机下降按钮*/
	button[3].start_x = BUTTON_START_X;
	button[3].start_y = button[0].start_y + 3 * COLOR_BLOCK_HEIGHT;
	button[3].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH ;
	button[3].end_y = button[0].start_y + 4 * COLOR_BLOCK_HEIGHT;
	button[3].touch_num = 0; 
	button[3].touch_flag = 0;  
	button[3].draw_btn = Draw_StepMotor_Down_Btn ;//指向颜色按钮所对应的画面
	button[3].btn_command = StepMotor_Down_Command ;//指向此按键按下去所要执行的功能
	
	
	/*停止按钮*/
	button[4].start_x = BUTTON_START_X;
	button[4].start_y = button[0].start_y + 4 * COLOR_BLOCK_HEIGHT;
	button[4].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH ;
	button[4].end_y = button[0].start_y + 5 * COLOR_BLOCK_HEIGHT;
	button[4].touch_num = 0; 
	button[4].touch_flag = 0;  
	button[4].draw_btn = Draw_Stop_Btn ;//指向颜色按钮所对应的画面
	button[4].btn_command = Stop_Command ;//指向此按键按下去所要执行的功能
	
		/*电压按钮*/
	button[5].start_x = PALETTE_END_X - 1.5 * COLOR_BLOCK_WIDTH;
	button[5].start_y = 0;
	button[5].end_x = PALETTE_END_X;
	button[5].end_y = COLOR_BLOCK_HEIGHT;
	button[5].touch_num = 0; 
	button[5].touch_flag = 0;  
	button[5].draw_btn = Draw_V_Btn ;//指向颜色按钮所对应的画面
	button[5].btn_command = V_Command ;//指向此按键按下去所要执行的功能
	
	
}




/**********************以下的函数不需要做任何的改动了***********************************/

/**
* @brief  Palette_Init 画板初始化
* @param  无
* @retval 无
*/
void Palette_Init(void)
{
	uint8_t i;
	/* 整屏清为白色 */
	LCD_Clear(CL_WHITE);	/* 清屏，显示全白 */ 
	/* 初始化按钮 */
	Touch_Button_Init();
	/* 描绘按钮 */
	for(i=0;i<BUTTON_NUM;i++)
	{
		button[i].draw_btn(&button[i]);
	}	
}
/**
* @brief  Touch_Button_Down 按键被按下时调用的函数，由触摸屏调用
* @param  x 触摸位置的x坐标
* @param  y 触摸位置的y坐标
* @retval 无
*/
void Touch_Button_Down(uint16_t x,uint16_t y)
{
	for(uint8_t i=0;i<BUTTON_NUM;i++)
	{
		/* 触摸到了按钮 */
		if(x<=button[i].end_x && y<=button[i].end_y && y>=button[i].start_y && x>=button[i].start_x )
		{
			if(button[i].touch_flag == 0)     /*原本的状态为没有按下，则更新状态*/
			{
				//printf("按键按下\n");
				button[i].touch_num++;
				button[i].touch_flag = 1;         /* 记录按下标志 */ 
				button[i].draw_btn(&button[i]);  /*重绘按钮*/
				button[i].btn_command(&button[i]);  /*执行按键的功能命令*/
			}        
		}
	}
}

/**
* @brief  Touch_Button_Up 按键被释放时调用的函数，由触摸屏调用
* @param  x 触摸最后释放时的x坐标
* @param  y 触摸最后释放时的y坐标
* @retval 无
*/
void Touch_Button_Up(uint16_t x,uint16_t y)
{
	for(uint8_t i=0;i<BUTTON_NUM;i++)
	{
		/* 触摸到了按钮 */
		if(x<=button[i].end_x && y<=button[i].end_y && y>=button[i].start_y && x>=button[i].start_x )
		{
			if(button[i].touch_flag ==1)     /*原本的状态为没有按下，则更新状态*/
			{
				//printf("按键抬起\n");
				button[i].touch_flag = 0;         /* 记录按下标志 */ 
				button[i].draw_btn(&button[i]);  /*重绘按钮*/
				button[i].btn_command(&button[i]);  /*执行按键的功能命令*/
			}        
		}
	}
}
/* ------------------------------------------end of file---------------------------------------- */



