#ifndef HW_DETECT_H
#define HW_DETECT_H

#include "stm32f10x.h"

typedef struct {
	uint8_t Op;
	uint8_t Sp;
	uint8_t Sh;
}	Spark_t;

typedef struct {
	Spark_t *PtrHead;
	uint8_t Current;
} RateHistoryDate_p_t;

typedef struct {
	
	uint16_t  opentimes;
	uint16_t  sparktimes;
	uint16_t	shorttimes;
	
	uint8_t 	open_rate;
	uint8_t		spark_rate;
	uint8_t		short_rate;
	
	uint32_t  ReportOpenRate;
	uint32_t  ReportSparkRate;
	uint32_t	ReportShortRate;
	
	uint8_t 	ReportFlag;
	
} spark_ratio_t;


void Detect_Cmd(char NewState);

void Init_Spark_Detect(void);		

RateHistoryDate_p_t *GetRateData(void);

void SetTotalCount(uint8_t ratio_percent);	

uint32_t ReportGetRatioRate(void);

#endif

