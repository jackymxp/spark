/*
*********************************************************************************************************
*
*	ģ������ : �ⲿSRAM��д��ʾģ�顣
*	�ļ����� : demo_ext_sdram.c
*	��    �� : V1.0
*	˵    �� : �ⲿSDRAM��д���̡�
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2015-12-12 armfly  ��ʽ����
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "demo_ext_sdram.h"
#include "bsp.h"

/* �������ļ��ڵ��õĺ������� */
static void DispMenu(void);
static void ReadTest(void);
static void WriteTest(uint32_t _Value);

#define TEST_ADDRESS	0
#define TEST_BUF_SIZE	1024

uint32_t g_TestBuf[TEST_BUF_SIZE];	/* ���ݲ����û����� */

/*
*********************************************************************************************************
*	�� �� ��: DemoExtSDRAM
*	����˵��: SDRAM��д����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DemoExtSDRAM(void)
{
	uint8_t cmd;
	uint32_t err;
	uint32_t testdata;
	
	//printf("�ͺ�: IS61WV102416BLL-10TL  ����2M�ֽڣ�16Bit��10ns�ٶ�\r\n");
	printf("SDRAM Model MT48LC4M32B2TG-7, 32Bit");
	DispMenu();		/* ��ӡ������ʾ */
	while(1)
	{
		bsp_Idle();		/* ���������bsp.c�ļ����û������޸��������ʵ��CPU���ߺ�ι�� */
		
		if (comGetChar(COM1, &cmd))	/* �Ӵ��ڶ���һ���ַ�(��������ʽ) */
		{			
			switch (cmd)
			{
				case '1':
					printf("��1 - ����SDRAM (%d �ֽ�)��\r\n", EXT_SDRAM_SIZE);
					err = bsp_TestExtSDRAM2();
					if (err == 0)
					{
						printf("�ⲿSDRAM����ͨ��\r\n");
					}
					else
					{
						printf("�ⲿSDRAM��������Ԫ������%d\r\n", err);
					}
					break;

				case '2':
					testdata = 0x55555555;
					printf("��2 - дSDRAM (%dM �ֽ�,0x55)��\r\n", EXT_SDRAM_SIZE /(1024 * 1024));
					WriteTest(testdata);	/* дSDRAM���� 0x55 */
					break;

				case '3':
					testdata = 0xAAAAAAAA;
					printf("��3 - дSDRAM (%dM �ֽ�,0xAA)��\r\n", EXT_SDRAM_SIZE /(1024 * 1024));
					WriteTest(testdata);	/* дSDRAM���� 0xAA */
					break;

				case '4':
					printf("��4 - ��SDRAM���ݡ�\r\n");
					ReadTest();	/* ��SDRAM���� */
					break;

				default:
					DispMenu();	/* ��Ч������´�ӡ������ʾ */
					break;
			}
		}

		/* �����˲��ͼ���ɺ�̨systick�жϷ������ʵ�֣�����ֻ��Ҫ����bsp_GetKey��ȡ��ֵ���ɡ� */		
		switch (bsp_GetKey())	/* bsp_GetKey()��ȡ��ֵ, �޼�����ʱ���� KEY_NONE = 0 */
		{
			case KEY_DOWN_K1:			/* K1������ */
				break;

			case KEY_UP_K1:				/* K1������ */
				break;

			case KEY_DOWN_K2:			/* K2������ */
				break;

			case KEY_UP_K2:				/* K2������ */
				break;

			case KEY_DOWN_K3:			/* K3������ */
				break;

			case KEY_UP_K3:				/* K3������ */
				break;

			case JOY_DOWN_U:			/* ҡ��UP������ */
				break;

			case JOY_DOWN_D:			/* ҡ��DOWN������ */
				break;

			case JOY_DOWN_L:			/* ҡ��LEFT������ */
				break;

			case JOY_DOWN_R:			/* ҡ��RIGHT������ */
				break;

			case JOY_DOWN_OK:			/* ҡ��OK������ */
				break;

			case JOY_UP_OK:				/* ҡ��OK������ */
				break;

			case KEY_NONE:				/* �޼����� */
			default:
				/* �����ļ�ֵ������ */
				break;
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: WriteTest
*	����˵��: дSRAM
*	��    ��: _Value : д�������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void WriteTest(uint32_t _Value)
{
	uint32_t i;
	int32_t iTime1, iTime2;
	uint32_t *pBuf;

	pBuf = (uint32_t *)EXT_SDRAM_ADDR;
	
	/* дEEPROM, ��ʼ��ַ = 0�����ݳ���Ϊ 256 */
	iTime1 = bsp_GetRunTime();	/* ���¿�ʼʱ�� */
	pBuf = (uint32_t *)EXT_SDRAM_ADDR;

	for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
	{
		*pBuf++ = _Value;
	}
	
	iTime2 = bsp_GetRunTime();	/* ���½���ʱ�� */

	/* ��ӡ�ٶ� */
	printf("����д�� %dM,��ʱ: %dms, д�ٶ�: %dKB/s\r\n", (EXT_SDRAM_SIZE / 1024 / 1024),
		iTime2 - iTime1, (EXT_SDRAM_SIZE / 1024 * 1000) / (iTime2 - iTime1));
}

/*
*********************************************************************************************************
*	�� �� ��: ReadTest
*	����˵��: �����ݣ�����ӡ����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ReadTest(void)
{
	uint32_t i, j;
	int32_t iTime1, iTime2;
	uint32_t *pBuf;
	
	iTime1 = bsp_GetRunTime();	/* ���¿�ʼʱ�� */
	pBuf = (uint32_t *)(EXT_SDRAM_ADDR + TEST_ADDRESS);

	/* ����16M�ֽ� */
	for (i = 0; i < EXT_SDRAM_SIZE / TEST_BUF_SIZE / 4; i++)
	{
		for (j = 0; j < TEST_BUF_SIZE; j++)		
		{
			g_TestBuf[j] = *pBuf++;
		}
	}
	iTime2 = bsp_GetRunTime();	/* ���½���ʱ�� */
	
	printf("�����ַ��0x%08X  ��С: %d�ֽ�  ��ʾ: %d�ֽ�  ��������: \r\n", EXT_SDRAM_ADDR + TEST_ADDRESS, EXT_SDRAM_SIZE, TEST_BUF_SIZE);
	
	/* ��ӡ���� */
	for (i = 0; i < TEST_BUF_SIZE; i++)
	{
		printf(" %04X", g_TestBuf[i]);

		if ((i & 31) == 31)
		{
			printf("\r\n");		/* ÿ����ʾ32�ֽ����� */
		}
		else if ((i & 31) == 15)
		{
			printf(" - ");
		}
	}

	/* ��ӡ���ٶ� */
	printf("����ʱ: %dms, ���ٶ�: %dKB/s\r\n", iTime2 - iTime1, (EXT_SDRAM_SIZE / 1024 * 1000) / (iTime2 - iTime1));
}

/*
*********************************************************************************************************
*	�� �� ��: DispMenu
*	����˵��: ��ʾ������ʾ�˵�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispMenu(void)
{
	printf("\r\n------------------------------------------------\r\n");
	printf("��ѡ���������:\r\n");
	printf("1 - ����SDRAM (%d �ֽ�)\r\n", EXT_SDRAM_SIZE);
	printf("2 - дSDRAM (%dM �ֽ�,0x55)\r\n", EXT_SDRAM_SIZE /(1024 * 1024));
	printf("3 - дSDRAM (%dM �ֽ�,0xAA)\r\n", EXT_SDRAM_SIZE /(1024 * 1024));
	printf("4 - ��SDRAM����\r\n");
	printf("5 - ��ʾ������ʾ\r\n");
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
