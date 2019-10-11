/**
  ******************************************************************************
  * @file    palette.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ��������Ӧ�ú���
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

/*��ť�ṹ������*/
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
	LCD_DispString_EN_CH(y ,x - 24,"����");
}


void Spindle_Command(void* btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;	
	//���Ὺ
	if((ptr->touch_num & 0x01)  )
	{
		CurrState.SpindleState = ENABLE;
	}
	//����ֹͣ
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
	LCD_DispString_EN_CH(y ,x-24,"ˮ��");
}







void Coolant_Command(void* btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;
	//ˮ�ÿ�
	if((ptr->touch_num & 0x01) == 1  )
	{
		CurrState.CoolantState = ENABLE;
	}
	//ˮ��ֹͣ
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
	LCD_DispString_EN_CH(y ,x - 24,"����");
}


void StepMotor_Up_Command(void* btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;
	
	//������ť���������ı䲽��������������򣬲���ʹ�ܲ������
	if(ptr->touch_flag)
	{
		//LED_GREEN;
		CurrState.MotorState.MSDState = MSD_RUNNING;
		CurrState.MotorState.MSDdir = MSD_UP;
		CurrState.MotorState.MSDMode = MSD_LASTING;
		CurrState.MotorState.MSDStep = ADJUSTSTEP;//��LASTINGģʽ�£�ֻҪ���ô���0����
		//CurrState.MotorState.MSDSpeed = ADJUSTSPEED;
	}
	//������ť���������ı䲽���������������ֹͣ�������
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
	LCD_DispString_EN_CH(y ,x - 24,"�½�");
}

void StepMotor_Down_Command(void* btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;
	//�½���ť���������ı䲽��������������򣬲���ʹ�ܲ������
	if(ptr->touch_flag)
	{
		//LED_GREEN;
		CurrState.MotorState.MSDState = MSD_RUNNING;
		CurrState.MotorState.MSDdir = MSD_DOWN;
		CurrState.MotorState.MSDMode = MSD_LASTING;
		CurrState.MotorState.MSDStep = ADJUSTSTEP;//��LASTINGģʽ�£�ֻҪ���ô���0����
		//CurrState.MotorState.MSDSpeed = ADJUSTSPEED;
	}
	//�½���ť���������ı䲽���������������ֹͣ�������
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
	//	LCD_DispString_EN_CH(y ,x - 24, "��ѹ:33V");
	}
	else
	{
	//	LCD_SetTextColor(LCD_COLOR_RED);
		//LCD_DispString_EN_CH(y ,x - 24, "��ѹ:33V");	
	}
}
void V_Command(void* btn)
{}
	
//ֹͣ��ť
void Draw_Stop_Btn(void* btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;
	
	LCD_DrawFullRect(ptr->start_x,ptr->start_y,ptr->end_x - ptr->start_x,ptr->end_y - ptr->start_y);
	int x = (ptr->start_x + ptr->end_x) / 2;
	int y = (ptr->start_y + ptr->end_y) / 2;
	if((ptr->touch_num & 0x01)  )
	{
		LCD_SetTextColor(LCD_COLOR_GREEN);
		LCD_DispString_EN_CH(y ,x - 24, "ֹͣ");
	}
	else
	{
		LCD_SetTextColor(LCD_COLOR_RED);
		LCD_DispString_EN_CH(y ,x - 24, "��ʼ");	
	}		
}

void Stop_Command(void* btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;
	//��ʼ�ӹ�
	if((ptr->touch_num & 0x01)  )
	{
		CurrState.AutoWorking = ENABLE;

		CurrState.MotorState.MSDState = MSD_RUNNING;
		CurrState.MotorState.MSDMode = MSD_PLUSE;
		CurrState.MotorState.MSDStep = OPENSTEP;
		CurrState.MotorState.MSDSpeed = OPENSPEED;
		
	}
	//ֹͣ�ӹ�
	else
	{
		CurrState.AutoWorking = DISABLE;
		
		CurrState.MotorState.MSDState = MSD_STOP;
		CurrState.MotorState.MSDMode = MSD_PLUSE;
	}	
	
}

