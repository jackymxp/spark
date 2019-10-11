#include "LCD_Display.h"
#include <string.h>

#define RATIOPROGRESSBARWIDTH	20

#define OPEN_WORDS_POS_X	400	
#define SPARK_WORDS_POS_X	400
#define SHORT_WORDS_POS_X	400
#define OPEN_WORDS_POS_Y	150
#define SPARK_WORDS_POS_Y	200
#define SHORT_WORDS_POS_Y	250
#define RATIO_RATE_WIDTH	40

#define OPEN_PROGRESSBAR_POS_X	550
#define SPARK_PROGRESSBAR_POS_X	550
#define SHORT_PROGRESSBAR_POS_X	550
#define OPEN_PROGRESSBAR_POS_Y	155
#define SPARK_PROGRESSBAR_POS_Y	205
#define SHORT_PROGRESSBAR_POS_Y	255

#define RATIO_NUM_BIAS	120

#define TOUCHED_WORDS_POS_X	200
#define TOUCHED_WORDS_POS_Y	200
#define ELEC_WORDS_POS_X	200
#define ELEC_WORDS_POS_Y	150

#define TOUCHED_LED_POS_X 280
#define TOUCHED_LED_POS_Y 205

#define ELEC_LED_POS_X	280
#define ELEC_LED_POS_Y	155

uint8_t ShowNumChar[8];
uint8_t ShowNumCharChangeFlag[8];
uint8_t PrevRatioData[3];
uint8_t PrevTouchState;


static void Init_Show_Words(void)             //汉字显示初始化函数
{
		LCD_ShowWord(0,0,322,60,64,0);
		LCD_ShowWord(0,1,386,60,64,0);
		LCD_ShowChar(450,60,':',64,0);	
	
		LCD_ShowWord(1,8, OPEN_WORDS_POS_X,OPEN_WORDS_POS_Y,32,0);
		LCD_ShowWord(1,9, OPEN_WORDS_POS_X + RATIO_RATE_WIDTH,OPEN_WORDS_POS_Y,32,0);
		LCD_ShowWord(1,14, OPEN_WORDS_POS_X + 2 * RATIO_RATE_WIDTH, OPEN_WORDS_POS_Y, 32, 0);
		
		LCD_ShowWord(1,10, SPARK_WORDS_POS_X,SPARK_WORDS_POS_Y,32,0);
		LCD_ShowWord(1,11, SPARK_WORDS_POS_X + RATIO_RATE_WIDTH, SPARK_WORDS_POS_Y,32,0);
		LCD_ShowWord(1,14, SPARK_WORDS_POS_X + 2 * RATIO_RATE_WIDTH, SPARK_WORDS_POS_Y, 32, 0);
	
		LCD_ShowWord(1,12, SHORT_WORDS_POS_X,SHORT_WORDS_POS_Y,32,0);
		LCD_ShowWord(1,13, SHORT_WORDS_POS_X + RATIO_RATE_WIDTH,SHORT_WORDS_POS_Y,32,0);
		LCD_ShowWord(1,14, SHORT_WORDS_POS_X + 2 * RATIO_RATE_WIDTH, SHORT_WORDS_POS_Y, 32, 0);
		
		LCD_ShowWord(1,4,  ELEC_WORDS_POS_X, ELEC_WORDS_POS_Y, 32,0);
		LCD_ShowWord(1,5,  ELEC_WORDS_POS_X + 40, ELEC_WORDS_POS_Y, 32,0);
		
		LCD_ShowWord(1,6,  TOUCHED_WORDS_POS_X, TOUCHED_WORDS_POS_Y, 32, 0);
		LCD_ShowWord(1,7,  TOUCHED_WORDS_POS_X + 40, TOUCHED_WORDS_POS_Y, 32, 0);
}

void Init_Display(void)	       //显示内容总初始化函数
{
		memset(ShowNumChar,0,sizeof(ShowNumChar));
		memset(ShowNumCharChangeFlag,1,sizeof(ShowNumCharChangeFlag));
		Init_Show_Words();
}

