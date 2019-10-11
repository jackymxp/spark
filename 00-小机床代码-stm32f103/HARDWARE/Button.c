#include "Button.h"
#include "lcd.h"
#include "led.h"

#define BUTTON_WIDTH	120 //px
#define BUTTON_HEIGHT	 80 //px

#define BUTTON_WORD_BIA_X	20
#define BUTTON_WORD_BIA_Y 20

#define PAINT_WIDTH		4		//px
#define SPEED_WORDS_POS_X	430
#define SPEED_WORDS_POS_Y	340

#define SLIDER_POS_X	500
#define SLIDER_POS_Y	350
#define SLIDER_BACKGROUND_WIDTH	50
#define SLIDER_ENABLE_WIDTH	100
#define SLIDER_WIDTH	20
#define SLIDER_LENGTH	250

uint16_t ButtonExsitFlag; //to Code 

uint16_t ButtonChangeFlag;

uint16_t ButtonActionState;

uint8_t ButtonEvent;

uint16_t SliderPrevPos_X;

CallBackFun_f		BtnClickCallBackFun[16];
CallBackFun_f		BtnPressCallBackFun[16];

button_t *btn[16];

static void Display_Word(button_t *button)      //��ʾ�����ᣬˮ�ã��������½�����ʼ��ֹͣ��
{
	if(button->State == ENABLE)
	{
		LCD_ShowWord(1,button->StrIndexA,button->AreaStart_X + 20,button->AreaStart_Y + 25,32,0);
		LCD_ShowWord(1,button->StrIndexA + 1,button->AreaStart_X + 64,button->AreaStart_Y + 25 ,32,0);
	}
	else
	{
		LCD_ShowWord(1,button->StrIndexB ,button->AreaStart_X + 20, button->AreaStart_Y + 25,32,0);
		LCD_ShowWord(1,button->StrIndexB + 1 ,button->AreaStart_X + 64, button->AreaStart_Y + 25 ,32,0);
	}
}

static void DisplaySpeed(uint8_t SpeedtoDisplay)    //�ŷ������ٶ���ʾ����
{
	LCD_ShowChar(SPEED_WORDS_POS_X,SPEED_WORDS_POS_Y,SpeedtoDisplay/100 + 0x30,32,0);
	LCD_ShowChar(SPEED_WORDS_POS_X + 16,SPEED_WORDS_POS_Y,SpeedtoDisplay/10 % 10 + 0x30,32,0);
	LCD_ShowChar(SPEED_WORDS_POS_X + 32,SPEED_WORDS_POS_Y,SpeedtoDisplay % 10 + 0x30,32,0);
}

static void ClearSliderBlock(void)       //�������
{	
	if(SliderPrevPos_X > SLIDER_POS_X && SliderPrevPos_X < SLIDER_POS_X + SLIDER_LENGTH){
		LCD_Fill(SliderPrevPos_X, SLIDER_POS_Y - 20, SliderPrevPos_X + 20, SLIDER_POS_Y + 20, BACK_COLOR);
	}
}
static void DrawSliderBlock(uint16_t PosX)    //������
{
	LCD_Fill(PosX,SLIDER_POS_Y - 20, PosX + 20, SLIDER_POS_Y + 20, BLUE);
	SliderPrevPos_X = PosX;
}

static void Update_SliderBlock(button_t *button)    //�������麯��
{
	ClearSliderBlock();
	DrawSliderBlock(button->Value + SLIDER_POS_X);
}


static void Display_Frame(button_t *button)      //�������ᣬˮ�á������ȵ����(����������һ����ȱ߿򣬴���㵽�ڲ㣬һ��һ�㻭��)
{
	if(button->State == ENABLE){
		POINT_COLOR = GREEN;
		for(uint8_t i = 0;i < PAINT_WIDTH;i++){
			LCD_DrawRectangle(button->AreaStart_X + i,button->AreaStart_Y + i,button->AreaEnd_X - i,button->AreaEnd_Y - i);
		}
	}else{
		POINT_COLOR = RED;
		for(uint8_t i = 0;i < PAINT_WIDTH;i++){
			LCD_DrawRectangle(button->AreaStart_X + i,button->AreaStart_Y + i,button->AreaEnd_X - i,button->AreaEnd_Y - i);
		}
	}
	POINT_COLOR = BLACK;
//	Display_Word(button);
}

static void Update_Single_Button_Display(button_t *b,Sys_t *s)    //���ݴ���״̬�������ڰ���������������и�������
{
	if(b->Type == 1){
		s->SparkSpeed = b->Value;
		Update_SliderBlock(b);	
	}else{
		Display_Frame(b);
	}

}

static void Update_Single_Button_Word(button_t *button)    //���ݴ���������ڡ���ʼ/ֹͣ�����з�ת��ʾ
{
	if(button->State == ENABLE){
		LCD_ShowWord(1,button->StrIndexA,button->AreaStart_X + 20,button->AreaStart_Y + 25,32,0);
		LCD_ShowWord(1,button->StrIndexA + 1,button->AreaStart_X + 64,button->AreaStart_Y + 25 ,32,0);
	}else{
		LCD_ShowWord(1,button->StrIndexB ,button->AreaStart_X + 20, button->AreaStart_Y + 25,32,0);
		LCD_ShowWord(1,button->StrIndexB + 1 ,button->AreaStart_X + 64, button->AreaStart_Y + 25 ,32,0);
	}
}

