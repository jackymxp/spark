/*
*********************************************************************************************************
*
*	模块名称 : AD7606 驱动模块(8通道16位ADC)
*	文件名称 : bsp_spi_ad7606.h
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#ifndef _BSP_SPI_AD7606_H
#define _BSP_SPI_AD7606_H

#define CH_NUM			1				/* 采集2通道 */

/* 片选 */
#define AD_CS_0()						GPIO_ResetBits(PORT_CS, PIN_CS)
#define AD_CS_1()						GPIO_SetBits(PORT_CS, PIN_CS)

/* 设置量程 RANGE  外扩IO */
#define AD_RANGE_5V()					HC574_SetPin(NRF24L01_CE, 0)
#define AD_RANGE_10V()					HC574_SetPin(NRF24L01_CE, 1)	//置1

/* 复位引脚 */
#define AD_RESET_LOW()					GPIO_ResetBits(PORT_RESET, PIN_RESET)
#define AD_RESET_HIGH()					GPIO_SetBits(PORT_RESET, PIN_RESET)

/* 起始信号 */
#define	AD_CONVST_LOW()					GPIO_ResetBits(PORT_CONVST, PIN_CONVST)
#define	AD_CONVST_HIGH()				GPIO_SetBits(PORT_CONVST, PIN_CONVST)

typedef struct
{
	uint8_t Range;
}AD7606_T;

void AD7606_Reset(void);		
void AD7606_SetInputRange(uint8_t _ucRange);	/* 设置AD7606量程 */
void bsp_spi_InitAD7606(void);		/* 初始化AD7606 */
void AD7606_Scan(void); 		/* 此函数代码按照时序编写 */
int16_t AD7606_ReadAdc(uint8_t _ch);
void AD7606_SetOS(uint8_t _ucMode);

extern AD7606_T g_tAD7606;

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
