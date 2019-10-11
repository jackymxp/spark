#include "Systems.h"
#include "led.h"
#include "delay.h"
#include "stm32f10x_exti.h"

#define DETECT_TIME 10
#define DETECT_TIMER_PSC 36
#define DETECT_TIMER_ARR (DETECT_TIME*72/DETECT_TIMER_PSC)
#define DETECT_CALCULATE_TIME		200 * 1000		// 200ms 统计一次开路火花短路率

#define ENCODER_TIMER   TIM4  // Encoder unit connected to TIM3  
#define ENCODER_PPR           (u16)(100)   // number of pulses per revolution  
#define SPEED_BUFFER_SIZE 8  
  
#define COUNTER_RESET   (u16)0  
#define ICx_FILTER      (u8) 6 // 6<-> 670nsec  
  
#define TIMx_PRE_EMPTION_PRIORITY 1  
#define TIMx_SUB_PRIORITY 0  
  
#define ENCODER_TIM_PERIOD 0xffff//???????65536-1  
#define MAX_COUNT          30//10000???1ms?????10000???  


uint32_t detectRatioOpenCounter;
uint32_t detectRatioSparkCounter;
uint32_t detectRatioShortCounter;



uint32_t DetectCounter;

Sys_t *Syst;

static void Enable_Coolant(uint8_t NewState)                   //水泵启动/关闭函数
{
	if(NewState == 1){GPIO_ResetBits(COOLANT_PORT, COOLANT_PIN);}
	else{GPIO_SetBits(COOLANT_PORT, COOLANT_PIN);}
}
	

static void Enable_Spindle(uint8_t NewState)                  //主轴启动/关闭函数
{
	if(NewState == 1){GPIO_ResetBits(SPINDLE_PORT, SPINDLE_PIN);}
	else{GPIO_SetBits(SPINDLE_PORT, SPINDLE_PIN);}
}



uint8_t Check_Elec_Touched(void)                      //上电感知状态模块
{
	if(GPIO_ReadInputDataBit(ELEC_PORT, ELEC_PIN) == SET)//上电返回0
		return 0;
	else
	{
		if(GPIO_ReadInputDataBit(TOUCHED_PORT, TOUCHED_PIN) == SET) return 1;         //未上电未接触返回1
		else return 2;                                                                //未上电接触返回2
	}
}

void SystemScan(Sys_t *s)                                       //全系统扫描，功能更新模块
{	
	Enable_Spindle(s->SpindleState);                     //主轴启动/关闭
	Enable_Coolant(s->CoolantState);                      //水泵启动/关闭
	
	//不使用自动加工功能
	if(s->Auto == 0)
	{                                     //如果自动加工功能关闭                 
		Syst->Time_Change_Speed = 72000;                  //速度改变时间间隔
		//检查接触感知的状态
		Syst->Touched = Check_Elec_Touched();
		
		//未上电接触了，则自动回退一点
		if(Syst->Touched == 2 && Syst->Motion_State == MOTION_STATE_STP)
		{  //如果接触感知成立，且主轴伺服运动停止，回退DEFAULT_GOBACK_DISTANCE的距离
			s->Motion_State = MOTION_STATE_BACK;                                      //电机反向
			s->GoBackDistanceifTouched = DEFAULT_GOBACK_DISTANCE;
		}
		
		if(s->Motion_State == MOTION_STATE_WORK){   //如果步进电机处于自动加工伺服进给状态，则自动失能TIM3和TIM4定时器，改方向为正向
			TIM_Cmd(TIM3, DISABLE);
			TIM_Cmd(TIM4, DISABLE);
			
			s->Motion_State = MOTION_STATE_STP;
		}
		
		if(s->Motion_State == MOTION_STATE_BACK )
		{     //如果处于接触感知自回退状态，同时电机回退，打开定时器TIM3
			if(s->GoBackDistanceifTouched == DEFAULT_GOBACK_DISTANCE){
				s->CurrentSpeed = s->Entry_Speed;
				GPIO_SetBits(STEPPER_PORT, DIR_PIN);
				TIM_Cmd(TIM3, ENABLE);
			}
			
		}
		else 
		{		
			if(s->GoUp == 1 && s->Motion_State == MOTION_STATE_STP){          //如果按下上升并且电机处于停止状态，则开始回退
				GPIO_SetBits(STEPPER_PORT, DIR_PIN);
				s->Motion_State = MOTION_STATE_START;
				TIM_Cmd(TIM3, ENABLE);
				
			}else if(s->GoDown == 1  && s->Motion_State == MOTION_STATE_STP){         //如果按下下降并且电机处于停止状态，则开始进给
				s->Motion_State = MOTION_STATE_START;
				GPIO_ResetBits(STEPPER_PORT, DIR_PIN);
				TIM_Cmd(TIM3, ENABLE);
				
			}else if(s->GoUp == 0 && s->GoDown == 0 && (s->Motion_State != MOTION_STATE_STP)){    //如果上升下降按钮都没按下，且电机处于工作状态，则开始减速
				s->Motion_State = MOTION_STATE_DEC;
				
			}
		}
		
		
	}
	
	//如果按下自动加工按钮，则自动使能两个定时器，开始伺服加工
	else{                              
		Syst->Time_Change_Speed = 72000;
		if(s->Motion_State != MOTION_STATE_WORK){
			TIM_Cmd(TIM4, ENABLE);	//开放电统计
			TIM_Cmd(TIM3, ENABLE);		//开运动
			s->Motion_State = MOTION_STATE_WORK;
		}
	}

}


