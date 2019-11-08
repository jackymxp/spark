#include "./system/system.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./motor/MicroStepDriver.h"




SystemState CurrState =
{
	.SpindleState = DISABLE, //主轴状态
	.CoolantState = DISABLE,  //水泵状态
	

	.MotorState = {.MSDState = MSD_STOP,  .MSDdir = MSD_DOWN,  .MSDSpeed = OPENSPEED, .MSDStep = OPENSTEP, .MSDMode = MSD_LASTING,},
	
	.DischargeCount = {.interruptFrequency = 1000, .StaticsCount = 50, .openCount = 0, .shortCount = 0, .sparkCount = 0},
	.DischargeState = OPEN_STATE //默认为开路状态
};

void SpindleAndCoolant_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //电机方向输出 GPIO 初始化
    SPINDLE_AHBxClock_FUN(SPINDLE_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  SPINDLE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPINDLE_PORT, &GPIO_InitStructure);
	//初始化的时候，要保证它不是转的
	GPIO_SetBits(SPINDLE_PORT,SPINDLE_PIN);
    
    
    //电机使能输出 GPIO 初始化
    COOLANT_AHBxClock_FUN(COOLANT_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  COOLANT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(COOLANT_PORT, &GPIO_InitStructure);
	//初始化的时候，要保证它不是转的
    GPIO_SetBits(COOLANT_PORT,COOLANT_PIN);
}







void Spindle_Cmd(FunctionalState NewState)
{
	/* Check the parameters */
	assert_param(IS_FUNCTIONAL_STATE(NewState));
	if(NewState == ENABLE)
	{
		GPIO_ResetBits(SPINDLE_PORT,SPINDLE_PIN);
	}
	else
	{
		GPIO_SetBits(SPINDLE_PORT,SPINDLE_PIN);
	}
}

void Coolant_Cmd(FunctionalState NewState)
{
	/* Check the parameters */
	assert_param(IS_FUNCTIONAL_STATE(NewState));
	if(NewState == ENABLE)
	{
		GPIO_ResetBits(COOLANT_PORT,COOLANT_PIN);
	}
	else
	{
		GPIO_SetBits(COOLANT_PORT,COOLANT_PIN);
	}
}


void Scan_System(void)
{
	Coolant_Cmd(CurrState.CoolantState);
	Spindle_Cmd(CurrState.SpindleState);
	
	//非自动加工
	if(CurrState.AutoWorking == DISABLE)
	{
		uint16_t currentSpeed = CurrState.MotorState.MSDSpeed;
		if(CurrState.MotorState.MSDState == MSD_RUNNING)
		{
			if(currentSpeed != ADJUSTSPEED)
			{//这句话只执行一次就可以了，怎么搞。
				CurrState.MotorState.MSDSpeed = ADJUSTSPEED;
				MotorSetSpeed(CurrState.MotorState.MSDSpeed);	
			}
			TIM_Cmd(MSD_PULSE_TIM, ENABLE);
			//printf("CurrState.MotorState.MSDdir = %d\n",CurrState.MotorState.MSDdir);
			DIR(CurrState.MotorState.MSDdir);

		}
		else //步进电机停止
		{
			TIM_Cmd(MSD_PULSE_TIM, DISABLE);
		}
	}

	
	//开始自动加工
	else      //(CurrState.AutoWorking == ENABLE)
	{
		uint16_t currentSpeed = CurrState.MotorState.MSDSpeed;
		
		if(CurrState.MotorState.MSDStep > 0)
			Motor_Cmd(ENABLE);
		else
			Motor_Cmd(DISABLE);
		
		//判断转态
		switch(CurrState.DischargeState)
		{
			case SHORT_STATE:
				if(currentSpeed !=  SHORTSPEED)
				{//这句话只执行一次就可以了，怎么搞。
					CurrState.MotorState.MSDSpeed = SHORTSPEED;
					MotorSetSpeed(CurrState.MotorState.MSDSpeed);			
				}
				CurrState.MotorState.MSDStep = SHORTSTEP;
				DIR(MSD_UP);
			break;
				
			case OPEN_STATE:
				if(currentSpeed !=  OPENSPEED)
				{//这句话只执行一次就可以了，怎么搞。
					CurrState.MotorState.MSDSpeed = OPENSPEED;
					MotorSetSpeed(CurrState.MotorState.MSDSpeed);			
				}
				CurrState.MotorState.MSDStep = OPENSTEP;
				DIR(MSD_DOWN);
			break;
			
			case SPARK_STATE:
				if(currentSpeed !=  SPARKSPEED)
				{//这句话只执行一次就可以了，怎么搞。
					CurrState.MotorState.MSDSpeed = SPARKSPEED;
					MotorSetSpeed(CurrState.MotorState.MSDSpeed);				
				}
				CurrState.MotorState.MSDStep = SPARKSTEP;
				DIR(MSD_DOWN);
			break;
			
			case ERROR_STATE:
				LED_RED;
			break;
		}		
	}
}

