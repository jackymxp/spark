#include "Systems.h"
#include "led.h"
#include "delay.h"
#include "stm32f10x_exti.h"

#define DETECT_TIME 10
#define DETECT_TIMER_PSC 36
#define DETECT_TIMER_ARR (DETECT_TIME*72/DETECT_TIMER_PSC)
#define DETECT_CALCULATE_TIME		200 * 1000		// 200ms ͳ��һ�ο�·�𻨶�·��

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

static void Enable_Coolant(uint8_t NewState)                   //ˮ������/�رպ���
{
	if(NewState == 1){GPIO_ResetBits(COOLANT_PORT, COOLANT_PIN);}
	else{GPIO_SetBits(COOLANT_PORT, COOLANT_PIN);}
}
	

static void Enable_Spindle(uint8_t NewState)                  //��������/�رպ���
{
	if(NewState == 1){GPIO_ResetBits(SPINDLE_PORT, SPINDLE_PIN);}
	else{GPIO_SetBits(SPINDLE_PORT, SPINDLE_PIN);}
}



uint8_t Check_Elec_Touched(void)                      //�ϵ��֪״̬ģ��
{
	if(GPIO_ReadInputDataBit(ELEC_PORT, ELEC_PIN) == SET)//�ϵ緵��0
		return 0;
	else
	{
		if(GPIO_ReadInputDataBit(TOUCHED_PORT, TOUCHED_PIN) == SET) return 1;         //δ�ϵ�δ�Ӵ�����1
		else return 2;                                                                //δ�ϵ�Ӵ�����2
	}
}

void SystemScan(Sys_t *s)                                       //ȫϵͳɨ�裬���ܸ���ģ��
{	
	Enable_Spindle(s->SpindleState);                     //��������/�ر�
	Enable_Coolant(s->CoolantState);                      //ˮ������/�ر�
	
	//��ʹ���Զ��ӹ�����
	if(s->Auto == 0)
	{                                     //����Զ��ӹ����ܹر�                 
		Syst->Time_Change_Speed = 72000;                  //�ٶȸı�ʱ����
		//���Ӵ���֪��״̬
		Syst->Touched = Check_Elec_Touched();
		
		//δ�ϵ�Ӵ��ˣ����Զ�����һ��
		if(Syst->Touched == 2 && Syst->Motion_State == MOTION_STATE_STP)
		{  //����Ӵ���֪�������������ŷ��˶�ֹͣ������DEFAULT_GOBACK_DISTANCE�ľ���
			s->Motion_State = MOTION_STATE_BACK;                                      //�������
			s->GoBackDistanceifTouched = DEFAULT_GOBACK_DISTANCE;
		}
		
		if(s->Motion_State == MOTION_STATE_WORK){   //���������������Զ��ӹ��ŷ�����״̬�����Զ�ʧ��TIM3��TIM4��ʱ�����ķ���Ϊ����
			TIM_Cmd(TIM3, DISABLE);
			TIM_Cmd(TIM4, DISABLE);
			
			s->Motion_State = MOTION_STATE_STP;
		}
		
		if(s->Motion_State == MOTION_STATE_BACK )
		{     //������ڽӴ���֪�Ի���״̬��ͬʱ������ˣ��򿪶�ʱ��TIM3
			if(s->GoBackDistanceifTouched == DEFAULT_GOBACK_DISTANCE){
				s->CurrentSpeed = s->Entry_Speed;
				GPIO_SetBits(STEPPER_PORT, DIR_PIN);
				TIM_Cmd(TIM3, ENABLE);
			}
			
		}
		else 
		{		
			if(s->GoUp == 1 && s->Motion_State == MOTION_STATE_STP){          //��������������ҵ������ֹͣ״̬����ʼ����
				GPIO_SetBits(STEPPER_PORT, DIR_PIN);
				s->Motion_State = MOTION_STATE_START;
				TIM_Cmd(TIM3, ENABLE);
				
			}else if(s->GoDown == 1  && s->Motion_State == MOTION_STATE_STP){         //��������½����ҵ������ֹͣ״̬����ʼ����
				s->Motion_State = MOTION_STATE_START;
				GPIO_ResetBits(STEPPER_PORT, DIR_PIN);
				TIM_Cmd(TIM3, ENABLE);
				
			}else if(s->GoUp == 0 && s->GoDown == 0 && (s->Motion_State != MOTION_STATE_STP)){    //��������½���ť��û���£��ҵ�����ڹ���״̬����ʼ����
				s->Motion_State = MOTION_STATE_DEC;
				
			}
		}
		
		
	}
	
	//��������Զ��ӹ���ť�����Զ�ʹ��������ʱ������ʼ�ŷ��ӹ�
	else{                              
		Syst->Time_Change_Speed = 72000;
		if(s->Motion_State != MOTION_STATE_WORK){
			TIM_Cmd(TIM4, ENABLE);	//���ŵ�ͳ��
			TIM_Cmd(TIM3, ENABLE);		//���˶�
			s->Motion_State = MOTION_STATE_WORK;
		}
	}

}


