#include "./motor/MicroStepDriver.h" 
#include <stdio.h>
#include <math.h>
#include "./led/bsp_led.h"

int stepPosition = 0;

/**

  * @brief  ��ʱ���ж����ȼ�����

  * @param  ��

  * @retval ��

  */
static void TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
	// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = MSD_PULSE_TIM_IRQ; 	
	// ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	// ���������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**

  * @brief  ��ʼ����������õ�������

  * @param  ��

  * @retval ��

  */
static void MSD_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //���������� GPIO ��ʼ��
    MSD_PULSE_AHBxClock_FUN(MSD_PULSE_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  MSD_PULSE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MSD_PULSE_PORT, &GPIO_InitStructure);
      /*  ���� PXx �� ��ʱ�����ͨ��*/
    GPIO_PinAFConfig(MSD_PULSE_PORT,MSD_PULSE_PIN_SOURCE,MSD_PULSE_PIN_AF);
    
    //���������� GPIO ��ʼ��
    MSD_DIR_AHBxClock_FUN(MSD_DIR_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  MSD_DIR_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MSD_DIR_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(MSD_DIR_PORT,MSD_DIR_PIN);
    
    //���ʹ����� GPIO ��ʼ��
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
// * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
// * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
// * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            ����
// *  TIM_CounterMode		   TIMx,x[6,7]û�У���������
// *  TIM_Period               ����
// *  TIM_ClockDivision        TIMx,x[6,7]û�У���������
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]����
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */

/* ----------------   PWM�ź� ���ں�ռ�ձȵļ���--------------- */
// ARR ���Զ���װ�ؼĴ�����ֵ
// CLK_cnt����������ʱ�ӣ����� Fck_int / (psc+1) = 180M/(psc+1)
// PWM �źŵ����� T = (ARR+1) * (1/CLK_cnt) = (ARR+1)*(PSC+1) / 72M
// ռ�ձ�P=CCR/(ARR+1)

static void TIM_Mode_Config(uint32_t pulseTime)
{
  // ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=180M
	MSD_PULSE_TIM_APBxClock_FUN(MSD_PULSE_TIM_CLK,ENABLE);

    /*--------------------ʱ���ṹ���ʼ��-------------------------*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    // �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1�����ں����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period= pulseTime - 1;	
	// ����CNT��������ʱ�� = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= 9 - 1;	
	// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// �ظ���������ֵ�����ֵΪ255
	//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// ��ʼ����ʱ��
	TIM_TimeBaseInit(MSD_PULSE_TIM, &TIM_TimeBaseStructure);

	/*--------------------����ȽϽṹ���ʼ��-------------------*/		
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	// ����ΪPWMģʽ2
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	// ���ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// �����������
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; 
	// ����ռ�ձȴ�С
	TIM_OCInitStructure.TIM_Pulse = (pulseTime>>1) - 1; //�Ǹߵ�ƽ��ռ�ݵ�ʱ��
	// ���ͨ����ƽ��������
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	// ���ͨ�����е�ƽ��������
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    
	MSD_PULSE_OCx_Init(MSD_PULSE_TIM, &TIM_OCInitStructure);
    //ʹ��TIM1_CH1Ԥװ�ؼĴ���
	MSD_PULSE_OCx_PreloadConfig(MSD_PULSE_TIM, TIM_OCPreload_Enable);
   

	//ʹ��TIM1Ԥװ�ؼĴ���
    TIM_ARRPreloadConfig(MSD_PULSE_TIM, ENABLE); 
    //�����ж�Դ��ֻ�����ʱ���ж�
    TIM_UpdateRequestConfig(MSD_PULSE_TIM,TIM_UpdateSource_Regular);
	// ����жϱ�־λ
	TIM_ClearITPendingBit(MSD_PULSE_TIM, TIM_IT_Update);
    // ʹ���ж�
    TIM_ITConfig(MSD_PULSE_TIM, TIM_IT_Update, ENABLE);
	// ʹ�ܼ�����
	TIM_Cmd(MSD_PULSE_TIM, ENABLE);
}
/**

  * @brief  ��ʼ�������ص�����

  * @param  ��

  * @retval ��

  */
void Motor_Config(uint32_t pulseTime)
{
    MSD_GPIO_Config();
    
    TIM_NVIC_Config();

    TIM_Mode_Config(pulseTime);    
}
/**

  * @brief  ����������ֹͣ

  * @param  NewState��ʹ�ܻ��߽�ֹ

  * @retval ��

  */
void MSD_ENA(FunctionalState NewState)
{
    if(NewState)
    {
      //ENAʧ�ܣ���ֹ���������
      GPIO_SetBits(MSD_ENA_PORT,MSD_ENA_PIN);
      //����ֹͣ��־λΪ��
      //status.out_ena = FALSE; 
      printf("\n\r��������ֹ������ѻ�״̬����ʱ���Ϊ�ޱ�������״̬�������ֶ���ת���");        
    }
    else
    {
      //ENAʹ��
      GPIO_ResetBits(MSD_ENA_PORT,MSD_ENA_PIN);
      //����ֹͣ��־λΪ��
      //status.out_ena = TRUE; 
      printf("\n\r�������ָ����У���ʱ���Ϊ��������״̬����ʱ����ָ������������Ƶ��");         
    }
    
}

/**

  * @brief  �����˶������жϲ������������λ��

  * @param  inc �˶�����

  * @retval ��

  */
void MSD_StepCounter(signed char inc)
{
  //���ݷ����жϵ��λ��
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