void Creator_Button(uint16_t Posx,uint16_t Posy, button_t **button, uint8_t Code,uint8_t NameIndexA,uint8_t NameIndexB, CallBackFun_f ClickCallBackFun, CallBackFun_f PressCallBackFun, uint8_t Type)     //�������ɺ���
{ 
		*button = (button_t *)malloc(sizeof(button_t));
	
		(*button)->Pos_X = Posx;
		(*button)->Pos_Y = Posy;
		(*button)->Code = Code;
	
		//���ְ�ť����
		if(Type == 0)
		{	
			(*button)->AreaStart_X = Posx;
			(*button)->AreaStart_Y = Posy;
			(*button)->AreaEnd_X = Posx + BUTTON_WIDTH;
			(*button)->AreaEnd_Y = Posy + BUTTON_HEIGHT;
			(*button)->AreaWord_X = Posx + BUTTON_WORD_BIA_X;
			(*button)->AreaWord_Y = Posy + BUTTON_WORD_BIA_Y;
		}
		
		else
		{
			(*button)->AreaStart_X = Posx;
			(*button)->AreaStart_Y = Posy - SLIDER_BACKGROUND_WIDTH / 2;
			(*button)->AreaEnd_X = Posx + SLIDER_LENGTH;
			(*button)->AreaEnd_Y = Posy + SLIDER_BACKGROUND_WIDTH / 2;
		}
		
		(*button)->State = DISABLE;
		(*button)->StrIndexA = NameIndexA;
		(*button)->StrIndexB = NameIndexB;
		(*button)->Type = Type;
		(*button)->Switch = NULL;
		
		ButtonExsitFlag |= (1 << Code);       //��һ��1��0��ɵĴ���֤�����а����Ĵ�����
		btn[Code] = *button;
		ButtonChangeFlag |= (1 << Code);
	
		BtnClickCallBackFun[Code] = ClickCallBackFun;
		BtnPressCallBackFun[Code] = PressCallBackFun;
}


void Update_Button_Display(Sys_t *s)        //�����ʹ�������֤���Ӷ������������
{ 
	if(s->flag==1)
	{
	  btn[5]->Value = s->SparkSpeed;
	  DisplaySpeed(btn[5]->Value);
	  Update_Single_Button_Display(btn[5],s);
	  s->flag=0;
	}
	for(uint8_t i = 0;i < 16;i++){
		if(((ButtonChangeFlag >> i) & 0x01) && ((ButtonExsitFlag >> i) & 0x01)){	//Button exsit and Button state need to update
			Update_Single_Button_Display(btn[i],s);
		}
	}
	ButtonChangeFlag = 0;
}

void Init_Button(void)  //������ʼ������
{
	tp_dev.init();//��ʼ��������
	
	button_t *a;
	button_t *b;
	button_t *c;
	button_t *d;
	button_t *e;
	button_t *f;

	Creator_Button(40,50,&a,0,0,0,&Btn0_Clicked_CallBack,&NoneFun,0);
	Creator_Button(40,150,&b,1,2,2,&Btn1_Clicked_CallBack,&NoneFun,0);
	Creator_Button(40,250,&c,2,19,19, &Btn2_Clicked_CallBack,&Btn2_Press_CallBack,0);
	Creator_Button(40,350,&d,3,21,21, &Btn3_Clicked_CallBack, &Btn3_Press_CallBack,0);
	Creator_Button(180,350,&e,4,23,25,&Btn4_Clicked_CallBack,&NoneFun, 0);
	Creator_Button(SLIDER_POS_X,SLIDER_POS_Y, &f, 5, 23, 25, &Slider_Release_CallBack, &Slider_Press_CallBack, 1);
	
	a->State = 0;
	b->State = 0;
	c->State = 0;
	d->State = 0;
	e->State = 0;
	f->State = 0;
	f->Value = 100;
	
	DisplaySpeed(btn[5]->Value);
	
	Display_Word(a);
	Display_Word(b);
	Display_Word(c);
	Display_Word(d);
	Display_Word(e);
	Display_Frame(f);	
}