static void init_detect(void)                          //��ʼ������·ʱ��(TIM4)���Լ�����·A1���ŵ�ƽ��ÿ���ж�����Ϊ0.00001s
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
	DetectGPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING;              //��������Ϊ����
	DetectGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	DetectGPIO.GPIO_Pin = DETECT_OPEN_PIN;                     //A0����  
	GPIO_Init(DETECT_OPEN_PORT, &DetectGPIO);
		
	DetectGPIO.GPIO_Pin = DETECT_SHORT_PIN;                    //E0����
	GPIO_Init(DETECT_SHORT_PORT, &DetectGPIO);
	
	DetectGPIO.GPIO_Pin = DETECT_BASE_PULSE_PIN;               //A1����
	DetectGPIO.GPIO_Mode = GPIO_Mode_Out_PP;                   //��������Ϊ�������
	GPIO_Init(DETECT_BASE_PULSE_PORT, &DetectGPIO);
	
	GPIO_SetBits(DETECT_BASE_PULSE_PORT, DETECT_BASE_PULSE_PIN);   //��A1������λһ��Ŀ��Ϊʹ������A0��E0���������������
	

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
	  DetectGPIO.GPIO_Mode = GPIO_Mode_IPU;              //��������Ϊ����
	  DetectGPIO.GPIO_Speed = GPIO_Speed_10MHz;
	  DetectGPIO.GPIO_Pin = GPIO_Pin_4;                     //B6����  
	  GPIO_Init(GPIOG, &DetectGPIO);
	
	  DetectGPIO.GPIO_Pin = GPIO_Pin_5;                     //B7����  
	  GPIO_Init(GPIOG, &DetectGPIO);

	
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource4);
	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource5);
	
	  EXTI_InitTypeDef EXTI_InitStructure;
  	EXTI_InitStructure.EXTI_Line=EXTI_Line4;	//KEY2
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	  
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

static void init_Timer(void)                              //��ʼ���ŷ��˶�ʱ��TIM3
{
	TIM_TimeBaseInitTypeDef	Stepper_timer;
	Stepper_timer.TIM_ClockDivision = TIM_CKD_DIV1;         //ʱ�ӷ�Ƶ����(��ʱ��ʱ��Ƶ�ʵ���ϵͳʱ��Ƶ��)
	Stepper_timer.TIM_CounterMode = TIM_CounterMode_Up;     //���ϼ����취
	Stepper_timer.TIM_Period = 10;                          //ԤװֵΪ10
	Stepper_timer.TIM_Prescaler = 72 - 1;                   //Ԥ��Ƶϵ��
	
	TIM_TimeBaseInit(TIM3, &Stepper_timer);                //��ʱ��������ʼ��
	
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);            //����жϱ�־λ
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);             //ʹ�ܶ�ʱ�����жϷ�ʽ���˴�Ϊ�����жϣ�

	NVIC_InitTypeDef	NVICTIM3;
	NVICTIM3.NVIC_IRQChannel = TIM3_IRQn;
	NVICTIM3.NVIC_IRQChannelCmd = ENABLE;
	NVICTIM3.NVIC_IRQChannelPreemptionPriority = 1;         //�ж����ȼ�Ϊ1
	NVICTIM3.NVIC_IRQChannelSubPriority = 0;                //��Ӧ���ȼ�Ϊ0
	NVIC_Init(&NVICTIM3);
		
}

