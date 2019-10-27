 #include "./sample_status/sample_status.h"

 /**
  * @brief  ������ʱ�� TIMx,x[6,7]�ж����ȼ�����
  * @param  ��
  * @retval ��
  */
static void TIMx_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = SAMPLE_BASIC_TIM_IRQn; 	
		// ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // ���������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode			 TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 */
static void Sample_TIM_Mode_Config(uint32_t sampleFrequency)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	// ����TIMx_CLK,x[6,7] 
  RCC_APB1PeriphClockCmd(SAMPLE_BASIC_TIM_CLK, ENABLE); 

  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  //����ʱ����0������4999����Ϊ5000�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Period = 1000 * 1000 / sampleFrequency - 1;       
	
	//��ʱ��ʱ��ԴTIMxCLK = 2 * PCLK1  
  //				PCLK1 = HCLK / 4 
  //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=90MHz
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 90 - 1;	
	
	// ��ʼ����ʱ��TIMx, x[2,3,4,5]
	TIM_TimeBaseInit(SAMPLE_BASIC_TIM, &TIM_TimeBaseStructure);
	
	// �����ʱ�������жϱ�־λ
	TIM_ClearFlag(SAMPLE_BASIC_TIM, TIM_FLAG_Update);
	
	// ������ʱ�������ж�
	TIM_ITConfig(SAMPLE_BASIC_TIM,TIM_IT_Update,ENABLE);
	
	// ʹ�ܶ�ʱ��
	TIM_Cmd(SAMPLE_BASIC_TIM, ENABLE);	
}




/**
  * @brief  ��ʼ����״̬���ܵ�����
  * @param  ��
  * @retval ��
  */
static void Sample_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*��������GPIO�ڵ�ʱ��*/
	RCC_AHB1PeriphClockCmd(OPEN_GPIO_CLK | NOTSHORT_GPIO_CLK,ENABLE);
	
	/*ѡ�񰴼�������*/
	GPIO_InitStructure.GPIO_Pin = OPEN_PIN; 
  
	/*��������Ϊ����ģʽ*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  
	/*�������Ų�����Ҳ������*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	/*ʹ������Ľṹ���ʼ������*/
	GPIO_Init(OPEN_GPIO_PORT, &GPIO_InitStructure);    
  
	/*ѡ�񰴼�������*/
	GPIO_InitStructure.GPIO_Pin = NOTSHORT_PIN; 
  
	/*ʹ������Ľṹ���ʼ������*/
	GPIO_Init(NOTSHORT_GPIO_PORT, &GPIO_InitStructure);  
}

/**
  * @brief  ��ʼ��������ʱ����ʱ��sampleFrequency�ж�Ƶ��
  * @param  ��
  * @retval ��
  */
void Sample_TIM_Configuration(uint32_t sampleFrequency)
{
	Sample_GPIO_Config();
	TIMx_NVIC_Configuration();	
  
	Sample_TIM_Mode_Config(sampleFrequency);
}

/*********************************************END OF FILE**********************/
