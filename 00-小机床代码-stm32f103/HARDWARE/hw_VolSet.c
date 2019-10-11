#include "hw_VolSet.h"
#include "delay.h"

typedef struct {
		uint16_t 	vH;	//mV
		uint16_t	vL;	//mV
} RefVol_t;

RefVol_t	Ref;

static void Dac1_and_Dac2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitType;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	  //??DAC???? 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 // ????
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		 //????
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4)	;//PA.4 ???
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 // ????
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		 //????
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_5)	;//PA.5 ???
	
	DAC_InitType.DAC_Trigger=DAC_Trigger_None;	//??????? TEN1=0
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//???????
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//???????
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	//DAC1?????? BOFF1=1
  DAC_Init(DAC_Channel_1,&DAC_InitType);	 //???DAC??1
	DAC_Init(DAC_Channel_2, &DAC_InitType);	//Init on DAC2 PA5

	DAC_Cmd(DAC_Channel_1, ENABLE);  //??DAC1	PA4
  DAC_Cmd(DAC_Channel_2, ENABLE);		//DAC2 PA5
	
  DAC_SetChannel1Data(DAC_Align_12b_R,0); //12??????????DAC?
	DAC_SetChannel2Data(DAC_Align_12b_R,0);

}

static void Init_ADC1(void)
{
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1	, ENABLE );	  //??ADC1????
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //??ADC????6 72M/6=12,ADC????????14M

	//PC10 ??????????                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//??????
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //??ADC1 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC????:ADC1?ADC2???????
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//????????????
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//?????????????
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//??????????????
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC?????
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//?????????ADC?????
	ADC_Init(ADC1, &ADC_InitStructure);	//??ADC_InitStruct???????????ADCx????   
	
	ADC_Cmd(ADC1, ENABLE);	//?????ADC1
	
	ADC_ResetCalibration(ADC1);	//??????  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//????????
	
	ADC_StartCalibration(ADC1);	 //??AD??
 
	while(ADC_GetCalibrationStatus(ADC1));	 //??????
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//?????ADC1?????????
}

void Init_ADC_And_DAC(void)
{
	  Dac1_and_Dac2_Init();
	  Init_ADC1();
		set_RefVolParam(8000,2800);
		Set_RefVoltage();
}

static void Dac_Set_Vol(u16 vol,uint32_t DAC_Channel)
{
	float temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
	if(DAC_Channel == DAC_Channel_1)
		DAC_SetChannel1Data(DAC_Align_12b_R, temp);//12??????????DAC?
	else if(DAC_Channel == DAC_Channel_2)
		DAC_SetChannel2Data(DAC_Align_12b_R, temp);
}

static u16 Get_Adc1(u8 ch)   
{
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);	//ADC1
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//?????ADC1?????????	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//??????

	return ADC_GetConversionValue(ADC1);	//??????ADC1????????
}

uint16_t Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc1(ch);
		delay_ms(1);
	}
	return temp_val/times;
} 	

void set_RefVolParam(uint16_t High,uint16_t Low)	//Set Parameter
{
		Ref.vH = High;
		Ref.vL = Low;
}

void Set_RefVoltage(void)										// Set the Parameter to IO
{
	Dac_Set_Vol(Ref.vH,	DAC_Channel_1);
	Dac_Set_Vol(Ref.vL,	DAC_Channel_2);
	
}



