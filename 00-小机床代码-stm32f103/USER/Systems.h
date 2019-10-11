#ifndef SYSTEMS_H
#define SYSTEMS_H	

#include "stm32f10x.h"
#include "GPIO_Config.h"
#include "stm32f10x_exti.h"


	#define COOLANT_PORT	GPIOE
	#define SPINDLE_PORT	GPIOE
	
	#define COOLANT_PIN		GPIO_Pin_2
	#define SPINDLE_PIN		GPIO_Pin_3

	#define TOUCHED_PORT	GPIOF
	#define TOUCHED_PIN		GPIO_Pin_0
	#define ELEC_PORT			GPIOF
	#define ELEC_PIN			GPIO_Pin_1

	#define STEPPER_PORT	GPIOC
	#define STEPPER_PIN		GPIO_Pin_6
	#define DIR_PIN				GPIO_Pin_7
	
	#define DETECT_OPEN_PORT	GPIOA
	#define DETECT_SHORT_PORT	GPIOE
	#define DETECT_OPEN_PIN		GPIO_Pin_0
	#define DETECT_SHORT_PIN	GPIO_Pin_0
	#define DETECT_BASE_PULSE_PORT 	GPIOA
	#define DETECT_BASE_PULSE_PIN		GPIO_Pin_1
	
#define MOTION_STATE_ACC 1
#define MOTION_STATE_NOM	2
#define MOTION_STATE_DEC	3       //步进电机处于减速状态
#define MOTION_STATE_STP	4   //步进电机处于停止状态
#define MOTION_STATE_START	5    //非自动加工状态下伺服进给状态

#define MOTION_STATE_BACK	6    //步进电机反向运动

#define MOTION_STATE_WORK 7      //步进电机自动加工伺服进给状态

#define DEFAULT_GOBACK_DISTANCE 500

typedef struct {
	uint8_t SysState;
	uint8_t Auto;
	
	uint32_t TempDistance;
	uint32_t CurrentSpeed;
	
	uint32_t Entry_Speed;
	uint32_t End_Speed;
	
	uint32_t Nominal_Speed;
	uint32_t Delta_Speed;
	uint32_t Time_Change_Speed;
	
	uint32_t deltaTime;
	
	uint8_t OpenSpeed;
	uint8_t SparkSpeed;
	uint8_t SHortSpeed;
	
	int32_t Position;
	int32_t Acccelerate_Distance;
	int32_t Decelerate_Distance;
	uint32_t GoBackDistanceifTouched;
	uint8_t Motion_State;

	uint8_t OpenRatio;
	uint8_t SparkRatio;
	uint8_t ShortRatio;
		
	uint32_t OpenTimes;
	uint32_t SparkTimes;
	uint32_t ShortTimes;
	
	uint8_t SpindleState;
	uint8_t CoolantState;
	
	uint8_t GoUp;
	uint8_t GoDown;	// 1有效
	uint8_t StepperDir;
	
	uint8_t Touched;	// 1接触/0上电
	uint8_t flag;
}	Sys_t;



void SysInit(Sys_t *sy);



void SystemScan(Sys_t *s);

void HardWare_Init(void);

uint8_t Check_Elec_Touched(void);

uint32_t SpeedBaseonSparkDetect(void);

void SetTimPscArrSpeed(uint32_t Speed); 

#endif
