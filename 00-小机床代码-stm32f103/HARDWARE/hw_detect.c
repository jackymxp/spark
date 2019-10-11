#include "hw_detect.h"

#define SHORT_GO_BACK_SPEED -30000
#define OPEN_AHEAD_SPEED		10000
#define PULSE_TOTAL_PER_TIMES	10000  //10ms TIM1

uint8_t CalculateBusyFlag;

uint16_t TotalCount;  //Count if Total open state, PULSE_TOTAL_PER_TIMES * 占空比

spark_ratio_t ratio_t;

Spark_t	RateDataBuffer[20]; 

RateHistoryDate_p_t	RatedataPtr;

uint8_t buffer_head;


static void Init_TIM1(void)
{
	TIM_TimeBaseInitTypeDef  TIM1_BaseStruct;
	TIM1_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM1_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM1_BaseStruct.TIM_Period = PULSE_TOTAL_PER_TIMES;
	TIM1_BaseStruct.TIM_Prescaler = 72 - 1;
	TIM1_BaseStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM1_BaseStruct);

	GPIO_InitTypeDef	GPIO_TIM1_CH3_Struct;
	GPIO_TIM1_CH3_Struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_TIM1_CH3_Struct.GPIO_Pin =GPIO_Pin_10;
	GPIO_TIM1_CH3_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_TIM1_CH3_Struct);	
	
	TIM_OCInitTypeDef	TIM1_PWM_Init_Struct;
	TIM1_PWM_Init_Struct.TIM_OCMode = TIM_OCMode_PWM2;
	TIM1_PWM_Init_Struct.TIM_OutputState = TIM_OutputState_Enable;
	TIM1_PWM_Init_Struct.TIM_Pulse = 20;
	TIM1_PWM_Init_Struct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(TIM1, &TIM1_PWM_Init_Struct);
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	TIM_SelectMasterSlaveMode(TIM1,TIM_MasterSlaveMode_Enable); 
	TIM_SelectOutputTrigger(TIM1,TIM_TRGOSource_OC3Ref); 
	
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	NVIC_InitTypeDef TIM1_NVIC_Init_Struct;
	TIM1_NVIC_Init_Struct.NVIC_IRQChannel = TIM1_UP_IRQn;
	TIM1_NVIC_Init_Struct.NVIC_IRQChannelCmd = ENABLE;
	TIM1_NVIC_Init_Struct.NVIC_IRQChannelPreemptionPriority = 0;
	TIM1_NVIC_Init_Struct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&TIM1_NVIC_Init_Struct);
	
}

static void Init_TIM2_TIM4(void)
{
	GPIO_InitTypeDef	GPIO_TIM2_ETR;
	TIM_DeInit(TIM2);
	TIM_DeInit(TIM4);
	
	GPIO_TIM2_ETR.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_TIM2_ETR.GPIO_Pin = GPIO_Pin_0;
	GPIO_TIM2_ETR.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA,&GPIO_TIM2_ETR);		//管脚配置PA0/TIM2_CH1_ETR
	
	GPIO_Init(GPIOE,&GPIO_TIM2_ETR);		//管脚配置PE0 /TIM4_ETR
	
	TIM_TimeBaseInitTypeDef	TIM2_InitBase;
	
	TIM2_InitBase.TIM_ClockDivision = TIM_CKD_DIV1;		//fDTS = f 72MHz
	TIM2_InitBase.TIM_CounterMode = TIM_CounterMode_Up;	
	TIM2_InitBase.TIM_Period = PULSE_TOTAL_PER_TIMES;
	TIM2_InitBase.TIM_Prescaler = 1 - 1;
	
	TIM_TimeBaseInit(TIM2, &TIM2_InitBase);
	TIM_TimeBaseInit(TIM4, &TIM2_InitBase);
	//滤波配置 防干扰
	TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF,TIM_ExtTRGPolarity_NonInverted, 0x06); //f = fDTS/2;N = 6; 
	TIM_ETRClockMode2Config(TIM4, TIM_ExtTRGPSC_OFF,TIM_ExtTRGPolarity_NonInverted, 0x06);	//	 about 3MHz

	TIM_SetCounter(TIM2, 0);
	TIM_SetCounter(TIM4, 0);
	
	TIM_ITConfig(TIM2,TIM_IT_Update, DISABLE);
	TIM_ITConfig(TIM4,TIM_IT_Update, DISABLE);
	
	//从模式 定时器1输出有效电平时定时器2和4的Gate位被设置为1,开始计时；无效电平暂停计时
	TIM_SelectMasterSlaveMode(TIM2,TIM_MasterSlaveMode_Enable); 
	TIM_SelectMasterSlaveMode(TIM4,TIM_MasterSlaveMode_Enable); 
	
	TIM_ITRxExternalClockConfig(TIM2, TIM_TS_ITR0);
	TIM_ITRxExternalClockConfig(TIM4, TIM_TS_ITR0);
	
	TIM_SelectSlaveMode(TIM2,TIM_SlaveMode_Gated);
	TIM_SelectSlaveMode(TIM4,TIM_SlaveMode_Gated);
}

