/**
  ******************************************************************************
  * @file    palette.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ��������Ӧ�ú���
  ******************************************************************************
  * @attention
  * ���ⲿʵ�� Touch_Button_Init ��ʼ����ť����
  */

#include "./touch/palette.h"


/*��ť�ṹ������*/
Touch_Button button[BUTTON_NUM];



extern void Touch_Button_Init(void);
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
	for(i = 0;i < BUTTON_NUM; i++)
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
			if(button[i].touch_flag == 1)     /*ԭ����״̬Ϊû�а��£������״̬*/
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