void Scanning_Touch_Action(Sys_t *s)   //����ɨ�躯��
{
	uint16_t TouchPosX;
	uint16_t TouchPosY;
	
	tp_dev.scan(0);                //��ȡ��������
	
	if(tp_dev.sta & (1 << 7))
	{
		TouchPosX = tp_dev.x[0];
		TouchPosY = tp_dev.y[0];
		
		for(uint8_t i = 0; i< 16; i++)
		{
			if(ButtonExsitFlag & (1 << i))
			{
				//�жϴ������ĸ���ť����
				if(TouchPosX > btn[i]->AreaStart_X && TouchPosX < btn[i]->AreaEnd_X && TouchPosY > btn[i]->AreaStart_Y && TouchPosY < btn[i]->AreaEnd_Y)
				{
					if(ButtonEvent == 0)
					{
						ButtonEvent = EVENTBUTTONPRESSBASE + i;		//No Touch Previous
					}
					else if(ButtonEvent != EVENTBUTTONPRESSBASE + i)
					{ 	//Slider To Another Button 
						BtnClickCallBackFun[ButtonEvent - EVENTBUTTONPRESSBASE]();
						ButtonEvent = EVENTBUTTONPRESSBASE + i;
					}
					BtnPressCallBackFun[i]();
				}
			}
		}	
	}
	
	else
	{	//��Ļû�е�����ͷŰ�ť
		if(ButtonEvent != 0){		//�����ظ�����	
				uint8_t code = ButtonEvent - EVENTBUTTONPRESSBASE;
				
				BtnClickCallBackFun[code]();
				ButtonEvent = 0;	
		}
	}
	Update_Button_Display(s);
}




void Btn0_Clicked_CallBack(void)            //���ᴥ����ʽ�趨
{
	if(btn[0]->Switch == NULL) 
		return;
	
	btn[0]->State ^= 1;
	
	*(btn[0]->Switch) = btn[0]->State;
	
	ButtonChangeFlag |= (1 << 0);
}

void Btn1_Clicked_CallBack(void)         //ˮ�ô�����ʽ�趨
{
	if(btn[1]->Switch == NULL) return;
	
	btn[1]->State ^= 1;
	
	*(btn[1]->Switch) = btn[1]->State;
	
	ButtonChangeFlag |= (1 << 1);

}

void Btn2_Press_CallBack(void)              //����������ʽ�趨(��������)
{
	if(btn[2]->Switch == NULL) return;
	
	btn[2]->State = 1;
	
	if(*(btn[2]->Switch) != btn[2]->State){
		ButtonChangeFlag |= (1 << 2);
		*(btn[2]->Switch) = btn[2]->State;
	}
}

void Btn3_Press_CallBack(void)                 //�½�������ʽ�趨(��������)
{
	if(btn[3]->Switch == NULL) return;
	
	btn[3]->State = 1;
	
	if(*(btn[3]->Switch) != btn[3]->State){
		ButtonChangeFlag |= (1 << 3);
		*(btn[3]->Switch) = btn[3]->State;
	}

}

void Btn2_Clicked_CallBack(void)               //����������ʽ�趨(�㶯����)
{
	if(btn[2]->Switch == NULL) return;
	btn[2]->State = 0;
	
	if(*(btn[2]->Switch) != btn[2]->State){
		ButtonChangeFlag |= (1 << 2);
		*(btn[2]->Switch) = btn[2]->State;
	}
}
void Btn3_Clicked_CallBack(void)                 //�½�������ʽ�趨(�㶯����)
{
	if(btn[3]->Switch == NULL) return;
	
	btn[3]->State = 0;
	
	if(*(btn[3]->Switch) != btn[3]->State){
		ButtonChangeFlag |= (1 << 3);
		*(btn[3]->Switch) = btn[3]->State;
	}
}

void Btn4_Clicked_CallBack(void)                    //��ʼ/ֹͣ��ת�㶯����
{	
	if(btn[4]->Switch == NULL) return;
		
	btn[4]->State ^= 1;	
	
	if(*(btn[4]->Switch) != btn[4]->State){
		Display_Word(btn[4]);
		*(btn[4]->Switch) = btn[4]->State;
		ButtonChangeFlag |= (4 << 1);
	}
}

void Slider_Press_CallBack(void)               //�����麯��
{
	if(tp_dev.x[0] < SLIDER_POS_X + SLIDER_LENGTH - 20){
		
		btn[5]->Value = tp_dev.x[0] - SLIDER_POS_X;
		
		DisplaySpeed(btn[5]->Value);
		
		Update_SliderBlock(btn[5]);
		
		ButtonChangeFlag |= (5 << 1);
	}
}

void Slider_Release_CallBack(void)            //�����ͷź���
{
	if(btn[5]->Switch == NULL) return;
	*(btn[5]->Switch) = btn[5]->Value;
	
}

void Button_Define_Function(Sys_t *S)
{
		btn[0]->Switch = &(S->SpindleState);//����״̬
		btn[1]->Switch = &(S->CoolantState);//��ȴ״̬
		btn[2]->Switch = &(S->GoUp);//����
		btn[3]->Switch = &(S->GoDown);//����
		btn[4]->Switch = &(S->Auto);//����
		btn[5]->Switch = &(S->SparkSpeed);//��ʾ�Ǹ���·��  ����
	
		*(btn[0]->Switch) = 0;
		*(btn[1]->Switch) = 0;
		*(btn[2]->Switch) = 0;
		*(btn[3]->Switch) = 0;
		*(btn[4]->Switch) = 0;
		
}

void InitButtonMod(Sys_t *s)
{
	 ButtonExsitFlag = 0x00;
	 ButtonChangeFlag = 0x00;
	 SliderPrevPos_X = 0;
	
	 Init_Button();
	
	 Button_Define_Function(s);	
	
	 Update_Button_Display(s);
	
}	


void NoneFun(void)
{
	

}

