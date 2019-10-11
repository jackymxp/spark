#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "LCD_Display.h"
#include "Button.h"
#include "stm32f10x_exti.h"
#include "touch.h"

#define TICKS_PER_MICROSECOND	72

static void set_step_events_per_minute(uint32_t steps_per_Second);
 
static uint32_t config_step_timer(uint32_t cycles);
 

uint32_t VolSpeedTime;
uint32_t flag=0;
 
Sys_t *systems;
	
static uint8_t Speed_is_Time_to_Change(uint16_t deltatime)          //速度改变周期计数函数
{
	VolSpeedTime += deltatime;
	if(VolSpeedTime > systems->Time_Change_Speed){
		VolSpeedTime -= systems->Time_Change_Speed;
		return 1;
	}else{
		return 0;
	}
}
	

int main(void)
{	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级	 
		
		delay_init();	    	 //延时函数初始化	  
	 
		//uart_init(115200);	 	//串口初始化为115200
	 
		LCD_Init();
	 
		BACK_COLOR = CYAN;
	 
		POINT_COLOR=BLACK;	
		
		LCD_Clear(BACK_COLOR);

		systems = (Sys_t *)malloc(sizeof(Sys_t));
		
		SysInit(systems);

		Init_Display();
	
		Init_Button();
	
		Button_Define_Function(systems);
	
		while(1)
		{
			Scanning_Touch_Action(systems);
			
			SystemScan(systems);
			
			Refresh_Screen(systems);
		}
}
 

//控制步进电机的前进还是后退
void TIM3_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

	if(systems->Auto == 0)
	{		//空走
		
		if(Check_Elec_Touched() == 2 && systems->Motion_State != MOTION_STATE_BACK){
			systems->Motion_State = MOTION_STATE_DEC;
		}
		
		if(systems->Motion_State == MOTION_STATE_START){
			systems->CurrentSpeed = systems->Entry_Speed;
			systems->Motion_State = MOTION_STATE_ACC;
			set_step_events_per_minute(systems->CurrentSpeed);
			VolSpeedTime += systems->deltaTime / 2;
		}
		
		if(systems->Motion_State == MOTION_STATE_BACK){
			set_step_events_per_minute(systems->CurrentSpeed);
			systems->GoBackDistanceifTouched --;
			if(systems->GoBackDistanceifTouched == 0){
				TIM_Cmd(TIM3, DISABLE);
				systems->Motion_State = MOTION_STATE_STP;
			}
	
		}else if(Speed_is_Time_to_Change(systems->deltaTime)){
			if(systems->TempDistance < systems->Acccelerate_Distance && systems->Motion_State == MOTION_STATE_ACC){//加速
					systems->CurrentSpeed += systems->Delta_Speed;
					if(systems->CurrentSpeed >= systems->Nominal_Speed){
						systems->CurrentSpeed = systems->Nominal_Speed;
						systems->Motion_State = MOTION_STATE_NOM;
					}
					
			}else if(systems->Motion_State == MOTION_STATE_DEC){		//减速
					if(systems->CurrentSpeed > systems->Delta_Speed){
						systems->CurrentSpeed -= systems->Delta_Speed;
						
					}else{
						systems->CurrentSpeed = systems->Delta_Speed / 2;
						TIM_Cmd(TIM3, DISABLE);
						systems->Motion_State = MOTION_STATE_STP;
						systems->TempDistance = 0;
					}
				
			}else if(systems->Motion_State == MOTION_STATE_NOM){				//匀速
				systems->Motion_State = MOTION_STATE_NOM;
				systems->CurrentSpeed = systems->Nominal_Speed;
			}
			set_step_events_per_minute(systems->CurrentSpeed);
		}
		
		
		GPIO_SetBits(STEPPER_PORT, STEPPER_PIN);                //原有驱动器脉冲发生函数
		GPIO_ResetBits(STEPPER_PORT, STEPPER_PIN);
		delay_us(1);
		GPIO_SetBits(STEPPER_PORT, STEPPER_PIN);
		systems->TempDistance ++;
		
		/*
		GPIO_ResetBits(STEPPER_PORT, STEPPER_PIN);		          //新买两个不好使的驱动器脉冲发生函数
		delay_us(6);
		GPIO_SetBits(STEPPER_PORT, STEPPER_PIN);
		delay_us(2);
		systems->TempDistance ++;
		*/

		
		if(GPIO_ReadOutputDataBit(STEPPER_PORT, DIR_PIN) == SET){
			systems->Position ++;
		}else
			systems->Position --;
		
	}
	
	else
	{		//加工
			
		if(Speed_is_Time_to_Change(systems->deltaTime)){
			systems->CurrentSpeed = SpeedBaseonSparkDetect();
			set_step_events_per_minute(systems->CurrentSpeed);
		}

			
		  GPIO_SetBits(STEPPER_PORT, STEPPER_PIN);                   //原有驱动器脉冲发生函数
			GPIO_ResetBits(STEPPER_PORT, STEPPER_PIN);
			delay_us(1);
			GPIO_SetBits(STEPPER_PORT, STEPPER_PIN);
		  
		/*
		GPIO_ResetBits(STEPPER_PORT, STEPPER_PIN);		          //新买两个不好使的驱动器脉冲发生函数
		delay_us(6);
		GPIO_SetBits(STEPPER_PORT, STEPPER_PIN);
		delay_us(2);
		*/
		
		if(GPIO_ReadOutputDataBit(STEPPER_PORT, DIR_PIN) == SET){
			systems->Position ++;
		}else
			systems->Position --;
			
	}
}


