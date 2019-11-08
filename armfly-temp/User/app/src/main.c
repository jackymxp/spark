/*
*********************************************************************************************************
*
*	ģ������ : ������ģ�顣
*	�ļ����� : main.c
*	��    �� : V1.0
*	˵    �� : ������
*	�޸ļ�¼ :
*		�汾��   ����       ����       ˵��
*		v1.0    2015-12-28 armfly    �׷�
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"			/* �ײ�Ӳ������ */
#include "demo_spi_ad7606.h"

/* ���������������̷������� */
#define EXAMPLE_NAME	"V6-AD7606��8ͨ��16λͬ��ADC��spiģʽ����"
#define EXAMPLE_DATE	"2015-12-30"
#define DEMO_VER		"1.0"


/* �������ļ��ڵ��õĺ������� */
static void PrintfLogo(void);
static void PrintfHardInfo(void);

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: c�������
*	��    �Σ���
*	�� �� ֵ: �������(���账��)
*********************************************************************************************************
*/
int main(void)
{
	/*
		����ST�̼���������ļ��Ѿ�ִ����CPUϵͳʱ�ӵĳ�ʼ�������Բ����ٴ��ظ�����ϵͳʱ�ӡ�
		�����ļ�������CPU��ʱ��Ƶ�ʡ��ڲ�Flash�����ٶȺͿ�ѡ���ⲿSRAM FSMC��ʼ����
	*/
	
	bsp_Init();
	printf("\33[2J");	/* ���� */
	
	PrintfLogo();		/* ��ӡ����Logo������1 */
	PrintfHardInfo();	/* ��ӡӲ��������Ϣ */
	
	bsp_SetTIMOutPWM(GPIOA, GPIO_Pin_8, TIM1, 1, 10000, 5000);
	Demo_spi_AD7606();	/* ����AD7606��ʾ���� */
	while(1);
	
	
	
	
}

/*
*********************************************************************************************************
*	�� �� ��: PrintfHardInfo
*	����˵��: ��ӡӲ��������Ϣ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void PrintfHardInfo(void)
{
	printf("���߷���: \r\n");
	printf("AD7606ģ��       	spiģʽ \r\n");
    printf("  +5V       <------   5.0V      5V����\r\n");
    printf("  GND       -------   GND       ��\r\n");
	printf("  CS        <------   PG10      SPI_CS\r\n");
	printf("  RD/SCLK   <------   PB3       SPI_SCLK\r\n");
	printf("  DB7/DOUT  ------>   PB4       SPI_MISO\r\n");
	printf("  RAGE      <------   D2        ģ���ź���������\r\n");
	printf("  RST       <------   PC7       ��λ�ź�\r\n");
	printf("  CVA CVB   <------   PC6       ����ADת��\r\n");
	printf("  OS0       <------   GND       ����������0(Ĭ�Ͻӵ�)\r\n");
	printf("  OS1       <------   GND       ����������1(Ĭ�Ͻӵ�)\r\n");
	printf("  OS2       <------   GND       ����������2(Ĭ�Ͻӵ�)\r\n");
	printf("  BUSY      ------>   PE4       BUSY�ź�\r\n");
	printf("��ӡ�ɼ�����: \r\n");
}


/*
*********************************************************************************************************
*	�� �� ��: PrintfLogo
*	����˵��: ��ӡ�������ƺ����̷�������, ���ϴ����ߺ󣬴�PC���ĳ����ն�������Թ۲���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	/* ���CPU ID */
	{
		/* �ο��ֲ᣺
			32.6.1 MCU device ID code
			33.1 Unique device ID register (96 bits)
		*/
		uint32_t CPU_Sn0, CPU_Sn1, CPU_Sn2;

		CPU_Sn0 = *(__IO uint32_t*)(0x1FFF7A10);
		CPU_Sn1 = *(__IO uint32_t*)(0x1FFF7A10 + 4);
		CPU_Sn2 = *(__IO uint32_t*)(0x1FFF7A10 + 8);

		printf("\r\nCPU : STM32F429BI, LQFP208, ��Ƶ: %dMHz\r\n", SystemCoreClock / 1000000);
		printf("UID = %08X %08X %08X\n\r", CPU_Sn2, CPU_Sn1, CPU_Sn0);
	}

	printf("\n\r");
	printf("*************************************************************\n\r");
	printf("* ��������   : %s\r\n", EXAMPLE_NAME);	/* ��ӡ�������� */
	printf("* ���̰汾   : %s\r\n", DEMO_VER);		/* ��ӡ���̰汾 */
	printf("* ��������   : %s\r\n", EXAMPLE_DATE);	/* ��ӡ�������� */

	/* ��ӡST�̼���汾����3���������stm32f10x.h�ļ��� */
	printf("* �̼���汾 : V%d.%d.%d (STM32F4xx_StdPeriph_Driver)\r\n", __STM32F4XX_STDPERIPH_VERSION_MAIN,
			__STM32F4XX_STDPERIPH_VERSION_SUB1,__STM32F4XX_STDPERIPH_VERSION_SUB2);
	printf("* \r\n");	/* ��ӡһ�пո� */
	printf("* QQ    : 1295744630 \r\n");
	printf("* ����  : armfly\r\n");
	printf("* Email : armfly@qq.com \r\n");
	printf("* �Ա���: armfly.taobao.com\r\n");
	printf("* Copyright www.armfly.com ����������\r\n");
	printf("*************************************************************\n\r");
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