static void init_detect(void)                          //初始化检测电路时钟(TIM4)，以及检测电路A1引脚电平，每次中断周期为0.00001s
{
	TIM_DeInit(TIM4);
	
	TIM_TimeBaseInitTypeDef Detect_timer;
	Detect_timer.TIM_ClockDivision = TIM_CKD_DIV1;
	Detect_timer.TIM_CounterMode = TIM_CounterMode_Up;
	Detect_timer.TIM_Period =  DETECT_TIMER_ARR;
	Detect_timer.TIM_Prescaler = DETECT_TIMER_PSC - 1;

	TIM_TimeBaseInit(TIM4, &Detect_timer);
	
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	NVIC_InitTypeDef	NVICTIM;
	NVICTIM.NVIC_IRQChannel = TIM4_IRQn;
	NVICTIM.NVIC_IRQChannelCmd = ENABLE;
	NVICTIM.NVIC_IRQChannelPreemptionPriority = 0;
	NVICTIM.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVICTIM);
	
	GPIO_InitTypeDef	DetectGPIO;
	DetectGPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING;              //配置引脚为输入
	DetectGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	DetectGPIO.GPIO_Pin = DETECT_OPEN_PIN;                     //A0引脚  
	GPIO_Init(DETECT_OPEN_PORT, &DetectGPIO);
		
	DetectGPIO.GPIO_Pin = DETECT_SHORT_PIN;                    //E0引脚
	GPIO_Init(DETECT_SHORT_PORT, &DetectGPIO);
	
	DetectGPIO.GPIO_Pin = DETECT_BASE_PULSE_PIN;               //A1引脚
	DetectGPIO.GPIO_Mode = GPIO_Mode_Out_PP;                   //配置引脚为推挽输出
	GPIO_Init(DETECT_BASE_PULSE_PORT, &DetectGPIO);
	
	GPIO_SetBits(DETECT_BASE_PULSE_PORT, DETECT_BASE_PULSE_PIN);   //将A1引脚置位一，目的为使得其与A0和E0检测引脚做与运算
	

}

void init_xuanniu(void)
{
  GPIO_InitTypeDef	gpioinit;
	gpioinit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioinit.GPIO_Pin = GPIO_Pin_4 ;
	gpioinit.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOG, &gpioinit); 
	
	gpioinit.GPIO_Pin = GPIO_Pin_5 ;
	GPIO_Init(GPIOG, &gpioinit);

}

static void init_EXIT(void) 
{   
	  GPIO_InitTypeDef	DetectGPIO;
	  DetectGPIO.GPIO_Mode = GPIO_Mode_IPU;              //配置引脚为输入
	  DetectGPIO.GPIO_Speed = GPIO_Speed_10MHz;
	  DetectGPIO.GPIO_Pin = GPIO_Pin_4;                     //B6引脚  
	  GPIO_Init(GPIOG, &DetectGPIO);
	
	  DetectGPIO.GPIO_Pin = GPIO_Pin_5;                     //B7引脚  
	  GPIO_Init(GPIOG, &DetectGPIO);

	
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource4);
	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource5);
	
	  EXTI_InitTypeDef EXTI_InitStructure;
  	EXTI_InitStructure.EXTI_Line=EXTI_Line4;	//KEY2
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	  
  	EXTI_InitStructure.EXTI_Line=EXTI_Line5;	//KEY2  	
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	
		
	  NVIC_InitTypeDef	NVICTIM;
	  NVICTIM.NVIC_IRQChannel = EXTI4_IRQn;
	  NVICTIM.NVIC_IRQChannelCmd = ENABLE;
	  NVICTIM.NVIC_IRQChannelPreemptionPriority = 0;
	  NVICTIM.NVIC_IRQChannelSubPriority = 1;
	  NVIC_Init(&NVICTIM);
				
		NVICTIM.NVIC_IRQChannel = EXTI9_5_IRQn;
		NVIC_Init(&NVICTIM);
	
	
	  
}