static uint32_t config_step_timer(uint32_t cycles)            //梯形加减速速度算法
{	
  uint16_t ceiling;
  uint16_t prescaler;
  uint32_t actual_cycles;
  if (cycles <= 0xffffL) {                    //后面的L声明其为长整型，即32位
    ceiling = cycles;
    //prescaler = 1; // prescaler: 0
		prescaler = 1;
    actual_cycles = ceiling;
  } else if (cycles <= 0x7ffffL) {         
    ceiling = cycles >> 3;
    prescaler = 8; // prescaler: 8
    actual_cycles = ceiling * 8L;
  } else if (cycles <= 0x3fffffL) {
    ceiling =  cycles >> 6;
    prescaler = 64; // prescaler: 64
    actual_cycles = ceiling * 64L;
  } else if (cycles <= 0xffffffL) {
    ceiling =  (cycles >> 8);
    prescaler = 256; // prescaler: 256
    actual_cycles = ceiling * 256L;
  } else if (cycles <= 0x3ffffffL) {
    ceiling = (cycles >> 10);
    prescaler = 1024; // prescaler: 1024
    actual_cycles = ceiling * 1024L;
  } else {
    // Okay, that was slower than we actually go. Just set the slowest speed
    ceiling = 0xffff;
    prescaler = 1024;
    actual_cycles = 0xffff * 1024;
  }
  // Set prescaler
	TIM3->PSC = prescaler - 1;
  //TCCR1B = (TCCR1B & ~(0x07<<CS10)) | (prescaler<<CS10);
  // Set ceiling
	TIM3->ARR = ceiling;
  //OCR1A = ceiling;
  return(actual_cycles);
}




//统计开路率，火化率，短路率
void EXTI4_IRQHandler(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
  	EXTI_InitStructure.EXTI_Line=EXTI_Line5;	//KEY2
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	
	if(flag == 1)
		{
			if(systems->SparkSpeed<225)
			{
				systems->SparkSpeed++ ;
				systems->flag = 1;
				Update_Button_Display(systems);		
			}
			/*if(systems->SparkSpeed<225)
				{
			systems->SparkSpeed++ ;
			systems->flag = 1;
			Update_Button_Display(systems);		
			delay_ms(500);
			while(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_4) == RESET)
			{
	       if(systems->SparkSpeed<225)
				 {
				   systems->SparkSpeed++ ;
	         systems->flag = 1;
					 Update_Button_Display(systems);
					 delay_ms(100);
				 }					 
  			}
		  }*/		
			EXTI_Init(&EXTI_InitStructure);	
			flag = 0;
		}
		else{
			    EXTI_InitStructure.EXTI_Line=EXTI_Line4;	
  	      EXTI_InitStructure.EXTI_LineCmd = DISABLE;
			    EXTI_Init(&EXTI_InitStructure);
          flag = 1; 			
		    }
	  EXTI_ClearITPendingBit(EXTI_Line4); //清除LINE0上的中断标志位  
}

void EXTI9_5_IRQHandler(void)
{   
	EXTI_InitTypeDef EXTI_InitStructure;
  	EXTI_InitStructure.EXTI_Line=EXTI_Line5;	//KEY2
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	
	if(flag == 0)
	{
		EXTI_InitStructure.EXTI_LineCmd = DISABLE;
		EXTI_Init(&EXTI_InitStructure);	
		flag = 1;
	}
	  
	else
	 {
			if(systems->SparkSpeed>1)
			{
				systems->SparkSpeed-- ;
				systems->flag = 1;
				Update_Button_Display(systems);
			}
				/*if(systems->SparkSpeed>1)
				{
     	systems->SparkSpeed-- ;
	    systems->flag = 1;
			Update_Button_Display(systems);
			delay_ms(500);
			while(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_5) == RESET)
			{
	       if(systems->SparkSpeed>1)
				 {
				   systems->SparkSpeed-- ;
	         systems->flag = 1;
					 Update_Button_Display(systems);
					 delay_ms(70);
				 }					 
  			}
		  }*/
			EXTI_InitStructure.EXTI_Line=EXTI_Line4;	
			EXTI_Init(&EXTI_InitStructure);	
			flag = 0;
	  }
	 EXTI_ClearITPendingBit(EXTI_Line5); //清除LINE0上的中断标志位 
}

static void set_step_events_per_minute(uint32_t steps_per_Second)          //
{	
  systems->deltaTime = config_step_timer((TICKS_PER_MICROSECOND*1000000)/steps_per_Second);													
}