/**
* @brief  Touch_Button_Init ��ʼ����ť����
* @param  ��
* @retval ��
*/
void Touch_Button_Init(void)
{
	/*���ᰴť*/
	button[0].start_x = BUTTON_START_X;
	button[0].start_y = 0;
	button[0].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH ;
	button[0].end_y = button[0].start_y + 1 * COLOR_BLOCK_HEIGHT;
	button[0].touch_num = 0; 
	button[0].touch_flag = 0;  
	button[0].draw_btn = Draw_Spindle_Btn ;//ָ����ɫ��ť����Ӧ�Ļ���
	button[0].btn_command = Spindle_Command ;//ָ��˰�������ȥ��Ҫִ�еĹ���
	
	/*ˮ�ð�ť*/
	button[1].start_x = BUTTON_START_X;
	button[1].start_y = button[0].start_y + 1 * COLOR_BLOCK_HEIGHT;
	button[1].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH ;
	button[1].end_y = button[0].start_y + 2 * COLOR_BLOCK_HEIGHT;
	button[1].touch_num = 0; 
	button[1].touch_flag = 0;  
	button[1].draw_btn = Draw_Coolant_Btn ;//ָ����ɫ��ť����Ӧ�Ļ���
	button[1].btn_command = Coolant_Command ;//ָ��˰�������ȥ��Ҫִ�еĹ���
	
	/*�������������ť*/
	button[2].start_x = BUTTON_START_X;
	button[2].start_y = button[0].start_y + 2 * COLOR_BLOCK_HEIGHT;
	button[2].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH ;
	button[2].end_y = button[0].start_y + 3 * COLOR_BLOCK_HEIGHT;
	button[2].touch_num = 0; 
	button[2].touch_flag = 0;  
	button[2].draw_btn = Draw_StepMotor_Up_Btn ;//ָ����ɫ��ť����Ӧ�Ļ���
	button[2].btn_command = StepMotor_Up_Command ;//ָ��˰�������ȥ��Ҫִ�еĹ���
	
	/*��������½���ť*/
	button[3].start_x = BUTTON_START_X;
	button[3].start_y = button[0].start_y + 3 * COLOR_BLOCK_HEIGHT;
	button[3].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH ;
	button[3].end_y = button[0].start_y + 4 * COLOR_BLOCK_HEIGHT;
	button[3].touch_num = 0; 
	button[3].touch_flag = 0;  
	button[3].draw_btn = Draw_StepMotor_Down_Btn ;//ָ����ɫ��ť����Ӧ�Ļ���
	button[3].btn_command = StepMotor_Down_Command ;//ָ��˰�������ȥ��Ҫִ�еĹ���
	
	
	/*ֹͣ��ť*/
	button[4].start_x = BUTTON_START_X;
	button[4].start_y = button[0].start_y + 4 * COLOR_BLOCK_HEIGHT;
	button[4].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH ;
	button[4].end_y = button[0].start_y + 5 * COLOR_BLOCK_HEIGHT;
	button[4].touch_num = 0; 
	button[4].touch_flag = 0;  
	button[4].draw_btn = Draw_Stop_Btn ;//ָ����ɫ��ť����Ӧ�Ļ���
	button[4].btn_command = Stop_Command ;//ָ��˰�������ȥ��Ҫִ�еĹ���
	
		/*��ѹ��ť*/
	button[5].start_x = PALETTE_END_X - 1.5 * COLOR_BLOCK_WIDTH;
	button[5].start_y = 0;
	button[5].end_x = PALETTE_END_X;
	button[5].end_y = COLOR_BLOCK_HEIGHT;
	button[5].touch_num = 0; 
	button[5].touch_flag = 0;  
	button[5].draw_btn = Draw_V_Btn ;//ָ����ɫ��ť����Ӧ�Ļ���
	button[5].btn_command = V_Command ;//ָ��˰�������ȥ��Ҫִ�еĹ���
	
	
}




/**********************���µĺ�������Ҫ���κεĸĶ���***********************************/

/**
* @brief  Palette_Init �����ʼ��
* @param  ��
* @retval ��
*/
void Palette_Init(void)
{
	uint8_t i;
	/* ������Ϊ��ɫ */
	LCD_Clear(CL_WHITE);	/* ��������ʾȫ�� */ 
	/* ��ʼ����ť */
	Touch_Button_Init();
	/* ��水ť */
	for(i=0;i<BUTTON_NUM;i++)
	{
		button[i].draw_btn(&button[i]);
	}	
}
/**
* @brief  Touch_Button_Down ����������ʱ���õĺ������ɴ���������
* @param  x ����λ�õ�x����
* @param  y ����λ�õ�y����
* @retval ��
*/
void Touch_Button_Down(uint16_t x,uint16_t y)
{
	for(uint8_t i=0;i<BUTTON_NUM;i++)
	{
		/* �������˰�ť */
		if(x<=button[i].end_x && y<=button[i].end_y && y>=button[i].start_y && x>=button[i].start_x )
		{
			if(button[i].touch_flag == 0)     /*ԭ����״̬Ϊû�а��£������״̬*/
			{
				//printf("��������\n");
				button[i].touch_num++;
				button[i].touch_flag = 1;         /* ��¼���±�־ */ 
				button[i].draw_btn(&button[i]);  /*�ػ水ť*/
				button[i].btn_command(&button[i]);  /*ִ�а����Ĺ�������*/
			}        
		}
	}
}

/**
* @brief  Touch_Button_Up �������ͷ�ʱ���õĺ������ɴ���������
* @param  x ��������ͷ�ʱ��x����
* @param  y ��������ͷ�ʱ��y����
* @retval ��
*/
void Touch_Button_Up(uint16_t x,uint16_t y)
{
	for(uint8_t i=0;i<BUTTON_NUM;i++)
	{
		/* �������˰�ť */
		if(x<=button[i].end_x && y<=button[i].end_y && y>=button[i].start_y && x>=button[i].start_x )
		{
			if(button[i].touch_flag ==1)     /*ԭ����״̬Ϊû�а��£������״̬*/
			{
				//printf("����̧��\n");
				button[i].touch_flag = 0;         /* ��¼���±�־ */ 
				button[i].draw_btn(&button[i]);  /*�ػ水ť*/
				button[i].btn_command(&button[i]);  /*ִ�а����Ĺ�������*/
			}        
		}
	}
}
/* ------------------------------------------end of file---------------------------------------- */



