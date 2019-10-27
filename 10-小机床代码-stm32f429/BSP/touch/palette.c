/**
  ******************************************************************************
  * @file    palette.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   触摸画板应用函数
  ******************************************************************************
  * @attention
  * 在外部实现 Touch_Button_Init 初始化按钮参数
  */

#include "./touch/palette.h"


/*按钮结构体数组*/
Touch_Button button[BUTTON_NUM];



extern void Touch_Button_Init(void);
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
	for(i = 0;i < BUTTON_NUM; i++)
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
			if(button[i].touch_flag == 1)     /*原本的状态为没有按下，则更新状态*/
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



