/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块。
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : 主程序
*	修改记录 :
*		版本号   日期       作者       说明
*		v1.0    2015-12-28 armfly    首发
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"			/* 底层硬件驱动 */
#include "demo_spi_ad7606.h"

/* 定义例程名和例程发布日期 */
#define EXAMPLE_NAME	"V6-AD7606（8通道16位同步ADC）spi模式例程"
#define EXAMPLE_DATE	"2015-12-30"
#define DEMO_VER		"1.0"


/* 仅允许本文件内调用的函数声明 */
static void PrintfLogo(void);
static void PrintfHardInfo(void);

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参：无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{
	/*
		由于ST固件库的启动文件已经执行了CPU系统时钟的初始化，所以不必再次重复配置系统时钟。
		启动文件配置了CPU主时钟频率、内部Flash访问速度和可选的外部SRAM FSMC初始化。
	*/
	
	bsp_Init();
	printf("\33[2J");	/* 清屏 */
	
	PrintfLogo();		/* 打印例程Logo到串口1 */
	PrintfHardInfo();	/* 打印硬件接线信息 */
	
	bsp_SetTIMOutPWM(GPIOA, GPIO_Pin_8, TIM1, 1, 10000, 5000);
	Demo_spi_AD7606();	/* 串行AD7606演示程序 */
	while(1);
	
	
	
	
}

/*
*********************************************************************************************************
*	函 数 名: PrintfHardInfo
*	功能说明: 打印硬件接线信息
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfHardInfo(void)
{
	printf("接线方法: \r\n");
	printf("AD7606模块       	spi模式 \r\n");
    printf("  +5V       <------   5.0V      5V供电\r\n");
    printf("  GND       -------   GND       地\r\n");
	printf("  CS        <------   PG10      SPI_CS\r\n");
	printf("  RD/SCLK   <------   PB3       SPI_SCLK\r\n");
	printf("  DB7/DOUT  ------>   PB4       SPI_MISO\r\n");
	printf("  RAGE      <------   D2        模拟信号输入量程\r\n");
	printf("  RST       <------   PC7       复位信号\r\n");
	printf("  CVA CVB   <------   PC6       启动AD转换\r\n");
	printf("  OS0       <------   GND       过采样引脚0(默认接地)\r\n");
	printf("  OS1       <------   GND       过采样引脚1(默认接地)\r\n");
	printf("  OS2       <------   GND       过采样引脚2(默认接地)\r\n");
	printf("  BUSY      ------>   PE4       BUSY信号\r\n");
	printf("打印采集数据: \r\n");
}


/*
*********************************************************************************************************
*	函 数 名: PrintfLogo
*	功能说明: 打印例程名称和例程发布日期, 接上串口线后，打开PC机的超级终端软件可以观察结果
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	/* 检测CPU ID */
	{
		/* 参考手册：
			32.6.1 MCU device ID code
			33.1 Unique device ID register (96 bits)
		*/
		uint32_t CPU_Sn0, CPU_Sn1, CPU_Sn2;

		CPU_Sn0 = *(__IO uint32_t*)(0x1FFF7A10);
		CPU_Sn1 = *(__IO uint32_t*)(0x1FFF7A10 + 4);
		CPU_Sn2 = *(__IO uint32_t*)(0x1FFF7A10 + 8);

		printf("\r\nCPU : STM32F429BI, LQFP208, 主频: %dMHz\r\n", SystemCoreClock / 1000000);
		printf("UID = %08X %08X %08X\n\r", CPU_Sn2, CPU_Sn1, CPU_Sn0);
	}

	printf("\n\r");
	printf("*************************************************************\n\r");
	printf("* 例程名称   : %s\r\n", EXAMPLE_NAME);	/* 打印例程名称 */
	printf("* 例程版本   : %s\r\n", DEMO_VER);		/* 打印例程版本 */
	printf("* 发布日期   : %s\r\n", EXAMPLE_DATE);	/* 打印例程日期 */

	/* 打印ST固件库版本，这3个定义宏在stm32f10x.h文件中 */
	printf("* 固件库版本 : V%d.%d.%d (STM32F4xx_StdPeriph_Driver)\r\n", __STM32F4XX_STDPERIPH_VERSION_MAIN,
			__STM32F4XX_STDPERIPH_VERSION_SUB1,__STM32F4XX_STDPERIPH_VERSION_SUB2);
	printf("* \r\n");	/* 打印一行空格 */
	printf("* QQ    : 1295744630 \r\n");
	printf("* 旺旺  : armfly\r\n");
	printf("* Email : armfly@qq.com \r\n");
	printf("* 淘宝店: armfly.taobao.com\r\n");
	printf("* Copyright www.armfly.com 安富莱电子\r\n");
	printf("*************************************************************\n\r");
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