//配置为定时器5一直发出1MHz PWM波 作为检测电路基准调制脉冲
static void Init_TIM5(void)
{
	TIM_DeInit(TIM5);	
	GPIO_InitTypeDef	GPIO_TIM5_CH2_Struct;
	GPIO_TIM5_CH2_Struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_TIM5_CH2_Struct.GPIO_Pin =GPIO_Pin_1;
	GPIO_TIM5_CH2_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_TIM5_CH2_Struct);							//管脚配置PA9/TIM1_CH2
	
	TIM_TimeBaseInitTypeDef	TIM5_Init_Struct;
	TIM5_Init_Struct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM5_Init_Struct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM5_Init_Struct.TIM_Period = 36;
	TIM5_Init_Struct.TIM_Prescaler = 2 - 1;
	TIM_TimeBaseInit(TIM5, &TIM5_Init_Struct);
	
	TIM_OCInitTypeDef	TIM5_PWM_Init_Struct;
	TIM5_PWM_Init_Struct.TIM_OCMode = TIM_OCMode_PWM2;
	TIM5_PWM_Init_Struct.TIM_OutputState = TIM_OutputState_Enable;
	TIM5_PWM_Init_Struct.TIM_Pulse = 18;
	TIM5_PWM_Init_Struct.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_ITConfig(TIM5,TIM_IT_Update,DISABLE);
	TIM_OC2Init(TIM5, &TIM5_PWM_Init_Struct);
	
	TIM_ARRPreloadConfig(TIM5, ENABLE);
	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);
	TIM_Cmd(TIM5, ENABLE);
}

static void StoreRateDatatoBuffer(void)
{
		RateDataBuffer[buffer_head].Op = ratio_t.open_rate;
		RateDataBuffer[buffer_head].Sp = ratio_t.spark_rate;
		RateDataBuffer[buffer_head].Sh = ratio_t.short_rate;
		RatedataPtr.Current = buffer_head;
		if(++buffer_head == 20) buffer_head = 0;
}

static void Caculate_Ratio(void)
{
	int16_t sptimes,optimes;
	optimes = ratio_t.opentimes;
	sptimes = ratio_t.sparktimes;
	
	if(sptimes < 0) sptimes = 0;
	//current ratio 
	ratio_t.open_rate = optimes * 100 / TotalCount;
	ratio_t.spark_rate = sptimes * 100 / TotalCount;
	ratio_t.short_rate = 100 - ratio_t.open_rate - ratio_t.spark_rate;
	//Store the RatioData to Buffer
	StoreRateDatatoBuffer();
	//report ratio
	if(ratio_t.ReportFlag == 1)
	{
		ratio_t.ReportOpenRate += ratio_t.open_rate;
		ratio_t.ReportSparkRate += ratio_t.spark_rate;
		ratio_t.ReportShortRate += ratio_t.short_rate;
	}
}

//更新中断内 取出定时器2和4的计数值
void TIM1_UP_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	if(CalculateBusyFlag) return;
	//TIM2->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
	//TIM4->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
	CalculateBusyFlag = 1;
	ratio_t.opentimes = TIM4->CNT;
	ratio_t.sparktimes = (TIM2->CNT - ratio_t.opentimes);
	TIM2->CNT = 0;
	TIM4->CNT = 0;
	Caculate_Ratio();
	CalculateBusyFlag = 0;
}

void Init_Spark_Detect()
{
	Init_TIM2_TIM4();
	Init_TIM5();
	Init_TIM1();
	ratio_t.opentimes = 0;
	ratio_t.sparktimes = 0;
	ratio_t.shorttimes = 0;
	buffer_head = 0;
	RatedataPtr.PtrHead = &RateDataBuffer[0];
	Caculate_Ratio();
	
}

void Detect_Cmd(char NewState)
{
	if(NewState == 1){
		TIM2->CNT = 0;
		TIM4->CNT = 0;
		TIM_Cmd(TIM1,ENABLE);
		TIM_Cmd(TIM2, ENABLE);
		TIM_Cmd(TIM4, ENABLE);
		CalculateBusyFlag = 0;
	}else{
		TIM_Cmd(TIM1, DISABLE);
		TIM_Cmd(TIM2, DISABLE);
		TIM_Cmd(TIM4, DISABLE);
		TIM2->CNT = 0;
		TIM4->CNT = 0;
		ratio_t.open_rate = 0;
		ratio_t.spark_rate = 0;
		ratio_t.short_rate = 0;
	}
}

RateHistoryDate_p_t *GetRateData(void)
{
	return &RatedataPtr;
}

void SetTotalCount(uint8_t ratio_percent)	
{
	TotalCount = (PULSE_TOTAL_PER_TIMES - 20) * ratio_percent / 100;
}

uint32_t ReportGetRatioRate(void)
{
	ratio_t.ReportFlag = 0;
	uint32_t totalRate = ratio_t.ReportOpenRate + ratio_t.ReportShortRate + ratio_t.ReportSparkRate;
	uint8_t RO = ratio_t.ReportOpenRate * 100 /  totalRate;
	uint8_t RS = ratio_t.ReportSparkRate * 100 / totalRate;
	uint8_t Rs = ratio_t.ReportShortRate * 100 / totalRate;

		ratio_t.ReportOpenRate = 0;
		ratio_t.ReportSparkRate = 0;
		ratio_t.ReportShortRate = 0;
	
	ratio_t.ReportFlag = 1;
	
	return (((uint32_t)RO << 16) + ((uint32_t)RS << 8) + ((uint32_t)Rs));
}

