/**
 * Файл, описывающий работу системных таймеров.
 *
 * Автор: Зенин Кирилл
 * Версия: 1.0
 *
 * Реализованы:
 * 	TIM3 - периодический таймер, по сигналу которого проводятся измерения.
 */


#include <stdbool.h>
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"
#include <misc.h>

#include "Main.h"
#include "Timers.h"
#include "ADCs.h"

#define lightTime  	        500

#define TIM3_Freq 			50000 //mHz
#define TIM3_IRFreq 		50000
#define TIM3_RFreq 			50000
#define TIM3_SRFreq 		780 // 1/64 от 50000
#define TIM3_TimeFreq		1000
#define TIM3_BatteryFreq  	500



	struct MainLoop_ConfigStructure MainLoop_Config;	//структура конфигурации главного цикла
	struct OLED_ConfigStructure OLED_Config;			//структура конфигурации индикации

	bool is_TIM3_TurnedOn = 0;							//Таймер3 включен?
	bool is_TIM3_Started = 0;							//Таймер3 запущен?
	bool is_MK_Charge = 0;								//Заряждка идет??

	u32 countSec = 0;		// счетчик секунд со времени старта
	u32 countAll = 0;		// главный кольцевой счетчик полного цикла

	u32 TIM3_IRPer   	=	TIM3_Freq / TIM3_IRFreq;
	u32 TIM3_RPer   	=	TIM3_Freq / TIM3_RFreq;
	u32 TIM3_SRPer   	=	TIM3_Freq / TIM3_SRFreq;
	u32 TIM3_TimePer	=	TIM3_Freq / TIM3_TimeFreq;
	u32 TIM3_BatteryPer =	TIM3_Freq / TIM3_BatteryFreq;
	u32 TIM3_Del 		=	(36000 - 1);   					//T = 0,5ms период тактирования таймера
	u32 TIM3_Nms 		=	2 * (1000000 / TIM3_Freq);		//количество тактов до сброса учитывая миллицерцы
	u32 TIM3_IRQn_Priority =	1;							//приоритет таймера

/**
 *  Ф-ция изначально инициализирующая структуры
 */
void MK_StructInit(void)
{
	MainLoop_Config.ChekBatteryVoltage = 0;
	MainLoop_Config.ChekBatteryCharge = 0;
	MainLoop_Config.DispatchData = 0;
	MainLoop_Config.SendDataToBluetooth = 0;
	MainLoop_Config.UpdateOLED = 0;

	OLED_Config.ShowBatteryVoltage = 0;
	OLED_Config.ShowBatteryCharge = 0;
	OLED_Config.ShowDataGraph = 0;
}


/**
 *  Ф-ция изначально инициализирующая структуры при включенной зарядке
 */
void MK_ExceptChargeStructInit(void)
{
	MainLoop_Config.DispatchData = 0;
	MainLoop_Config.SendDataToBluetooth = 0;
	MainLoop_Config.UpdateOLED = 0;

	OLED_Config.ShowBatteryVoltage = 0;
	OLED_Config.ShowDataGraph = 0;
}


/**
 * Ф-ция инициализирующая таймер3, изначально таймер остановлен.
 */
void TIM3_Init(void)
{

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);


	TIM3->PSC = TIM3_Del;
	TIM3->ARR = TIM3_Nms;
	TIM3->DIER |= TIM_DIER_UIE;
	TIM3->CR1 &= (!TIM_CR1_CEN);
	NVIC_DisableIRQ(TIM3_IRQn);

//	TIM_TimeBaseInitTypeDef Timer;
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
//	TIM_TimeBaseStructInit(&Timer);
//	Timer.TIM_Prescaler = (SystemCoreClock / 2) / 1000 - 1; //предделитель на 500 мс.
//	Timer.TIM_Period = 1000 * 2;							//
//	TIM_TimeBaseInit(TIM4, &Timer);
//	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
//	TIM_Cmd(TIM4, ENABLE);
//	NVIC_EnableIRQ(TIM4_IRQn);
}


