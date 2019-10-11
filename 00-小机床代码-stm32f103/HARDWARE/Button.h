#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f10x.h"
#include "touch.h"
#include "Systems.h"




#define EVENTBUTTONPRESSBASE 1

#define EVENTBUTTON0PRESS	1
#define EVENTBUTTON1PRESS	2
#define EVENTBUTTON2PRESS	3
#define EVENTBUTTON3PRESS	4
#define EVENTBUTTON4PRESS	5
#define EVENTBUTTON5PRESS	6
#define EVENTBUTTON6PRESS	7
#define EVENTBUTTON7PRESS	8
#define EVENTBUTTON8PRESS	9
#define EVENTBUTTON9PRESS	10
#define EVENTBUTTON10PRESS	11
#define EVENTBUTTON11PRESS	12


#define EVENTBUTTONRELEASEBASE	21
#define EVENTBUTTON0RELEASE	21
#define EVENTBUTTON1RELEASE	22
#define EVENTBUTTON2RELEASE	23
#define EVENTBUTTON3RELEASE	24
#define EVENTBUTTON4RELEASE	25
#define EVENTBUTTON5RELEASE	26
#define EVENTBUTTON6RELEASE	27
#define EVENTBUTTON7RELEASE	28
#define EVENTBUTTON8RELEASE	29
#define EVENTBUTTON9RELEASE	30
#define EVENTBUTTON10RELEASE	31
#define EVENTBUTTON11RELEASE	32

typedef struct {
	uint8_t Code;
	
	uint16_t Pos_X;
	uint16_t Pos_Y;	
	
	uint8_t State;
	
	uint16_t AreaStart_X;
	uint16_t AreaStart_Y;
	uint16_t AreaEnd_X;
	uint16_t AreaEnd_Y;
	
	uint16_t AreaWord_X;
	uint16_t AreaWord_Y;
	
	uint8_t  StrIndexA;
	uint8_t  StrIndexB;
	
	uint8_t  *Switch;
	
	uint32_t *ValueSwitch;
	
	uint8_t  Value;
	uint8_t	 Type;		
} button_t;

typedef void (*CallBackFun_f)(void);

void InitButtonMod(Sys_t *s);

void Creator_Button(uint16_t Posx,uint16_t Posy, button_t **button, uint8_t Code,uint8_t NameIndexA,uint8_t NameIndexB, CallBackFun_f ClickCallBackFun, CallBackFun_f PressCallBackFun, uint8_t Type);

void Update_Button_Display(Sys_t *s);

void Init_Button(void);

void LCD_Draw_Button(button_t *button);

void Button_Define_Function(Sys_t *S);

void Scanning_Touch_Action(Sys_t *s);

void Btn0_Clicked_CallBack(void);

void Btn1_Clicked_CallBack(void);

void Btn2_Press_CallBack(void);

void Btn3_Press_CallBack(void);

void Btn2_Clicked_CallBack(void);

void Btn3_Clicked_CallBack(void);

void Btn4_Clicked_CallBack(void);

void Slider_Press_CallBack(void);

void Slider_Release_CallBack(void);

void NoneFun(void);



#endif