void Update_Distance(int32_t Distance)      //主轴头相对位置显示
{
	if(Distance < 0){
		LCD_ShowChar(482,60,'-',64,0);             //负的先给出负号
		Distance = -Distance;
	}else{
		LCD_ShowChar(482,60,' ',64,0);             //正值先给出空格
	}
	
	for(uint8_t i = 0;i < 8;i++){
		uint8_t byteNum = Distance % 10;
		if(ShowNumChar[i] != byteNum){
			ShowNumChar[i] = byteNum;
			ShowNumCharChangeFlag[i] = 1;
		}
		Distance /= 10;
	}	
	
	for(uint8_t i = 0; i < 8;i++)
	{
		if(ShowNumCharChangeFlag[i]){
			LCD_ShowChar(738 - i * 32,60,ShowNumChar[i],64,0);
		}
	}
	memset(ShowNumCharChangeFlag,0,sizeof(ShowNumCharChangeFlag));
}

static void UpdateSingleRatio(uint8_t Ratio,uint16_t Posx,uint16_t Posy,uint16_t color)     //三率数字显示
{
	LCD_Fill(Posx,Posy,Posx + Ratio,Posy + RATIOPROGRESSBARWIDTH,color);
	LCD_Fill(Posx + Ratio,Posy, Posx+100,Posy + RATIOPROGRESSBARWIDTH,BACK_COLOR);
	
	LCD_ShowChar(Posx + RATIO_NUM_BIAS, Posy - 5, Ratio / 100 + 0x30, 32, 0); 
	LCD_ShowChar(Posx + RATIO_NUM_BIAS + 16, Posy - 5, (Ratio/10) % 10 + 0x30, 32, 0);
	LCD_ShowChar(Posx + RATIO_NUM_BIAS + 32, Posy - 5, Ratio % 10 + 0x30, 32, 0);
}

void Update_SparkRatio(uint8_t Open,uint8_t Spark,uint8_t Short)   //三率进度条显示
{
	if(PrevRatioData[0] != Open){
		UpdateSingleRatio(Open,OPEN_PROGRESSBAR_POS_X, OPEN_PROGRESSBAR_POS_Y,YELLOW);
		PrevRatioData[0] = Open;
	}
	
	if(PrevRatioData[1] != Spark){
		UpdateSingleRatio(Spark,SPARK_PROGRESSBAR_POS_X, SPARK_PROGRESSBAR_POS_Y,GREEN);
		PrevRatioData[1] = Spark;
	}
	
	if(PrevRatioData[2] != Short){
		UpdateSingleRatio(Short,SHORT_PROGRESSBAR_POS_X, SHORT_PROGRESSBAR_POS_Y,RED);
		PrevRatioData[2] = Short;
	}
}

static void Display_Update_Touch(uint8_t State)      //接触/上电感知显示
{
	if(State == 0){
		LCD_Fill(ELEC_LED_POS_X, ELEC_LED_POS_Y, ELEC_LED_POS_X + 20, ELEC_LED_POS_Y + 20, GREEN);
		LCD_Fill(TOUCHED_LED_POS_X, TOUCHED_LED_POS_Y, TOUCHED_LED_POS_X + 20, TOUCHED_LED_POS_Y + 20, GRAY);
	}else if(State == 1){
		LCD_Fill(ELEC_LED_POS_X, ELEC_LED_POS_Y, ELEC_LED_POS_X + 20, ELEC_LED_POS_Y + 20, GRAY);
		LCD_Fill(TOUCHED_LED_POS_X, TOUCHED_LED_POS_Y, TOUCHED_LED_POS_X + 20, TOUCHED_LED_POS_Y + 20, GREEN);
	}else{
		LCD_Fill(ELEC_LED_POS_X, ELEC_LED_POS_Y, ELEC_LED_POS_X + 20, ELEC_LED_POS_Y + 20, GRAY);
		LCD_Fill(TOUCHED_LED_POS_X, TOUCHED_LED_POS_Y, TOUCHED_LED_POS_X + 20, TOUCHED_LED_POS_Y + 20, RED);
	}
}	

void Update_Touched_State(uint8_t NewState)   //接触/上电感知状态函数
{
	if(NewState != PrevTouchState){
		Display_Update_Touch(NewState);
		PrevTouchState = NewState;
	}
}

void Refresh_Screen(Sys_t *s)   //总显示内容更新函数
{ 
	Update_Touched_State(s->Touched);
	Update_SparkRatio(s->OpenRatio, s->SparkRatio, s->ShortRatio);
	Update_Distance(s->Position);
}

