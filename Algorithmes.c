/**
 * ����, ����������� ������ ��������� ����������.
 *
 * �����: ����� ������
 * ������: 1.0
 */


#include <stdbool.h>
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "Main.h"
#include "EXTIs.h"
#include "Timers.h"
#include "USARTs.h"
#include "I2Cs.h"
#include "ADCs.h"
#include "SSD1306_I2C.h"
#include "Algorithmes.h"


	u32 i;	//����. ���.
	u32 j;	//����. ���.

	bool is_GPIOAB_TurnedOn = 0; 			//�������� �� �����?
	u16 ChargedBatteryVoltage = 4150;		//���������� ���������� �������
	u16 DischargedBatteryVoltage = 3200;	//���������� ����������� �������


/**
 * �-��� ���������� �����.
 */
void GPIOAB_TurnOn(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);	//��������� ���� �
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);	//��������� ���� B
	is_GPIOAB_TurnedOn = 1;
}


/**
 * �-��� ����������� �����.
 */
void GPIOAB_TurnOff(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , DISABLE);	//����������� ���� �
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , DISABLE);	//����������� ���� B
	is_GPIOAB_TurnedOn = 0;
}


/**
 * �-��� ������� � ������
 * 					~10 ����. ��. �� �����
 * 					~8 ����. ��. �� �����
 * 					~6 ����. ��. �� ������
 * @param nCount ���������� ������ �������
 */
void MK_Delay8MIPS(volatile uint32_t nCount)
{
	while(nCount--);
}


/**
 * �-��� ������� � ��. ��� ������� 72 ���. ����� ��������!!!
 * @param nCount ���������� ����������� �������
 */
void MK_DelayMS(volatile uint32_t nCount)
{
	nCount*=9000;
	while(nCount--);
}


/**
 * �-��� ������� � ���. ��� ������� 72 ���. ����� ��������!!!!!
 * @param nCount ���������� ����������� �������
 */
void MK_DelayMKS(volatile uint32_t nCount)
{
	nCount*=8;
	while(nCount--);
}


/**
 * �-��� ��������� �������� Vbat ��� �������, � ��������� ���������������
 */
u16 MK_GetBatteryVoltage(void)
{
	u16 voltage = 0;
	for (i = 0; i<3; i++)
	{
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC));
		voltage += ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_4);
	}
	voltage = ((1800000 / (voltage / 3)) * 4048) / 1000;
	return voltage;
}


/**
 * �-��� ��������� ���������������� � ������� ��������� ����� ������������ ����������.
 */
void MK_TurnOn(void)
{

	if (!is_GPIOAB_TurnedOn)
		GPIOAB_TurnOn();				//������������ ������
	if(!is_ADC1_TurnedOn)
		ADC1_TurnOn();					//�������� ADC
	if (!is_I2C1_TurnedOn)
		I2C1_TurnOn();					//�������� I2C1
	if (!is_SSD1306_TurnedOn)
		ssd1306_TurnOn();				//�������� OLED
	if (!is_TIM3_TurnedOn)
		TIM3_TurnOn();					//�������� ������

	OLED_Config.ShowDataGraph = 1;

	if (is_Vbus)
	{

		//���� ���� Vbus
		MainLoop_Config.ChekBatteryVoltage = 1;

		ssd1306_Draw12x16Str(30,25,"������!",0);

		GPIO_SetBits(GPIOC, GPIO_Pin_9);	//�������� ������� ����� � ��
		GPIO_SetBits(GPIOC, GPIO_Pin_8);	//
		MK_DelayMS(100);					//�����
		GPIO_ResetBits(GPIOC, GPIO_Pin_8);	//

		MK_DelayMS(500);
		ssd1306_Clear();

		USART1_TurnOn();
		setArrayStatusToReady(0);
		setArrayStatusToReady(1);

		MainLoop_Config.DispatchData =1;
		MainLoop_Config.ChekBatteryVoltage =1;
		MainLoop_Config.ChekBatteryCharge = 1;
		MainLoop_Config.UpdateOLED = 1;
		MainLoop_Config.SendDataToBluetooth = 1;

		USART1_TurnOn();

		is_MK_TurnedOn = 1;					//������������ ����

		if(!is_TIM3_Started)
				TIM3_Start();
	}
	else
	{

		//���� ��� Vbus

		if(MK_GetBatteryVoltage()<DischargedBatteryVoltage)		//���� GetVbat() < 3200
		{
			for(i = 0; i<5; i++)
			{
				ssd1306_Clear();
				MK_DelayMS(500);
				ssd1306_Draw12x16Str(10,16,"�������",0);
				ssd1306_Draw12x16Str(0,32,"���������!",0);
				MK_DelayMS(500);
			}
			MK_TurnOff();				//��������� ��
		}

		else
		{
			//��������� ������ hell�
			ssd1306_Draw12x16Str(30,25,"������!",0);

			GPIO_SetBits(GPIOC, GPIO_Pin_9);	//�������� ������� ����� � ��
			GPIO_SetBits(GPIOC, GPIO_Pin_8);	//
			MK_DelayMS(50);					//�����
			GPIO_ResetBits(GPIOC, GPIO_Pin_8);	//

			MK_DelayMS(500);
			ssd1306_Clear();

			USART1_TurnOn();
			setArrayStatusToReady(0);
			setArrayStatusToReady(1);

			MainLoop_Config.DispatchData =1;
			MainLoop_Config.ChekBatteryVoltage =1;
			MainLoop_Config.ChekBatteryCharge = 1;
			MainLoop_Config.UpdateOLED = 1;
			MainLoop_Config.SendDataToBluetooth = 1;

			is_MK_TurnedOn = 1;					//������������ ����

			if(!is_TIM3_Started)
					TIM3_Start();
		}
	}
}


/**
 * �-��� ���������� ���������������� � ���������
 */
void MK_TurnOff(void)
{
	if (is_MK_TurnedOn)
	{
		ssd1306_Clear();
		ssd1306_Draw12x16Str(40,25,"����!",0);

		GPIO_SetBits(GPIOC, GPIO_Pin_8);	//
		MK_DelayMS(100);					//�����
		GPIO_ResetBits(GPIOC, GPIO_Pin_8);	//
	}

	is_MK_TurnedOn = 0;	//������������ ����

	if (is_MK_Charge == 1)
	{
		MK_ExceptChargeStructInit();

		USART1_TurnOff();

		TIM3_Stop();
		TIM3_TurnOff();

		countSec = 0;

		GPIO_ResetBits(GPIOC, GPIO_Pin_9);	//��������� ������� ����� � ��

		MK_DelayMS(1000);
		ssd1306_Clear();
	}

	else
	{

		is_MK_TurnedOn = 0;	//������������ ����

		MK_StructInit();

		USART1_TurnOff();

		GPIO_ResetBits(GPIOC, GPIO_Pin_9);	//��������� ������� ����� � ��

		TIM3_Stop();
		TIM3_TurnOff();
		countSec = 0;
		MK_DelayMS(1000);
		ssd1306_Clear();

		if (is_SSD1306_TurnedOn)
			ssd1306_TurnOff();				//�������� OLED
		if (is_I2C1_TurnedOn)
			I2C1_TurnOff();					//�������� I2C1
		if(is_ADC1_TurnedOn)
			ADC1_TurnOff();					//�������� ADC
		if (is_GPIOAB_TurnedOn)
			GPIOAB_TurnOff();				//������������ ������

	}
}

