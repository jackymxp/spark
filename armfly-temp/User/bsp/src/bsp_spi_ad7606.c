/*
*********************************************************************************************************
*
*	模块名称 : AD7606 spi模式驱动模块(16位ADC)
*	文件名称 : bsp_spi_ad7606.c
*	版    本 : V1.0
*	说    明 : ad7606模块和CPU之间采用SPI接口。本驱动程序支持硬件SPI接口和软件SPI接口。
*			  通过宏切换。
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2015-11-06  armfly  正式发布
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/* SPI总线的SCK、MOSI、MISO 在 bsp_spi_bus.c中配置  */
/* CSN片选 */
#define RCC_CS 		RCC_AHB1Periph_GPIOA
#define PORT_CS		GPIOA
#define PIN_CS		GPIO_Pin_4

/* RESET */
#define RCC_RESET 	RCC_AHB1Periph_GPIOE
#define PORT_RESET	GPIOE
#define PIN_RESET	GPIO_Pin_2

#define RESET_0()	GPIO_ResetBits(PORT_RESET, PIN_RESET)
#define RESET_1()	GPIO_SetBits(PORT_RESET, PIN_RESET)

/* CONVST */		
#define RCC_CONVST 	RCC_AHB1Periph_GPIOE
#define PORT_CONVST	GPIOE
#define PIN_CONVST 	GPIO_Pin_3

/* BUSY */
#define RCC_BUSY 	RCC_AHB1Periph_GPIOI
#define PORT_BUSY 	GPIOI
#define PIN_BUSY 	GPIO_Pin_8

#define BUSY_IS_LOW()				(GPIO_ReadInputDataBit(PORT_BUSY, PIN_BUSY) == Bit_RESET)

static void AD7606_ConfigGPIO(void);
void AD7606_Reset(void);	
void AD7606_SetInputRange(uint8_t _ucRange);
void AD7606_StartConv(void);
void AD7606_CfgSpiHard(void);

static int16_t s_adc_now[8];

AD7606_T g_tAD7606;

/*
*********************************************************************************************************
*	函 数 名: bsp_InitAD7606
*	功能说明: 初始化AD7606 SPI口线
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_spi_InitAD7606(void)
{
	AD7606_ConfigGPIO();		/* 配置GPIO */
	
#ifdef HARD_SPI
	AD7606_CfgSpiHard();
#endif
	
	AD7606_SetInputRange(g_tAD7606.Range);	/* 设置采样量程 */

	/* 设置过采样模式 */
	//ad7606_SetOS(0);
	AD7606_Reset();				/* 硬件复位复AD7606 */
	
	AD_CONVST_HIGH();			/* CONVST脚设置为高电平 */	
}

