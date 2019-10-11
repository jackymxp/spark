#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include "lcd.h"
#include "Systems.h"

void Init_Display(void);

void Init_Display_Distance(int32_t Distance);

void Update_Distance(int32_t Distance);

void Update_SparkRatio(uint8_t Open,uint8_t Spark,uint8_t Short);

void Update_Touched_State(uint8_t NewState);

void Refresh_Screen(Sys_t *s);

#endif
