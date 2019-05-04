/**
 * Файл, описывающий работу различных алгоритмов.
 *
 * Автор: Зенин Кирилл
 * Версия: 1.0
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


	u32 i;	//цикл. пер.
	u32 j;	//цикл. пер.

	bool is_GPIOAB_TurnedOn = 0; 			//включены ли порты?
	u16 ChargedBatteryVoltage = 4150;		//напряжение заряженной батареи
	u16 DischargedBatteryVoltage = 3200;	//напряжение разряженной батареи


/**
 * Ф-ция включающая порты.
 */
void GPIOAB_TurnOn(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);	//тактируем порт А
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);	//тактируем порт B
	is_GPIOAB_TurnedOn = 1;
}


/**
 * Ф-ция выключающая порты.
 */
void GPIOAB_TurnOff(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , DISABLE);	//нетактируем порт А
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , DISABLE);	//нетактируем порт B
	is_GPIOAB_TurnedOn = 0;
}


/**
 * Ф-ция простоя в циклах
 * 					~10 атом. оп. на входе
 * 					~8 атом. оп. на цикле
 * 					~6 атом. оп. на выходе
 * @param nCount количество циклов простоя
 */
void MK_Delay8MIPS(volatile uint32_t nCount)
{
	while(nCount--);
}


/**
 * Ф-ция простоя в мс. при частоте 72 МГц. Очень примерно!!!
 * @param nCount количество миллисекунд простоя
 */
void MK_DelayMS(volatile uint32_t nCount)
{
	nCount*=9000;
	while(nCount--);
}


/**
 * Ф-ция простоя в мкс. при частоте 72 МГц. Очень примерно!!!!!
 * @param nCount количество микросекунд простоя
 */
void MK_DelayMKS(volatile uint32_t nCount)
{
	nCount*=8;
	while(nCount--);
}


/**
 * Ф-ция получения значения Vbat без таймера, с отдельным преобразованием
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
 * Ф-ция включения микроконтроллера и запуска периферии после программного выключения.
 */
void MK_TurnOn(void)
{

	if (!is_GPIOAB_TurnedOn)
		GPIOAB_TurnOn();				//тактирование портов
	if(!is_ADC1_TurnedOn)
		ADC1_TurnOn();					//включаем ADC
	if (!is_I2C1_TurnedOn)
		I2C1_TurnOn();					//включаем I2C1
	if (!is_SSD1306_TurnedOn)
		ssd1306_TurnOn();				//включаем OLED
	if (!is_TIM3_TurnedOn)
		TIM3_TurnOn();					//включаем таймер

	OLED_Config.ShowDataGraph = 1;

	if (is_Vbus)
	{

		//если есть Vbus
		MainLoop_Config.ChekBatteryVoltage = 1;

		ssd1306_Draw12x16Str(30,25,"Привет!",0);

		GPIO_SetBits(GPIOC, GPIO_Pin_9);	//включаем питание вибро и ОУ
		GPIO_SetBits(GPIOC, GPIO_Pin_8);	//
		MK_DelayMS(100);					//бжиик
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

		is_MK_TurnedOn = 1;					//выставляется флаг

		if(!is_TIM3_Started)
				TIM3_Start();
	}
	else
	{

		//если нет Vbus

		if(MK_GetBatteryVoltage()<DischargedBatteryVoltage)		//если GetVbat() < 3200
		{
			for(i = 0; i<5; i++)
			{
				ssd1306_Clear();
				MK_DelayMS(500);
				ssd1306_Draw12x16Str(10,16,"Батарея",0);
				ssd1306_Draw12x16Str(0,32,"разряжена!",0);
				MK_DelayMS(500);
			}
			MK_TurnOff();				//выключаем МК
		}

		else
		{
			//выводится напись hellо
			ssd1306_Draw12x16Str(30,25,"Привет!",0);

			GPIO_SetBits(GPIOC, GPIO_Pin_9);	//включаем питание вибро и ОУ
			GPIO_SetBits(GPIOC, GPIO_Pin_8);	//
			MK_DelayMS(50);					//бжиик
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

			is_MK_TurnedOn = 1;					//выставляется флаг

			if(!is_TIM3_Started)
					TIM3_Start();
		}
	}
}


/**
 * Ф-ция выключения микроконтроллера и периферии
 */
void MK_TurnOff(void)
{
	if (is_MK_TurnedOn)
	{
		ssd1306_Clear();
		ssd1306_Draw12x16Str(40,25,"Пока!",0);

		GPIO_SetBits(GPIOC, GPIO_Pin_8);	//
		MK_DelayMS(100);					//бжиик
		GPIO_ResetBits(GPIOC, GPIO_Pin_8);	//
	}

	is_MK_TurnedOn = 0;	//выставляется флаг

	if (is_MK_Charge == 1)
	{
		MK_ExceptChargeStructInit();

		USART1_TurnOff();

		TIM3_Stop();
		TIM3_TurnOff();

		countSec = 0;

		GPIO_ResetBits(GPIOC, GPIO_Pin_9);	//выключаем питание вибро и ОУ

		MK_DelayMS(1000);
		ssd1306_Clear();
	}

	else
	{

		is_MK_TurnedOn = 0;	//выставляется флаг

		MK_StructInit();

		USART1_TurnOff();

		GPIO_ResetBits(GPIOC, GPIO_Pin_9);	//выключаем питание вибро и ОУ

		TIM3_Stop();
		TIM3_TurnOff();
		countSec = 0;
		MK_DelayMS(1000);
		ssd1306_Clear();

		if (is_SSD1306_TurnedOn)
			ssd1306_TurnOff();				//включаем OLED
		if (is_I2C1_TurnedOn)
			I2C1_TurnOff();					//включаем I2C1
		if(is_ADC1_TurnedOn)
			ADC1_TurnOff();					//включаем ADC
		if (is_GPIOAB_TurnedOn)
			GPIOAB_TurnOff();				//тактирование портов

	}
}

