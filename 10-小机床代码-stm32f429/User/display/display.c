#include "./display/display.h"
#include "./touch/palette.h"

//���ݰ������µ�״̬����ʾ���ᰴť��״̬
void Draw_Spindle_Btn(void * btn)
{
	Touch_Button *ptr = (Touch_Button *)btn;
   
	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_WHITE);
	
	LCD_DrawFullRect(0,0,20,20);
	//����û�б�����
	if(ptr->touch_flag == 0)
	{
		LCD_DisplayStringLineEx(0,0,32,32,"����: ֹͣ",0);
	}
	if(ptr->touch_flag == 1)
	{
		LCD_DispString_EN_CH(5,5,"����: ����");
	}	
}