/**
 * Ф-ция запускающая таймер3
 */
void TIM3_Start(void)
{
	TIM3->CR1 |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM3_IRQn);
	is_TIM3_Started = 1;
}


/**
 * Ф-ция останавливающая таймер3
 */
void TIM3_Stop(void)
{
	TIM3->CR1 &= (!TIM_CR1_CEN);
	NVIC_DisableIRQ(TIM3_IRQn);
	is_TIM3_Started = 0;
}


/**
 * Ф-ция включающая таймер3
 */
void TIM3_TurnOn(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // включаем тактирование таймера
	is_TIM3_TurnedOn = 1;
}


/**
 * Ф-ция выключающая таймер3
 */
void TIM3_TurnOff(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE); // выключаем тактирование таймера
	is_TIM3_TurnedOn = 0;
}


/**
 * Функция-обработчик прерываний таймера3
 */
void TIM3_IRQHandler(void)
{
	if (arrayStatus[0] == 1 || arrayStatus[1] == 1) {
		u16 fillingArrayNum;

		if (arrayStatus[0] == 1)
			fillingArrayNum = 0;
		else
			fillingArrayNum = 1;



		if ((countAll % TIM3_IRPer == 0) && (ir_temp_count[fillingArrayNum] != ir_temp_capacity))
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_7);
			MK_DelayMKS(lightTime);
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			GPIO_SetBits(GPIOC, GPIO_Pin_7);
			MK_DelayMKS(lightTime);
			ir_temp[fillingArrayNum][ir_temp_count[fillingArrayNum]] = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
			ir_temp_count[fillingArrayNum]++;

			if (ir_temp_count[fillingArrayNum] == ir_temp_capacity)
			{
				setArrayStatusToFull(fillingArrayNum);
				MainLoop_Config.DispatchData = 1;
			}
		}

		if ((countAll % TIM3_RPer == 0) && (red_temp_count[fillingArrayNum] != red_temp_capacity))
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_6);
			MK_DelayMKS(lightTime);
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			GPIO_SetBits(GPIOC, GPIO_Pin_6);
			MK_DelayMKS(lightTime);
			red_temp[fillingArrayNum][red_temp_count[fillingArrayNum]] = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
			red_temp_count[fillingArrayNum]++;
			if (red_temp_count[fillingArrayNum] == red_temp_capacity)
			{
				setArrayStatusToFull(fillingArrayNum);
				MainLoop_Config.DispatchData = 1;
			}
		}

		if ((countAll % TIM3_SRPer == 0) && (sr_temp_count[fillingArrayNum] != sr_temp_capacity))
		{
			u32 vOut = ((MK_GetBatteryVoltage() * 1000) / 4096 * (u32)ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2)) / 1000 ;
			u32 vOp = (u32)MK_GetBatteryVoltage()/32;
			sr_temp[fillingArrayNum][sr_temp_count[fillingArrayNum]] = ((vOut - vOp) * 68) / vOp / 2;

//			sr_temp[fillingArrayNum][sr_temp_count[fillingArrayNum]]
			sr_temp_count[fillingArrayNum]++;
			if (sr_temp_count[fillingArrayNum] == sr_temp_capacity)
			{
				setArrayStatusToFull(fillingArrayNum);
				MainLoop_Config.DispatchData = 1;
			}
		}
	}

	if (countAll % TIM3_BatteryPer == 0) {
		MainLoop_Config.ChekBatteryVoltage = 1;
	}

	if (countAll % TIM3_TimePer == 0) {
			countSec++;
			char strout[12];
			sprintf(strout,"%d", countSec);
			ssd1306_Draw6x8Str(8,0,strout,0,0);
	}

	countAll++;
	TIM3->SR &=~ TIM_SR_UIF;
}
