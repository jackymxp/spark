#ifndef HW_VOLSET_H
#define HW_VOLSET_H

#include "stm32f10x.h"

void Init_ADC_And_DAC(void);	

uint16_t Get_Adc_Average(u8 ch,u8 times);//Get ADC from Ch1/Ch2 1/2

void set_RefVolParam(uint16_t High,uint16_t Low); // Apply the High and Low to RefVol setting parameter struct 

void Set_RefVoltage(void);// Application the ADC setting paramerer to ADC output

#endif

