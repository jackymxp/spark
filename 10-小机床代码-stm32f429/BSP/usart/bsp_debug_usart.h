#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f4xx.h"
#include <stdio.h>


#define DEBUG_USART                             USART1
#define RCC_APBxPeriphClockCmd					RCC_APB2PeriphClockCmd
#define DEBUG_USART_CLK                         RCC_APB2Periph_USART1
#define DEBUG_USART_BAUDRATE                    115200

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_RX_PIN                      GPIO_Pin_10
#define DEBUG_USART_RX_AF                       GPIO_AF_USART1
#define DEBUG_USART_RX_SOURCE                   GPIO_PinSource10

#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_TX_PIN                      GPIO_Pin_9
#define DEBUG_USART_TX_AF                       GPIO_AF_USART1
#define DEBUG_USART_TX_SOURCE                   GPIO_PinSource9


#define DEBUG_ON         	1
// Log define
#define INFO(fmt,arg...)           printf("<<-INFO->> "fmt"\n",##arg)
#define ERROR(fmt,arg...)          printf("<<-ERROR->> "fmt"\n",##arg)
#define printk(fmt,arg...)          do{\
                                         if(DEBUG_ON)\
                                         printf("<<-DEBUG->> %s-->>%d-->>"fmt"\n", __func__, __LINE__, ##arg);\
																					}while(0)


void Debug_USART_Config(void);





#endif /* __USART1_H */