/*
*********************************************************************************************************
*	函 数 名: AD7606_ConfigGPIO
*	功能说明: 配置GPIO。 不包括 SCK  MOSI  MISO 共享的SPI总线。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AD7606_ConfigGPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 打开GPIO时钟 */
	RCC_AHB1PeriphClockCmd(RCC_CS | RCC_BUSY | RCC_RESET | RCC_CONVST, ENABLE);

	/* 配置几个推完输出IO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* 设为输出口 */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 上下拉电阻不使能 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */

	GPIO_InitStructure.GPIO_Pin = PIN_RESET;
	GPIO_Init(PORT_RESET, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_CONVST;
	GPIO_Init(PORT_CONVST, &GPIO_InitStructure);
	
//	GPIO_InitStructure.GPIO_Pin = PIN_RANGE;
//	GPIO_Init(PORT_RANGE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PIN_CS;
	GPIO_Init(PORT_CS, &GPIO_InitStructure);

	/* 配置GPIO为浮动输入模式(实际上CPU复位后就是输入状态) */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* 设为输入口 */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 无需上下拉电阻 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO口最大速度 */

	GPIO_InitStructure.GPIO_Pin = PIN_BUSY;
	GPIO_Init(PORT_BUSY, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: AD7606_CfgSpiHard
*	功能说明: 配置STM32内部SPI硬件的工作模式、速度等参数，用于访问AD7606
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void AD7606_CfgSpiHard(void)
{
	SPI_InitTypeDef  SPI_InitStructure;

	/* 配置SPI硬件参数 */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	/* 数据方向：2线全双工 */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		/* STM32的SPI工作模式 ：主机模式 */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	/* 数据位长度 ： 8位 */
	/* SPI_CPOL和SPI_CPHA结合使用决定时钟和数据采样点的相位关系、
	   本例配置: 总线空闲是高电平,第2个边沿（上升沿采样数据)
	*/
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;			/* 时钟上升沿采样数据 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		/* 时钟的第2个边沿采样数据 */
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			/* 片选控制方式：软件控制 */

	/* 设置波特率预分频系数 SPI_BaudRatePrescaler_64 实测SCK周期 800ns ，12.5MHz */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	/* 数据位传输次序：高位先传 */
	SPI_InitStructure.SPI_CRCPolynomial = 7;			/* CRC多项式寄存器，复位后为7。本例程不用 */
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);				/* 使能SPI  */
}

/*
*********************************************************************************************************
*	函 数 名: ad7606_SetOS
*	功能说明: 设置过采样模式（数字滤波，硬件求平均值)
*	形    参：_ucMode : 0-6  0表示无过采样，1表示2倍，2表示4倍，3表示8倍，4表示16倍
*				5表示32倍，6表示64倍
*	返 回 值: 无
*********************************************************************************************************
*/
#if 0
void AD7606_SetOS(uint8_t _ucMode)
{
	if (_ucMode == 1)
	{
		AD_OS2_0();
		AD_OS1_0();
		AD_OS0_1();
	}
	else if (_ucMode == 2)
	{
		AD_OS2_0();
		AD_OS1_1();
		AD_OS0_0();
	}
	else if (_ucMode == 3)
	{
		AD_OS2_0();
		AD_OS1_1();
		AD_OS0_1();
	}
	else if (_ucMode == 4)
	{
		AD_OS2_1();
		AD_OS1_0();
		AD_OS0_0();
	}
	else if (_ucMode == 5)
	{
		AD_OS2_1();
		AD_OS1_0();
		AD_OS0_1();
	}
	else if (_ucMode == 6)
	{
		AD_OS2_1();
		AD_OS1_1();
		AD_OS0_0();
	}
	else	/* 按0处理 */
	{
		AD_OS2_0();
		AD_OS1_0();
		AD_OS0_0();
	}
}
#endif

/*
*********************************************************************************************************
*	函 数 名: AD7606_SetInputRange
*	功能说明: 配置AD7606模拟信号输入量程。
*	形    参: _ucRange : 0 表示正负5V   1表示正负10V
*	返 回 值: 无
*********************************************************************************************************
*/
void AD7606_SetInputRange(uint8_t _ucRange)
{
	if (_ucRange == 0)
	{
		AD_RANGE_5V();	/* 设置为正负5V */
	}
	else
	{
		AD_RANGE_10V();	/* 设置为正负10V */
	}
}

/*
*********************************************************************************************************
*	函 数 名: AD7606_RESET
*	功能说明: 硬件复位
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void AD7606_Reset(void)
{
	/* CS  SCLK拉高 */
#ifdef SOFT_SPI
		AD_CS_1();
		//SCK_1();
#endif

 	/* AD7606是高电平复位，要求最小脉宽50ns */
	AD_RESET_LOW();
	
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	
	AD_RESET_LOW();
}
/*
*********************************************************************************************************
*	函 数 名: ad7606_StartConv
*	功能说明: 启动AD7606的ADC转换
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void AD7606_StartConv(void)
{
	/* 上升沿开始转换，低电平持续时间至少25ns  */
	AD_CONVST_LOW();
	AD_CONVST_LOW();
	AD_CONVST_LOW();	/* 连续执行2次，低电平约50ns */
	
	AD_CONVST_HIGH();
}

/*
*********************************************************************************************************
*	函 数 名: AD7606_scan
*	功能说明: 扫描调用本函数，用于读取AD转换器数据
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void AD7606_Scan(void) 		/* 此函数代码按照时序编写 */
{
	uint8_t i;			

	/* BUSY = 0 时.ad7606处于空闲状态ad转换结束 */	
	if (BUSY_IS_LOW())	   
    {
		AD_CS_0(); /* SPI片选 = 0 */
	
		for (i = 0; i < CH_NUM; i++)
		{
			s_adc_now[i] = bsp_spiRead1();
			
			s_adc_now[i] = s_adc_now[i] * 256 + bsp_spiRead1(); /* 读数据 */	
		}
	
		AD_CS_1(); /* SPI片选 = 1 */

		AD7606_StartConv();	/* 给开始信号 */	
	}
}	  

/*
*********************************************************************************************************
*	函 数 名: GetAdcFormFifo
*	功能说明: 从FIFO中读取一个ADC值
*	形    参：_usReadAdc : 存放ADC结果的变量指针
*	返 回 值: 1 表示OK，0表示暂无数据
*********************************************************************************************************
*/
int16_t AD7606_ReadAdc(uint8_t _ch)
{
	int16_t sAdc;
	
	DISABLE_INT();	
	sAdc = s_adc_now[_ch];
	ENABLE_INT();

	return sAdc;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
