/*
*********************************************************************************************************
*
*	模块名称 : 外部SRAM读写演示模块。
*	文件名称 : demo_ext_sdram.c
*	版    本 : V1.0
*	说    明 : 外部SDRAM读写例程。
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2015-12-12 armfly  正式发布
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "demo_ext_sdram.h"
#include "bsp.h"

/* 仅允许本文件内调用的函数声明 */
static void DispMenu(void);
static void ReadTest(void);
static void WriteTest(uint32_t _Value);

#define TEST_ADDRESS	0
#define TEST_BUF_SIZE	1024

uint32_t g_TestBuf[TEST_BUF_SIZE];	/* 数据测试用缓冲区 */

/*
*********************************************************************************************************
*	函 数 名: DemoExtSDRAM
*	功能说明: SDRAM读写例程
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void DemoExtSDRAM(void)
{
	uint8_t cmd;
	uint32_t err;
	uint32_t testdata;
	
	//printf("型号: IS61WV102416BLL-10TL  容量2M字节，16Bit，10ns速度\r\n");
	printf("SDRAM Model MT48LC4M32B2TG-7, 32Bit");
	DispMenu();		/* 打印命令提示 */
	while(1)
	{
		bsp_Idle();		/* 这个函数在bsp.c文件。用户可以修改这个函数实现CPU休眠和喂狗 */
		
		if (comGetChar(COM1, &cmd))	/* 从串口读入一个字符(非阻塞方式) */
		{			
			switch (cmd)
			{
				case '1':
					printf("【1 - 测试SDRAM (%d 字节)】\r\n", EXT_SDRAM_SIZE);
					err = bsp_TestExtSDRAM2();
					if (err == 0)
					{
						printf("外部SDRAM测试通过\r\n");
					}
					else
					{
						printf("外部SDRAM出错，错误单元个数：%d\r\n", err);
					}
					break;

				case '2':
					testdata = 0x55555555;
					printf("【2 - 写SDRAM (%dM 字节,0x55)】\r\n", EXT_SDRAM_SIZE /(1024 * 1024));
					WriteTest(testdata);	/* 写SDRAM测试 0x55 */
					break;

				case '3':
					testdata = 0xAAAAAAAA;
					printf("【3 - 写SDRAM (%dM 字节,0xAA)】\r\n", EXT_SDRAM_SIZE /(1024 * 1024));
					WriteTest(testdata);	/* 写SDRAM测试 0xAA */
					break;

				case '4':
					printf("【4 - 读SDRAM数据】\r\n");
					ReadTest();	/* 读SDRAM测试 */
					break;

				default:
					DispMenu();	/* 无效命令，重新打印命令提示 */
					break;
			}
		}

		/* 按键滤波和检测由后台systick中断服务程序实现，我们只需要调用bsp_GetKey读取键值即可。 */		
		switch (bsp_GetKey())	/* bsp_GetKey()读取键值, 无键按下时返回 KEY_NONE = 0 */
		{
			case KEY_DOWN_K1:			/* K1键按下 */
				break;

			case KEY_UP_K1:				/* K1键弹起 */
				break;

			case KEY_DOWN_K2:			/* K2键按下 */
				break;

			case KEY_UP_K2:				/* K2键弹起 */
				break;

			case KEY_DOWN_K3:			/* K3键按下 */
				break;

			case KEY_UP_K3:				/* K3键弹起 */
				break;

			case JOY_DOWN_U:			/* 摇杆UP键按下 */
				break;

			case JOY_DOWN_D:			/* 摇杆DOWN键按下 */
				break;

			case JOY_DOWN_L:			/* 摇杆LEFT键按下 */
				break;

			case JOY_DOWN_R:			/* 摇杆RIGHT键按下 */
				break;

			case JOY_DOWN_OK:			/* 摇杆OK键按下 */
				break;

			case JOY_UP_OK:				/* 摇杆OK键弹起 */
				break;

			case KEY_NONE:				/* 无键按下 */
			default:
				/* 其它的键值不处理 */
				break;
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: WriteTest
*	功能说明: 写SRAM
*	形    参: _Value : 写入的数据
*	返 回 值: 无
*********************************************************************************************************
*/
static void WriteTest(uint32_t _Value)
{
	uint32_t i;
	int32_t iTime1, iTime2;
	uint32_t *pBuf;

	pBuf = (uint32_t *)EXT_SDRAM_ADDR;
	
	/* 写EEPROM, 起始地址 = 0，数据长度为 256 */
	iTime1 = bsp_GetRunTime();	/* 记下开始时间 */
	pBuf = (uint32_t *)EXT_SDRAM_ADDR;

	for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
	{
		*pBuf++ = _Value;
	}
	
	iTime2 = bsp_GetRunTime();	/* 记下结束时间 */

	/* 打印速度 */
	printf("数据写入 %dM,耗时: %dms, 写速度: %dKB/s\r\n", (EXT_SDRAM_SIZE / 1024 / 1024),
		iTime2 - iTime1, (EXT_SDRAM_SIZE / 1024 * 1000) / (iTime2 - iTime1));
}

/*
*********************************************************************************************************
*	函 数 名: ReadTest
*	功能说明: 读数据，并打印出来
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void ReadTest(void)
{
	uint32_t i, j;
	int32_t iTime1, iTime2;
	uint32_t *pBuf;
	
	iTime1 = bsp_GetRunTime();	/* 记下开始时间 */
	pBuf = (uint32_t *)(EXT_SDRAM_ADDR + TEST_ADDRESS);

	/* 容量16M字节 */
	for (i = 0; i < EXT_SDRAM_SIZE / TEST_BUF_SIZE / 4; i++)
	{
		for (j = 0; j < TEST_BUF_SIZE; j++)		
		{
			g_TestBuf[j] = *pBuf++;
		}
	}
	iTime2 = bsp_GetRunTime();	/* 记下结束时间 */
	
	printf("物理地址：0x%08X  大小: %d字节  显示: %d字节  数据如下: \r\n", EXT_SDRAM_ADDR + TEST_ADDRESS, EXT_SDRAM_SIZE, TEST_BUF_SIZE);
	
	/* 打印数据 */
	for (i = 0; i < TEST_BUF_SIZE; i++)
	{
		printf(" %04X", g_TestBuf[i]);

		if ((i & 31) == 31)
		{
			printf("\r\n");		/* 每行显示32字节数据 */
		}
		else if ((i & 31) == 15)
		{
			printf(" - ");
		}
	}

	/* 打印读速度 */
	printf("读耗时: %dms, 读速度: %dKB/s\r\n", iTime2 - iTime1, (EXT_SDRAM_SIZE / 1024 * 1000) / (iTime2 - iTime1));
}

/*
*********************************************************************************************************
*	函 数 名: DispMenu
*	功能说明: 显示操作提示菜单
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispMenu(void)
{
	printf("\r\n------------------------------------------------\r\n");
	printf("请选择操作命令:\r\n");
	printf("1 - 测试SDRAM (%d 字节)\r\n", EXT_SDRAM_SIZE);
	printf("2 - 写SDRAM (%dM 字节,0x55)\r\n", EXT_SDRAM_SIZE /(1024 * 1024));
	printf("3 - 写SDRAM (%dM 字节,0xAA)\r\n", EXT_SDRAM_SIZE /(1024 * 1024));
	printf("4 - 读SDRAM数据\r\n");
	printf("5 - 显示命令提示\r\n");
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
