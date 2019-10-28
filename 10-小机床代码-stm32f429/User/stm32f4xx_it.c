/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    06-March-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "./led/bsp_led.h"
#include "./key/bsp_exti.h"
#include "./usart/bsp_debug_usart.h"
#include "./touch/bsp_i2c_touch.h"
#include "./system/system.h"
#include "./sample_status/sample_status.h"
#include "./motor/MicroStepDriver.h" 

extern SystemState CurrState;
extern void GTP_TouchProcess(void);




/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/

/**
  * @}
  */ 
void KEY1_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
		// LED1 取反		
		//LED1_TOGGLE;
	
		switch(GPIO_ReadInputDataBit(KEY1_INT_GPIO_PORT, KEY1_INT_GPIO_PIN) + GPIO_ReadInputDataBit(KEY2_INT_GPIO_PORT, KEY2_INT_GPIO_PIN))
		{
			case 0: printf("开路状态\n"); break;
			case 1: printf("火花状态\n"); break;
			case 2: printf("短路状态\n"); break;
			default: printf("未知错误\n");
		}
		//printf("KEY1,,%d\n",GPIO_ReadInputDataBit(KEY1_INT_GPIO_PORT, KEY1_INT_GPIO_PIN));		
    //清除中断标志位
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
	}  
}
#if 0
void KEY2_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
		// LED2 取反		
		//LED2_TOGGLE;
		switch(GPIO_ReadInputDataBit(KEY1_INT_GPIO_PORT, KEY1_INT_GPIO_PIN) + GPIO_ReadInputDataBit(KEY2_INT_GPIO_PORT, KEY2_INT_GPIO_PIN))
		{
			case 0: printf("开路状态\n"); break;
			case 1: printf("火花状态\n"); break;
			case 2: printf("短路状态\n"); break;
			default: printf("未知错误\n");
		}
		//printf("KEY2,,%d\n",GPIO_ReadInputDataBit(KEY2_INT_GPIO_PORT,KEY2_INT_GPIO_PIN));	
		//清除中断标志位
		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);     
	}  
}
#endif
/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */	


void GTP_IRQHandler(void)
{
	if(EXTI_GetITStatus(GTP_INT_EXTI_LINE) != RESET) //确保是否产生了EXTI Line中断
	{
		GTP_TouchProcess(); 
		//Scan_System();			
		EXTI_ClearITPendingBit(GTP_INT_EXTI_LINE);     //清除中断标志位
	}  
}



void  SAMPLE_BASIC_TIM_IRQHandler (void)
{
	static uint32_t interrupt_count = 0;
	if ( TIM_GetITStatus( SAMPLE_BASIC_TIM, TIM_IT_Update) != RESET ) 
	{	
		interrupt_count++;
		//在这里读状态
		uint8_t res1 = GPIO_ReadInputDataBit(OPEN_GPIO_PORT, OPEN_PIN);           //0      0       1
		uint8_t res2 = GPIO_ReadInputDataBit(NOTSHORT_GPIO_PORT, NOTSHORT_PIN);  // 0      1       1
																				// 开      火花    短路
		switch((res2<<1) + res1)
		{
			case 0://开路
				//printf("开路\n");
				CurrState.DischargeCount.openCount++;
			break;
			
			case 2: //火花
				//printf("火花\n");
				CurrState.DischargeCount.sparkCount++;
			break;
			
			case 3://短路
				//printf("短路\n");
				CurrState.DischargeCount.shortCount++;
			break;
			
			default:
				printf("未知的放电类型\n");
			break;
		} 
		if(interrupt_count >= CurrState.DischargeCount.StaticsCount)
		{
			//printf("interrupt_count = %d\n",interrupt_count);
			//printf("open_count = %d\n",CurrState.DischargeCount.openCount);
			//printf("short_count = %d\n",CurrState.DischargeCount.shortCount);
			//printf("spart_count = %d\n",CurrState.DischargeCount.sparkCount);
			//在这里根据open_count   short_count    spart_count  的数量来判断是什么转态
			//这里只是根据了谁最大，就认为当前状态是什么状态。。。
			uint16_t temp = CurrState.DischargeCount.openCount > CurrState.DischargeCount.sparkCount  ? \
							CurrState.DischargeCount.openCount : CurrState.DischargeCount.sparkCount;
			
			uint16_t max_count = temp > CurrState.DischargeCount.shortCount ? \
								 temp : CurrState.DischargeCount.shortCount;
			
			if(max_count == CurrState.DischargeCount.sparkCount)
			{
				CurrState.DischargeState = SPARK_STATE;
				//printf("当前转态是火花状态\n");
			}
			
			else if(max_count == CurrState.DischargeCount.shortCount)
			{
				CurrState.DischargeState = SHORT_STATE;
				//printf("当前转态是短路状态\n");
			}
			else if(max_count == CurrState.DischargeCount.openCount)
			{
				CurrState.DischargeState = OPEN_STATE;
				//printf("当前转态是开路状态\n");
			}
			else
			{
				CurrState.DischargeState = ERROR_STATE;
				printf("统计错误\n");
			}
			interrupt_count = 0; 
			CurrState.DischargeCount.openCount = 0;
			CurrState.DischargeCount.shortCount = 0;
			CurrState.DischargeCount.sparkCount = 0;
		}
		
		//LED1_TOGGLE;
		TIM_ClearITPendingBit(SAMPLE_BASIC_TIM , TIM_IT_Update); 
	}		 	
}

#if 0
/**
  * @brief  产生脉冲定时器的中断响应程序，每走一步都会计算运动状态，每发出一个脉冲就会进入这个中断一次
  * @param  无
  * @retval 无
  */
void MSD_PULSE_TIM_IRQHandler(void)
{
	if (TIM_GetITStatus(MSD_PULSE_TIM, TIM_IT_Update) != RESET)
	{
		/* Clear MSD_PULSE_TIM Capture Compare1 interrupt pending bit*/
		TIM_ClearITPendingBit(MSD_PULSE_TIM, TIM_IT_Update);
		//这句话有待考察。。。。我们使用的是通道1。。。。设置脉宽的占空比
		//TIM_SetCompare1(MSD_PULSE_TIM, (CurrState.MotorState.MSDSpeed<<1));
		//设置脉宽频率
		TIM_SetAutoreload(MSD_PULSE_TIM , CurrState.MotorState.MSDSpeed);
	}
}

#endif




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
