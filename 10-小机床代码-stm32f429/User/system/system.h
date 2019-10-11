#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "stm32f4xx.h"

// 主轴引脚
#define            SPINDLE_AHBxClock_FUN            RCC_AHB1PeriphClockCmd
#define            SPINDLE_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define            SPINDLE_PORT                     GPIOB
#define            SPINDLE_PIN                      GPIO_Pin_4

// 冷却水泵引脚
#define            COOLANT_AHBxClock_FUN            RCC_AHB1PeriphClockCmd
#define            COOLANT_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define            COOLANT_PORT                     GPIOB
#define            COOLANT_PIN                      GPIO_Pin_15





typedef enum{OPENSPEED = 10000, SPARKSPEED = 60000, SHORTSPEED = 2000, ADJUSTSPEED = 150,} WorkSpeed;
typedef enum{OPENSTEP = 1,  SPARKSTEP= 1,   SHORTSTEP = 1, ADJUSTSTEP = 150} WorkStep;


typedef enum{SHORT_STATE = 1, OPEN_STATE = 2, SPARK_STATE = 3, ERROR_STATE = 4, } SystemDischargeState;
typedef enum{MSD_STOP = 0, MSD_RUNNING = 1, } RunningStatus;
typedef enum{MSD_DOWN = 0, MSD_UP = 1, } Dir;
typedef enum{MSD_LASTING = 0,  MSD_PLUSE = 1,} Mode;



typedef struct{
	RunningStatus  MSDState;
	Dir            MSDdir;
	WorkSpeed      MSDSpeed;
	WorkStep	   MSDStep;
	Mode		   MSDMode;
}Motor;


typedef struct{
	uint16_t  interruptFrequency;  //进入中断的频率
	uint16_t  StaticsCount;    //要进入多少次中断之后统计当前状态
	uint16_t  openCount;
	uint16_t  shortCount;
	uint16_t  sparkCount;	
}DischargeStatist;

typedef struct
{
	FunctionalState  SpindleState;//主轴状态
	FunctionalState  CoolantState;//水泵状态
	
	Motor      MotorState;
	
	DischargeStatist      DischargeCount;
	SystemDischargeState  DischargeState;//火花状态 短路  开路  火花状态
	 		  
	FunctionalState  AutoWorking;
	
	
} SystemState;



void Scan_System(void);
void SpindleAndCoolant_GPIO_Config(void);
#endif  /*   _SYSTEM_H  */