static void init_Timer(void)                              //初始化伺服运动时钟TIM3
{
	TIM_TimeBaseInitTypeDef	Stepper_timer;
	Stepper_timer.TIM_ClockDivision = TIM_CKD_DIV1;         //时钟分频因子(此时定时器频率等于系统时钟频率)
	Stepper_timer.TIM_CounterMode = TIM_CounterMode_Up;     //向上计数办法
	Stepper_timer.TIM_Period = 10;                          //预装值为10
	Stepper_timer.TIM_Prescaler = 72 - 1;                   //预分频系数
	
	TIM_TimeBaseInit(TIM3, &Stepper_timer);                //定时器参数初始化
	
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);            //清除中断标志位
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);             //使能定时器，中断方式（此处为更新中断）

	NVIC_InitTypeDef	NVICTIM3;
	NVICTIM3.NVIC_IRQChannel = TIM3_IRQn;
	NVICTIM3.NVIC_IRQChannelCmd = ENABLE;
	NVICTIM3.NVIC_IRQChannelPreemptionPriority = 1;         //中断优先级为1
	NVICTIM3.NVIC_IRQChannelSubPriority = 0;                //响应优先级为0
	NVIC_Init(&NVICTIM3);
		
}

static void init_Touch_Pin(void)                          //初始化接触感知F0/上电感知F1
{
		GPIO_InitTypeDef	gpioinit;
		gpioinit.GPIO_Mode = GPIO_Mode_IN_FLOATING;           //设为浮空输入
		gpioinit.GPIO_Pin = TOUCHED_PIN;
		gpioinit.GPIO_Speed = GPIO_Speed_2MHz;
	
		GPIO_Init(TOUCHED_PORT, &gpioinit);
		
		gpioinit.GPIO_Pin = ELEC_PIN;
		GPIO_Init(ELEC_PORT, &gpioinit);

}

static void init_GPIO_Port(void)                          //主轴，冷却泵，步进电机初始化
{
		GPIO_InitTypeDef	gpioinit;
		gpioinit.GPIO_Mode = GPIO_Mode_Out_PP;
		gpioinit.GPIO_Pin = SPINDLE_PIN;
		gpioinit.GPIO_Speed = GPIO_Speed_10MHz;
	
		GPIO_Init(SPINDLE_PORT, &gpioinit);                    //主轴初始
	
		gpioinit.GPIO_Pin = COOLANT_PIN;
		GPIO_Init(COOLANT_PORT, &gpioinit);                     //冷却泵初始化
	
		gpioinit.GPIO_Speed = GPIO_Speed_50MHz;
		gpioinit.GPIO_Pin = STEPPER_PIN |DIR_PIN;
		GPIO_Init(STEPPER_PORT, &gpioinit);                      //步进电机初始化
	
		GPIO_SetBits(SPINDLE_PORT,  SPINDLE_PIN);                //将主轴控制信号置1，因为其要通过光耦隔离，此时主轴停转
		GPIO_SetBits(COOLANT_PORT,  COOLANT_PIN);                 //将水泵控制信号置1，因为其要通过光耦隔离，此时水泵停转
	
		GPIO_SetBits(STEPPER_PORT, STEPPER_PIN | DIR_PIN);        //将步进电机方向控制和脉冲发生电路设为1
	
}