static void init_Touch_Pin(void)                          //��ʼ���Ӵ���֪F0/�ϵ��֪F1
{
		GPIO_InitTypeDef	gpioinit;
		gpioinit.GPIO_Mode = GPIO_Mode_IN_FLOATING;           //��Ϊ��������
		gpioinit.GPIO_Pin = TOUCHED_PIN;
		gpioinit.GPIO_Speed = GPIO_Speed_2MHz;
	
		GPIO_Init(TOUCHED_PORT, &gpioinit);
		
		gpioinit.GPIO_Pin = ELEC_PIN;
		GPIO_Init(ELEC_PORT, &gpioinit);

}

static void init_GPIO_Port(void)                          //���ᣬ��ȴ�ã����������ʼ��
{
		GPIO_InitTypeDef	gpioinit;
		gpioinit.GPIO_Mode = GPIO_Mode_Out_PP;
		gpioinit.GPIO_Pin = SPINDLE_PIN;
		gpioinit.GPIO_Speed = GPIO_Speed_10MHz;
	
		GPIO_Init(SPINDLE_PORT, &gpioinit);                    //�����ʼ
	
		gpioinit.GPIO_Pin = COOLANT_PIN;
		GPIO_Init(COOLANT_PORT, &gpioinit);                     //��ȴ�ó�ʼ��
	
		gpioinit.GPIO_Speed = GPIO_Speed_50MHz;
		gpioinit.GPIO_Pin = STEPPER_PIN |DIR_PIN;
		GPIO_Init(STEPPER_PORT, &gpioinit);                      //���������ʼ��
	
		GPIO_SetBits(SPINDLE_PORT,  SPINDLE_PIN);                //����������ź���1����Ϊ��Ҫͨ��������룬��ʱ����ͣת
		GPIO_SetBits(COOLANT_PORT,  COOLANT_PIN);                 //��ˮ�ÿ����ź���1����Ϊ��Ҫͨ��������룬��ʱˮ��ͣת
	
		GPIO_SetBits(STEPPER_PORT, STEPPER_PIN | DIR_PIN);        //���������������ƺ����巢����·��Ϊ1
	
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
	
	init_Timer();                              //��ʼ���ŷ��˶�ʱ��
	init_GPIO_Port();                          //���ᣬ��ȴ�ã����������ʼ�� 
	init_detect();                             //��ʼ�����ü���·
	init_Touch_Pin();                          //��ʼ������·ʱ��(TIM4)���Լ�����·A1���ŵ�ƽ
	
}

uint32_t SpeedBaseonSparkDetect(void)              //��·���𻨣���·״̬�б��߼�,������ÿ���ӽ������岽��
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

void SysInit(Sys_t *sy)               //ϵͳӲ����ʼ��/��ʼ�����趨
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
	sy->Motion_State = MOTION_STATE_STP;        //������������˶�
	sy->Nominal_Speed = 10000;
	sy->OpenSpeed = 100;
	sy->SparkSpeed = 100;
	sy->SHortSpeed = 100;
	sy->TempDistance = 0;
	sy->Touched = 0;
	sy->flag = 0;	
}

//ͳ�ƿ�·�ʡ���·��
void TIM4_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	
	if(GPIO_ReadInputDataBit(DETECT_OPEN_PORT, DETECT_OPEN_PIN) == RESET)
	{       //�������
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
	{                        //DETECT_CALCULATE_TIME / DETECT_TIME==200*1000/10==20000��  ÿ��TIM4�ж�����Ϊ0.00001s  ������Ϊ200msͳ��һ������
		Syst->OpenRatio = Syst->OpenTimes   * 100 / (DETECT_CALCULATE_TIME / DETECT_TIME);
		Syst->SparkRatio = Syst->SparkTimes * 100 / (DETECT_CALCULATE_TIME / DETECT_TIME);
		Syst->ShortRatio = Syst->ShortTimes * 100 / (DETECT_CALCULATE_TIME / DETECT_TIME);
	
		DetectCounter = 0;
		Syst->OpenTimes = 0;
		Syst->SparkTimes = 0;
		Syst->ShortTimes = 0;
	}
}


