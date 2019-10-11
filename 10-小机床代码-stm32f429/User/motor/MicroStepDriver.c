#include "./motor/MicroStepDriver.h" 
#include <stdio.h>
#include <math.h>
#include "./led/bsp_led.h"

int stepPosition = 0;

/**

  * @brief  定时器中断优先级配置

  * @param  无

  * @retval 无

  */
static void TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
	// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = MSD_PULSE_TIM_IRQ; 	
	// 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	// 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**

  * @brief  初始化电机驱动用到的引脚

  * @param  无

  * @retval 无

  */
static void MSD_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //电机脉冲输出 GPIO 初始化
    MSD_PULSE_AHBxClock_FUN(MSD_PULSE_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  MSD_PULSE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MSD_PULSE_PORT, &GPIO_InitStructure);
      /*  连接 PXx 到 定时器输出通道*/
    GPIO_PinAFConfig(MSD_PULSE_PORT,MSD_PULSE_PIN_SOURCE,MSD_PULSE_PIN_AF);
    
    //电机方向输出 GPIO 初始化
    MSD_DIR_AHBxClock_FUN(MSD_DIR_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  MSD_DIR_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MSD_DIR_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(MSD_DIR_PORT,MSD_DIR_PIN);
    
    //电机使能输出 GPIO 初始化
    MSD_ENA_AHBxClock_FUN(MSD_ENA_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  MSD_ENA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MSD_ENA_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(MSD_ENA_PORT,MSD_ENA_PIN);
}




///*
// * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
// * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
// * 另外三个成员是通用定时器和高级定时器才有.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            都有
// *  TIM_CounterMode		   TIMx,x[6,7]没有，其他都有
// *  TIM_Period               都有
// *  TIM_ClockDivision        TIMx,x[6,7]没有，其他都有
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]才有
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */

/* ----------------   PWM信号 周期和占空比的计算--------------- */
// ARR ：自动重装载寄存器的值
// CLK_cnt：计数器的时钟，等于 Fck_int / (psc+1) = 180M/(psc+1)
// PWM 信号的周期 T = (ARR+1) * (1/CLK_cnt) = (ARR+1)*(PSC+1) / 72M
// 占空比P=CCR/(ARR+1)

static void TIM_Mode_Config(uint32_t pulseTime)
{
  // 开启定时器时钟,即内部时钟CK_INT=180M
	MSD_PULSE_TIM_APBxClock_FUN(MSD_PULSE_TIM_CLK,ENABLE);

    /*--------------------时基结构体初始化-------------------------*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    // 自动重装载寄存器的值，累计TIM_Period+1个周期后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Period= pulseTime - 1;	
	// 驱动CNT计数器的时钟 = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= 9 - 1;	
	// 时钟分频因子 ，配置死区时间时需要用到
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// 重复计数器的值，最大值为255
	//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// 初始化定时器
	TIM_TimeBaseInit(MSD_PULSE_TIM, &TIM_TimeBaseStructure);

	/*--------------------输出比较结构体初始化-------------------*/		
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	// 配置为PWM模式2
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	// 输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// 互补输出禁能
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; 
	// 设置占空比大小
	TIM_OCInitStructure.TIM_Pulse = (pulseTime>>1) - 1; //是高电平所占据的时间
	// 输出通道电平极性配置
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	// 输出通道空闲电平极性配置
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    
	MSD_PULSE_OCx_Init(MSD_PULSE_TIM, &TIM_OCInitStructure);
    //使能TIM1_CH1预装载寄存器
	MSD_PULSE_OCx_PreloadConfig(MSD_PULSE_TIM, TIM_OCPreload_Enable);
   

	//使能TIM1预装载寄存器
    TIM_ARRPreloadConfig(MSD_PULSE_TIM, ENABLE); 
    //设置中断源，只有溢出时才中断
    TIM_UpdateRequestConfig(MSD_PULSE_TIM,TIM_UpdateSource_Regular);
	// 清除中断标志位
	TIM_ClearITPendingBit(MSD_PULSE_TIM, TIM_IT_Update);
    // 使能中断
    TIM_ITConfig(MSD_PULSE_TIM, TIM_IT_Update, ENABLE);
	// 使能计数器
	TIM_Cmd(MSD_PULSE_TIM, ENABLE);
}
/**

  * @brief  初始化电机相关的外设

  * @param  无

  * @retval 无

  */
void Motor_Config(uint32_t pulseTime)
{
    MSD_GPIO_Config();
    
    TIM_NVIC_Config();

    TIM_Mode_Config(pulseTime);    
}
/**

  * @brief  驱动器紧急停止

  * @param  NewState：使能或者禁止

  * @retval 无

  */
void MSD_ENA(FunctionalState NewState)
{
    if(NewState)
    {
      //ENA失能，禁止驱动器输出
      GPIO_SetBits(MSD_ENA_PORT,MSD_ENA_PIN);
      //紧急停止标志位为真
      //status.out_ena = FALSE; 
      printf("\n\r驱动器禁止输出（脱机状态）此时电机为无保持力矩状态，可以手动旋转电机");        
    }
    else
    {
      //ENA使能
      GPIO_ResetBits(MSD_ENA_PORT,MSD_ENA_PIN);
      //紧急停止标志位为假
      //status.out_ena = TRUE; 
      printf("\n\r驱动器恢复运行，此时电机为保持力矩状态，此时串口指令可以正常控制电机");         
    }
    
}

/**

  * @brief  根据运动方向判断步进电机的运行位置

  * @param  inc 运动方向

  * @retval 无

  */
void MSD_StepCounter(signed char inc)
{
  //根据方向判断电机位置
  if(inc == CCW)
  {
    stepPosition--;
  }
  else
  {
    stepPosition++;
  }
}

/*********************************************END OF FILE**********************/