/*void InitializeEncoder(void)  
	{  
	          
	        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
	        TIM_ICInitTypeDef TIM_ICInitStructure;  
	  
	              
        GPIO_InitTypeDef GPIO_InitStructure;  
        //NVIC_InitTypeDef NVIC_InitStructure;  
  
         
	        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//??TIM1??  
	          
       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//??GPIOA??  
  
        GPIO_StructInit(&GPIO_InitStructure);  
          
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PA8 PA9????  
	        GPIO_Init(GPIOB, &GPIO_InitStructure);  
	  
	          
	        //NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel;  
	        //NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  
	        //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIMx_PRE_EMPTION_PRIORITY;  
        //NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIMx_SUB_PRIORITY;  
       //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
        //NVIC_Init(&NVIC_InitStructure);  
  
	          
        TIM_DeInit(ENCODER_TIMER);  
        TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);  
	  
	        TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling //??????????0,???  
       //TIM_TimeBaseStructure.TIM_Period = (4*ENCODER_PPR)-1;  //????????   
	        TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD-1;  
          
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//?????? T_dts = T_ck_int   
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM????  
        TIM_TimeBaseInit(ENCODER_TIMER, &TIM_TimeBaseStructure);  
  
	        TIM_EncoderInterfaceConfig(ENCODER_TIMER, TIM_EncoderMode_TI12,   
                                 TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//???????3  
        TIM_ICStructInit(&TIM_ICInitStructure);  
	        TIM_ICInitStructure.TIM_ICFilter =ICx_FILTER;//?????????  
        TIM_ICInit(ENCODER_TIMER, &TIM_ICInitStructure);  
	  
        // Clear all pending interrupts  
        TIM_ClearFlag(ENCODER_TIMER, TIM_FLAG_Update);//??TIM1??????  
        TIM_ITConfig(ENCODER_TIMER, TIM_IT_Update, ENABLE);  
  
        //ENC_Clear_Speed_Buffer();  
	  
	        //Reset counter  
	        TIM4->CNT = 0;  
        //CurrentCount = TIM1->CNT;  
  
	        TIM_Cmd(ENCODER_TIMER, ENABLE);  
  
	}  

s16   Enc_GetCount(void)  
{  
        static  u16   lastCount = 0;  
        u16  curCount = ENCODER_TIMER->CNT;//?????  
        s32 dAngle = curCount - lastCount;  
        if(dAngle >= MAX_COUNT)  
        {  
            dAngle -= ENCODER_TIM_PERIOD;  
        }  
        else if(dAngle < -MAX_COUNT)  
        {  
            dAngle += ENCODER_TIM_PERIOD;  
        }  
        lastCount = curCount;  
        return (s16)dAngle;  
} */ 



void HardWare_Init(void)                                    
{
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	init_xuanniu();
	init_EXIT();
	
	init_Timer();                              //初始化伺服运动时钟
	init_GPIO_Port();                          //主轴，冷却泵，步进电机初始化 
	init_detect();                             //初始化配置检测电路
	init_Touch_Pin();                          //初始化检测电路时钟(TIM4)，以及检测电路A1引脚电平
	
}

uint32_t SpeedBaseonSparkDetect(void)              //开路，火花，短路状态判别逻辑,并给出每秒钟进给脉冲步数
{	
	if(Syst->OpenRatio - Syst->ShortRatio > 30){
			GPIO_ResetBits(STEPPER_PORT, DIR_PIN);
			return Syst->OpenSpeed;
	}else if(Syst->ShortRatio - Syst->OpenRatio > 30){
			GPIO_SetBits(STEPPER_PORT, DIR_PIN);
			return Syst->SHortSpeed;
	}else{
			GPIO_ResetBits(STEPPER_PORT, DIR_PIN);
			return Syst->SparkSpeed;
	}
	
}

void SysInit(Sys_t *sy)               //系统硬件初始化/初始参数设定
{
	Syst = sy;
	HardWare_Init();
	
	DetectCounter = 0;
	sy->Acccelerate_Distance = 20000;
	sy->Auto = 0;
	sy->CoolantState = 0;
	sy->Decelerate_Distance = 20000;
	sy->Time_Change_Speed = 720000;
	sy->Delta_Speed = 500;
	sy->Entry_Speed = 600;
	sy->Motion_State = MOTION_STATE_STP;        //步进电机正向运动
	sy->Nominal_Speed = 10000;
	sy->OpenSpeed = 100;
	sy->SparkSpeed = 100;
	sy->SHortSpeed = 100;
	sy->TempDistance = 0;
	sy->Touched = 0;
	sy->flag = 0;	
}

//统计开路率、短路率
void TIM4_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	
	if(GPIO_ReadInputDataBit(DETECT_OPEN_PORT, DETECT_OPEN_PIN) == RESET)
	{       //如果成立
			Syst->OpenTimes ++;
	}
	else if(GPIO_ReadInputDataBit(DETECT_SHORT_PORT, DETECT_SHORT_PIN) == SET)
	{
			Syst->ShortTimes ++;
	}
	else
	{
			Syst->SparkTimes ++;
	}
	if(++DetectCounter > DETECT_CALCULATE_TIME / DETECT_TIME)
	{                        //DETECT_CALCULATE_TIME / DETECT_TIME==200*1000/10==20000次  每次TIM4中断周期为0.00001s  所以其为200ms统计一次三率
		Syst->OpenRatio = Syst->OpenTimes   * 100 / (DETECT_CALCULATE_TIME / DETECT_TIME);
		Syst->SparkRatio = Syst->SparkTimes * 100 / (DETECT_CALCULATE_TIME / DETECT_TIME);
		Syst->ShortRatio = Syst->ShortTimes * 100 / (DETECT_CALCULATE_TIME / DETECT_TIME);
	
		DetectCounter = 0;
		Syst->OpenTimes = 0;
		Syst->SparkTimes = 0;
		Syst->ShortTimes = 0;
	}
}


