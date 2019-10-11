#ifndef __SAMPLE_TIM_H
#define	__SAMPLE_TIM_H

#include "stm32f4xx.h"

#define SAMPLE_BASIC_TIM           		TIM6
#define SAMPLE_BASIC_TIM_CLK       		RCC_APB1Periph_TIM6

#define SAMPLE_BASIC_TIM_IRQn				TIM6_DAC_IRQn
#define SAMPLE_BASIC_TIM_IRQHandler    	TIM6_DAC_IRQHandler


//Òý½Å¶¨Òå
/*******************************************************/
#define OPEN_PIN                  GPIO_Pin_0                 
#define OPEN_GPIO_PORT            GPIOA                      
#define OPEN_GPIO_CLK             RCC_AHB1Periph_GPIOA

#define NOTSHORT_PIN                  GPIO_Pin_13                 
#define NOTSHORT_GPIO_PORT            GPIOC                      
#define NOTSHORT_GPIO_CLK             RCC_AHB1Periph_GPIOC
/*******************************************************/



void Sample_TIM_Configuration(uint32_t sampleFrequency);

#endif /* __SAMPLE_TIM_H */

