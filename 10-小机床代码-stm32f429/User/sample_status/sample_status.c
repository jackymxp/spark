 #include "./sample_status/sample_status.h"

 /**
  * @brief  基本定时器 TIMx,x[6,7]中断优先级配置
  * @param  无
  * @retval 无
  */
static void TIMx_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = SAMPLE_BASIC_TIM_IRQn; 	
		// 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
 *-----------------------------------------------------------------------------
 */
static void Sample_TIM_Mode_Config(uint32_t sampleFrequency)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	// 开启TIMx_CLK,x[6,7] 
  RCC_APB1PeriphClockCmd(SAMPLE_BASIC_TIM_CLK, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/		
  //当定时器从0计数到4999，即为5000次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Period = 1000 * 1000 / sampleFrequency - 1;       
	
	//定时器时钟源TIMxCLK = 2 * PCLK1  
  //				PCLK1 = HCLK / 4 
  //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=90MHz
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 90 - 1;	
	
	// 初始化定时器TIMx, x[2,3,4,5]
	TIM_TimeBaseInit(SAMPLE_BASIC_TIM, &TIM_TimeBaseStructure);
	
	// 清除定时器更新中断标志位
	TIM_ClearFlag(SAMPLE_BASIC_TIM, TIM_FLAG_Update);
	
	// 开启定时器更新中断
	TIM_ITConfig(SAMPLE_BASIC_TIM,TIM_IT_Update,ENABLE);
	
	// 使能定时器
	TIM_Cmd(SAMPLE_BASIC_TIM, ENABLE);	
}




/**
  * @brief  初始化读状态功能的引脚
  * @param  无
  * @retval 无
  */
static void Sample_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*开启按键GPIO口的时钟*/
	RCC_AHB1PeriphClockCmd(OPEN_GPIO_CLK | NOTSHORT_GPIO_CLK,ENABLE);
	
	/*选择按键的引脚*/
	GPIO_InitStructure.GPIO_Pin = OPEN_PIN; 
  
	/*设置引脚为输入模式*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  
	/*设置引脚不上拉也不下拉*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	/*使用上面的结构体初始化按键*/
	GPIO_Init(OPEN_GPIO_PORT, &GPIO_InitStructure);    
  
	/*选择按键的引脚*/
	GPIO_InitStructure.GPIO_Pin = NOTSHORT_PIN; 
  
	/*使用上面的结构体初始化按键*/
	GPIO_Init(NOTSHORT_GPIO_PORT, &GPIO_InitStructure);  
}

/**
  * @brief  初始化基本定时器定时，sampleFrequency中断频率
  * @param  无
  * @retval 无
  */
void Sample_TIM_Configuration(uint32_t sampleFrequency)
{
	Sample_GPIO_Config();
	TIMx_NVIC_Configuration();	
  
	Sample_TIM_Mode_Config(sampleFrequency);
}

/*********************************************END OF FILE**********************/
