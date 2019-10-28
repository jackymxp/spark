#ifndef __BSP_ADVANCETIME_H
#define __BSP_ADVANCETIME_H


#include "stm32f4xx.h"
#include "./system/system.h"


// ��ʹ�ò�ͬ�Ķ�ʱ����ʱ�򣬶�Ӧ��GPIO�ǲ�һ���ģ����Ҫע��
// ��������ʹ�ö�ʱ��TIM2
#define            MSD_PULSE_TIM                    TIM2
#define            MSD_PULSE_TIM_APBxClock_FUN      RCC_APB1PeriphClockCmd
#define            MSD_PULSE_TIM_CLK                RCC_APB1Periph_TIM2
// ��ʱ�����PWMͨ����PA0��ͨ��1
#define            MSD_PULSE_OCx_Init               TIM_OC1Init
#define            MSD_PULSE_OCx_PreloadConfig      TIM_OC1PreloadConfig
// ��ʱ���ж�
#define            MSD_PULSE_TIM_IRQ                TIM2_IRQn
#define            MSD_PULSE_TIM_IRQHandler         TIM2_IRQHandler

// PWM �źŵ�Ƶ�� F = TIM_CLK/{(ARR+1)*(PSC+1)}
#define            MSD_PULSE_TIM_PERIOD             (100-1)
#define            MSD_PULSE_TIM_PSC                (9-1)

// ��������������ͨ��
#define            MSD_PULSE_AHBxClock_FUN          RCC_AHB1PeriphClockCmd
#define            MSD_PULSE_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define            MSD_PULSE_PORT                   GPIOA
#define            MSD_PULSE_PIN                    GPIO_Pin_5
#define            MSD_PULSE_PIN_AF                 GPIO_AF_TIM2
#define            MSD_PULSE_PIN_SOURCE             GPIO_PinSource5

// ��������������
#define            MSD_DIR_AHBxClock_FUN            RCC_AHB1PeriphClockCmd
#define            MSD_DIR_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define            MSD_DIR_PORT                     GPIOB
#define            MSD_DIR_PIN                      GPIO_Pin_13

// ����������ʹ������
#define            MSD_ENA_AHBxClock_FUN            RCC_AHB1PeriphClockCmd
#define            MSD_ENA_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define            MSD_ENA_PORT                     GPIOB
#define            MSD_ENA_PIN                      GPIO_Pin_12


#define DIR(a)	if (a == MSD_UP)	\
					GPIO_ResetBits(MSD_DIR_PORT,MSD_DIR_PIN);\
					else		\
					GPIO_SetBits(MSD_DIR_PORT,MSD_DIR_PIN)
                    
/**************************��������********************************/
void Motor_Config(void);
void MSD_ENA(FunctionalState NewState);
void MotorSetDir(Dir direction);
void Motor_Cmd(FunctionalState NewState);
void MotorSetSpeed(uint32_t speed);					
					
#endif	/* __BSP_ADVANCETIME_H */


