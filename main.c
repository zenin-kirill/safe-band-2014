/* Мониторинг ПФПЧ. Главный файл, в котором содержится основная инициализация МК и запуск остальных модулей.
 *
 * Автор: Зенин Кирилл
 * Версия: 1.0
 *
 * Описание:
 *
 *  PA9 = UART1 TX (!!!исправить схему, проверить устройство)
 *  PA10 = UART1 RX скорость = 9600 !блютуз!
 *  PA15 = ц.выход, низкая частота, стандартно = выс.ур.  вкл. зар. = низкий уровень  !управление зарядкой!
 *
 *  PB4 = ц.выход, низкая частота, стандартно = выс.ур.  низ.ур. = ребут !ребут блютуз!
 *  PB6 = I2C SCL
 *  PB7 = I2C SDA !дисплей!
 *  PB9 = ц.вход, подтяжка к Vcc, низкая частота, низ.ур. = вкл !зарядка идет! (из усторойства можно исключить внешнюю подтяжку)
 *
 *  PC0 = ц.вход, подтяжка к земле, низкая частота, выс.ур. = прерывание !кнопка включения утройства!
 *  PC1 = аналог.вход №11, 7.5 циклов !датчик ЧСС!
 *  PC2 = аналог.вход №12, 7.5 циклов !датчик КГР!
 *  PC3 = аналог.вход №13, 7.5 циклов !датчик дыхания!
 *  PC4 = аналог.вход №14, 7.5 циклов !датчик напряжения аккумулятора!
 *  PC6 = ц.выход, средняя частота, выс.ур. = вкл !управление кр. светодиодами!
 *  PC7 = ц.выход, средняя частота, выс.ур. = вкл !управление ик. светодиодами!
 *  PC8 = ц.выход, низкая частота !управление вибромотором!
 *  PC9 = ц.выход, низкая частота !управление питанием активных элементов!
 *  PC10 = ц.вход, с подтяжкой к земле, низкая частота, выс.ур. = прерывание !определение USB!
 *
 */


#include "stdbool.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include <misc.h>

#include "Main.h"
#include "EXTIs.h"
#include "Timers.h"
#include "USARTs.h"
#include "I2Cs.h"
#include "ADCs.h"
#include "SSD1306_I2C.h"
#include "Algorithmes.h"
#include "Stdio.h"

	char imageBat100[] =
	{
		0x00, 0xe0, 0xfe, 0x02, 0xfa, 0x5a, 0xaa, 0x5a,
		0xaa, 0xfa, 0x02, 0xfa, 0x5a, 0xaa, 0x5a, 0xfa,
		0x02, 0xfa, 0x5a, 0xaa, 0x5a, 0xfa, 0x02, 0xfa,
		0x5a, 0xaa, 0x5a, 0xaa, 0xfa, 0x02, 0xfe, 0x00,
		0x00, 0x07, 0x7f, 0x40, 0x5f, 0x55, 0x5a, 0x55,
		0x5a, 0x5f, 0x40, 0x5f, 0x55, 0x5a, 0x55, 0x5f,
		0x40, 0x5f, 0x55, 0x5a, 0x55, 0x5f, 0x40, 0x5f,
		0x55, 0x5a, 0x55, 0x5a, 0x5f, 0x40, 0x7f, 0x00
	};

	char imageBat80[] =
	{
		0x00, 0xe0, 0xfe, 0x02, 0x02, 0x02, 0x02, 0x02,
		0x02, 0x02, 0x02, 0xfa, 0x5a, 0xaa, 0x5a, 0xfa,
		0x02, 0xfa, 0x5a, 0xaa, 0x5a, 0xfa, 0x02, 0xfa,
		0x5a, 0xaa, 0x5a, 0xaa, 0xfa, 0x02, 0xfe, 0x00,
		0x00, 0x07, 0x7f, 0x40, 0x40, 0x40, 0x40, 0x40,
		0x40, 0x40, 0x40, 0x5f, 0x55, 0x5a, 0x55, 0x5f,
		0x40, 0x5f, 0x55, 0x5a, 0x55, 0x5f, 0x40, 0x5f,
		0x55, 0x5a, 0x55, 0x5a, 0x5f, 0x40, 0x7f, 0x00
 	};

	char imageBat60[] =
	{
		0x00, 0xe0, 0xfe, 0x02, 0x02, 0x02, 0x02, 0x02,
		0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
		0x02, 0xfa, 0x5a, 0xaa, 0x5a, 0xfa, 0x02, 0xfa,
		0x5a, 0xaa, 0x5a, 0xaa, 0xfa, 0x02, 0xfe, 0x00,
		0x00, 0x07, 0x7f, 0x40, 0x40, 0x40, 0x40, 0x40,
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
		0x40, 0x5f, 0x55, 0x5a, 0x55, 0x5f, 0x40, 0x5f,
		0x55, 0x5a, 0x55, 0x5a, 0x5f, 0x40, 0x7f, 0x00
	};

	char imageBat40[] =
	{
		0x00, 0xe0, 0xfe, 0x02, 0x02, 0x02, 0x02, 0x02,
		0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
		0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0xfa,
		0x5a, 0xaa, 0x5a, 0xaa, 0xfa, 0x02, 0xfe, 0x00,
		0x00, 0x07, 0x7f, 0x40, 0x40, 0x40, 0x40, 0x40,
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x5f,
		0x55, 0x5a, 0x55, 0x5a, 0x5f, 0x40, 0x7f, 0x00
	};

	char imageBat20[] =
	{
		0x00, 0xe0, 0xfe, 0x02, 0x02, 0x02, 0x02, 0x02,
		0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
		0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
		0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0xfe, 0x00,
		0x00, 0x07, 0x7f, 0x40, 0x40, 0x40, 0x40, 0x40,
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7f, 0x00
	};

	char imageBatCharge[] =
	{
		0x00, 0xe0, 0xfe, 0x02, 0x02, 0x12, 0x12, 0x12,
		0x12, 0x32, 0x32, 0x32, 0x32, 0x72, 0x72, 0xf2,
		0xf2, 0xf2, 0x72, 0x32, 0x12, 0x02, 0x02, 0x02,
		0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0xfe, 0x00,
		0x00, 0x07, 0x7f, 0x40, 0x40, 0x40, 0x40, 0x40,
		0x40, 0x40, 0x40, 0x48, 0x4c, 0x4e, 0x4f, 0x4f,
		0x4f, 0x4e, 0x4e, 0x4e, 0x4c, 0x4c, 0x4c, 0x4c,
		0x48, 0x48, 0x48, 0x48, 0x40, 0x40, 0x7f, 0x00
	};

	char imageSignalGraphArray[128][6];
	char imageSignalGraph[768];

	u16 BLinit = 1;
	bool lcdUpdated = 0;
	bool dataLoading = false;
	u16 lcdBuffer[512];	// независимый буффер вывода на экран


/**
 * Главная инициализация: конфигурирование портов МК.
 */
void MK_MainInit(void)
{
	SystemInit();

	GPIO_InitTypeDef PORT;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);	//тактируем порт А
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);	//тактируем порт B
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);	//тактируем порт С

	PORT.GPIO_Pin = GPIO_Pin_15;				 			//порт управления зарядкой
	PORT.GPIO_Mode = GPIO_Mode_Out_PP;						//пуш-пул
	PORT.GPIO_Speed = GPIO_Speed_2MHz;						//тактируем по минимуму
	GPIO_Init(GPIOA, &PORT);

	PORT.GPIO_Pin = GPIO_Pin_4;				 				//порт перезагрузки блютуза
	PORT.GPIO_Mode = GPIO_Mode_Out_PP;						//пуш-пул
	PORT.GPIO_Speed = GPIO_Speed_2MHz;						//тактируем по минимуму
	GPIO_Init(GPIOB, &PORT);

	PORT.GPIO_Pin = (GPIO_Pin_0 | GPIO_Pin_10 );			//порт кнопки включения
	PORT.GPIO_Mode = GPIO_Mode_IPD;							//с подт. к земле
	PORT.GPIO_Speed = GPIO_Speed_2MHz;						//тактируем по минимуму
	GPIO_Init(GPIOC, &PORT);


	PORT.GPIO_Pin = ( GPIO_Pin_6 | GPIO_Pin_7 ); 			//порты управления светодиодами
	PORT.GPIO_Mode = GPIO_Mode_Out_PP;						//пуш-пул
	PORT.GPIO_Speed = GPIO_Speed_10MHz;						//тактируем средне
	GPIO_Init(GPIOC, &PORT);

	PORT.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;				//I2C1 SDA SCL
	PORT.GPIO_Mode = GPIO_Mode_AF_OD;						//альтернативный с подтяжкой
	PORT.GPIO_Speed = GPIO_Speed_50MHz;						//тактируем по минимуму
	GPIO_Init(GPIOB, &PORT);


	PORT.GPIO_Pin = (GPIO_Pin_9);							//порт определения зарядки
	PORT.GPIO_Mode = GPIO_Mode_IPD;							//с подт. к земле
	PORT.GPIO_Speed = GPIO_Speed_2MHz;						//тактируем по минимуму
	GPIO_Init(GPIOB, &PORT);

	PORT.GPIO_Pin = ( GPIO_Pin_8 | GPIO_Pin_9 ); 			//порты управления вибромотором и контр. акт. устр.
	PORT.GPIO_Mode = GPIO_Mode_Out_PP;						//пуш-пул
	PORT.GPIO_Speed = GPIO_Speed_2MHz;						//тактируем по минимуму
	GPIO_Init(GPIOC, &PORT);

	PORT.GPIO_Pin = ( GPIO_Pin_1 | GPIO_Pin_2 				//порты АЦП
					| GPIO_Pin_3 | GPIO_Pin_4 );
	PORT.GPIO_Mode =  GPIO_Mode_AIN;
	PORT.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &PORT);

	PORT.GPIO_Pin = GPIO_Pin_9;								//USART TX
	PORT.GPIO_Speed = GPIO_Speed_50MHz;						//тактируем хорошо
	PORT.GPIO_Mode = GPIO_Mode_AF_PP;						//пуш-пул
	GPIO_Init(GPIOA, &PORT);

	PORT.GPIO_Pin = GPIO_Pin_10;							//USART RX
	PORT.GPIO_Speed = GPIO_Speed_50MHz;						//тактируем хорошо
	PORT.GPIO_Mode = GPIO_Mode_IN_FLOATING;					//висячий
	GPIO_Init(GPIOA, &PORT);

}

void setArrayStatusToReady(u16 arrayNum) {
	ir_temp_count[arrayNum] = 0;
	red_temp_count[arrayNum] = 0;
	sr_temp_count[arrayNum] = 0;

	if (lcdOutput != 3) {
	if (arrayNum == 0) {
		ssd1306_Draw6x8Str(72, 0, "0", 0, 0);
	} else {
		ssd1306_Draw6x8Str(80, 0, "0", 0, 0);
	}
	}

	arrayStatus[arrayNum] = 0;
}

void setArrayStatusToFilling(u16 arrayNum) {
	if (lcdOutput != 3) {
	if (arrayNum == 0) {
		ssd1306_Draw6x8Str(72, 0, "1", 0, 0);
	} else {
		ssd1306_Draw6x8Str(80, 0, "1", 0, 0);
	}
	}

	arrayStatus[arrayNum] = 1;
}

void setArrayStatusToFull(u16 arrayNum) {
	if (ir_temp_count[arrayNum] == ir_temp_capacity
		&& red_temp_count[arrayNum] == red_temp_capacity
		&& sr_temp_count[arrayNum] == sr_temp_capacity) {

		if (lcdOutput != 3) {
		if (arrayNum == 0) {
			ssd1306_Draw6x8Str(72, 0, "2", 0, 0);
		} else {
			ssd1306_Draw6x8Str(80, 0, "2", 0, 0);
		}
		}

		arrayStatus[arrayNum] = 2;
	}
}

void setArrayStatusToSending(u16 arrayNum) {
	u32 f = 0;

	if (dataLoading == true) {
		MainLoop_Config.DispatchData = true;
		return;
	}

	switch (lcdOutput) {
		case 0:
			for (f = 0; f < 512; f++)
				lcdBuffer[f] = ir_temp[arrayNum][f];
				lcdUpdated = true;
			break;
		case 1:
			for (f = 0; f < 512; f++)
				lcdBuffer[f] = red_temp[arrayNum][f];
				lcdUpdated = true;
			break;
		case 2:
			for (f = 0; f < 8; f++) {
				for (j = 0; j < 64; j++){
					lcdBuffer[(f*64) + j] = sr_temp[arrayNum][f];
				}
			}

			lcdUpdated = true;
			break;
		default:
			break;
	}

	MainLoop_Config.SendDataToBluetooth = true;
	MainLoop_Config.UpdateOLED = true;

	if (lcdOutput != 3) {
	if (arrayNum == 0) {
		ssd1306_Draw6x8Str(72, 0, "3", 0, 0);
	} else {
		ssd1306_Draw6x8Str(80, 0, "3", 0, 0);
	}
	}

	arrayStatus[arrayNum] = 3;
}


/**
 * Главная функция.
 */
int main(void)
{
	MK_MainInit();
	MK_StructInit();
	GPIO_SetBits(GPIOC, GPIO_Pin_6);
	GPIO_SetBits(GPIOC, GPIO_Pin_7);
	MK_DelayMS(1000);

	GPIO_SetBits(GPIOC, GPIO_Pin_8);
	MK_DelayMS(50);
	GPIO_ResetBits(GPIOC, GPIO_Pin_8);

	ADC1_Init();
	ADC1_TurnOn();
	BatteryVoltage = MK_GetBatteryVoltage();			// cчитываем Vbat


	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10))		// если Vbus подан
	{

		is_Vbus = 1;									// выставляем флаг наличия Vbus
		GPIO_ResetBits(GPIOA , GPIO_Pin_15);			// разрешаем зарядку
		MK_DelayMS(100);

		TIM3_Init();
		TIM3_TurnOn();
		I2C1_Init();
		I2C1_TurnOn();
		ssd1306_Init();
		ssd1306_TurnOn();
		ssd1306_Draw12x16Str(0,25,"Питание ОК",0);
		MK_DelayMS(500);
		ssd1306_Clear();

		MainLoop_Config.ChekBatteryCharge = 1;
		TIM3_Start();

		PowerButton_Init();
		USBInputLine_Init();

	}
	else
	{
		TIM3_Init();
		I2C1_Init();
		I2C1_TurnOn();
		ssd1306_Init();

		ssd1306_TurnOn();
		ssd1306_Draw12x16Str(0,25,"Питание ОК",0);
		MK_DelayMS(500);
		ssd1306_Clear();
		ssd1306_TurnOff();

		PowerButton_Init();
		USBInputLine_Init();

		I2C1_TurnOff();
		ADC1_TurnOff();
		GPIOAB_TurnOff();
	}
	MK_TurnOn();

	while (1) //главный непрерывный цикл
	{
		//Блок проверки Vbus и состояния зарядки,  а так же сохранения статуса зарядки, и выдача ошибки.

		if (MainLoop_Config.ChekBatteryCharge)
		{
			MainLoop_Config.ChekBatteryCharge = 0;
			BatteryVoltage = MK_GetBatteryVoltage();

			if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10))
			{
				is_Vbus = 1;
				if ((!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)) && (!is_MK_Charge))  //если зарядка идет и есть что заряжать
				{
					MK_DelayMS(100);
					if ((!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)))
					{
						is_MK_Charge = 1;
						ssd1306_Clear();
						ssd1306_Draw12x16Str(10,16,"Зарядка",0);
						ssd1306_Draw12x16Str(0,32,"подключена!",0);
						MK_DelayMS(500);
						OLED_Config.ShowBatteryCharge = 1;
						ssd1306_Clear();
					}
				}

				if ((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)) && (BatteryVoltage < ChargedBatteryVoltage))  //если зарядка НЕ идет и есть что заряжать
				{
					MK_DelayMS(1000);
					if ((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)) && (BatteryVoltage < ChargedBatteryVoltage))
					{
						is_MK_Charge = 0;
						ssd1306_Clear();
						ssd1306_Draw12x16Str(12,16,"Ошибка",0);
						ssd1306_Draw12x16Str(10,32,"Зарядки!",0);
						GPIO_SetBits(GPIOA , GPIO_Pin_15);	//остановить зарядку
						MK_DelayMS(1000);
						GPIO_ResetBits(GPIOA , GPIO_Pin_15);//перезапустить зарядку
						MainLoop_Config.ChekBatteryCharge = 1;
						ssd1306_Clear();
						OLED_Config.ShowBatteryCharge = 0;
					}
				}

				if((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)) && (BatteryVoltage >= ChargedBatteryVoltage))	//если зардка НЕ идет  но напряжение достаточно высокое
				{
					is_MK_Charge = 0;
					ssd1306_Clear();
					ssd1306_Draw12x16Str(10,16,"Зарядка",0);
					ssd1306_Draw12x16Str(0,32,"завершена!",0);
					MK_DelayMS(500);
					OLED_Config.ShowBatteryCharge = 0;

				}
			}
			else
			{
				if (is_MK_Charge)
				{
					is_MK_Charge = 0;
					ssd1306_Clear();
					ssd1306_Draw12x16Str(10,16,"Зарядка",0);
					ssd1306_Draw12x16Str(0,32,"отключена!",0);
					MK_DelayMS(500);
					ssd1306_Clear();
				}
				is_MK_Charge = 0;
				is_Vbus = 0;
				OLED_Config.ShowBatteryCharge = 0;
				if (!is_MK_TurnedOn)
					MK_TurnOff();
			}

		}

		//Блок проверки напряжения аккумулятора, и сохранения результатов

		if (MainLoop_Config.ChekBatteryVoltage)
		{
			MainLoop_Config.ChekBatteryVoltage = 0;

			u32 l = 0;
			BatteryVoltage = MK_GetBatteryVoltage();					//проверяем напряжение аккумулятора
			if (is_Vbus != GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10)) 	//проверяем Vbus и сохраняем
				MainLoop_Config.ChekBatteryCharge = 1;

			if ((BatteryVoltage < DischargedBatteryVoltage) && (!is_Vbus))
			{
				for(l = 0; l<5; l++)
				{
					ssd1306_Clear();
					MK_DelayMS(500);
					ssd1306_Draw12x16Str(10,16,"Батарея",0);
					ssd1306_Draw12x16Str(0,32,"разряжена!",0);
					MK_DelayMS(500);
				}
				ssd1306_Clear();
				TIM3_Stop();
				MK_TurnOff();
			}
			else
				if ((is_MK_TurnedOn) && (!OLED_Config.ShowBatteryVoltage))
					OLED_Config.ShowBatteryVoltage = 1;
		}

		// Блок диспетчеризации потоков данных

		if (MainLoop_Config.DispatchData) {

			MainLoop_Config.DispatchData = 0;

			if (((arrayStatus[0] == 0) && (arrayStatus[1] == 0))		// ready0 	&& ready1
			|| ((arrayStatus[0] == 0) && (arrayStatus[1] == 2))			// ready0	&& full1
			|| ((arrayStatus[0] == 0) && (arrayStatus[1] == 3))) {		// ready0	&& sending1
				setArrayStatusToFilling(0);								// ready0	-> filling0
			}

			if (((arrayStatus[0] == 2) && (arrayStatus[1] == 0))		// full0	&& ready1
			|| ((arrayStatus[0] == 3) && (arrayStatus[1] == 0))) {		// sending0	&& ready1
				setArrayStatusToFilling(1);								// ready1	-> filling1
			}

			if (((arrayStatus[0] == 2) && (arrayStatus[1] == 1))		// full0	&& filling1
			|| ((arrayStatus[0] == 2) && (arrayStatus[1] == 2))) {		// full0	&& full1
				setArrayStatusToSending(0);								// full0 	-> sending0
			}

			if ((arrayStatus[0] == 1) && (arrayStatus[1] == 2)) {		// filling0	&& full1
				setArrayStatusToSending(1);								// full1	-> sending1
			}

			if (((arrayStatus[0] == 1) && (arrayStatus[1] == 1))		// filling0	&& filling1 недопустимо!
			|| ((arrayStatus[0] == 3) && (arrayStatus[1] == 3))) {		// sending0	&& sending1 недопустимо!
				setArrayStatusToReady(0);								// *0 		-> ready0
				setArrayStatusToReady(1);								// *1 		-> ready1
				MainLoop_Config.DispatchData = 1;
			}
		}

		if (MainLoop_Config.UpdateOLED)
				{
					MainLoop_Config.UpdateOLED = 0;

					u32 h = 0;

						if (OLED_Config.ShowDataGraph && lcdOutput != 3)
						{
							if (lcdUpdated == true) {
								for (h = 0; h < 128; h++) {
									for (j = 0; j < 6; j++) {
										imageSignalGraphArray[h][j] = 0;
									}
								}

								u16 n = 512;
								u16 max = 0;
								u16 min = 10000;

								for (h = 0; h < n; h++) {
									if (lcdBuffer[h] > max)
										max = lcdBuffer[h];
									if (lcdBuffer[h] < min)
										min = lcdBuffer[h];
								}

								char strout[12];
								sprintf(strout,"%d - %d", min, max);
								ssd1306_Draw6x8Str(0,1,strout,0,0);


								u16 mul = 4096 / (max - min);

								for (h = 0; h < n; h++) {
									lcdBuffer[h] = (lcdBuffer[h] - min) * mul;
								}


								for (h = 0; h < 128; h++) {
									u16 outSig = 47 - (lcdBuffer[h*4] + lcdBuffer[h*4+1] + lcdBuffer[h*4+2] + lcdBuffer[h*4+3]) / 4 / 86;
									u16 byteNum = outSig / 8;  	// приведение к масштабу 0..47 и получение изменяемого байта
									u16 bitPos = 7 - outSig % 8;  		// приведение к масштабу 0..47 и получение изменяемого бита
									imageSignalGraphArray[h][byteNum] |= 1 << (7-bitPos);	// втягиваем единичку
								}

								int c = 0;

								for (h = 0; h < 6;  h++) {
									for (j = 0; j < 128; j++) {
										imageSignalGraph[c] = imageSignalGraphArray[j][h];
										c++;
									}
								}

								lcdUpdated = false;
							}

							ssd1306_DrawImage(0,16,128,48, imageSignalGraph,0);
						}

						if (OLED_Config.ShowBatteryVoltage)
						{
							int value = MK_GetBatteryVoltage();

							if (!is_MK_Charge)
							{
								if (value > 3900)
									ssd1306_DrawImage(96,0,32,16,imageBat100,0);
								if ((value > 3800) && (value <= 3900))
									ssd1306_DrawImage(96,0,32,16,imageBat80,0);
								if ((value > 3700) && (value <= 3800))
									ssd1306_DrawImage(96,0,32,16,imageBat60,0);
								if ((value > 3600) && (value <= 3800))
									ssd1306_DrawImage(96,0,32,16,imageBat40,0);
								if (value <= 3600)
									ssd1306_DrawImage(96,0,32,16,imageBat20,0);
							}

							// Прием и отрисовка данных
							//ssd1306_Draw6x8Str(0,2,USART1_ReciveBuf,0,0);

							// char strout[4];
							// sprintf(strout,"%d,%d%d", (BatteryVoltage/1000), (BatteryVoltage%1000/100), (BatteryVoltage%100/10));
							// ssd1306_Draw6x8Str(0,0,"V:",0,0);
							// ssd1306_Draw6x8Str(16,0,strout,0,0);

							switch (lcdOutput) {
								case 0: 	ssd1306_Draw6x8Str(0,0,"i",0,0); break;
								case 1: 	ssd1306_Draw6x8Str(0,0,"r",0,0); break;
								case 2: 	ssd1306_Draw6x8Str(0,0,"s",0,0); break;
								case 3: 	ssd1306_Draw6x8Str(0,0,"n",0,0); break;
								default: 	ssd1306_Draw6x8Str(0,0,"?",0,0); break;
							}
						}

						if (OLED_Config.ShowBatteryCharge)
						{
							if (is_MK_TurnedOn)
								ssd1306_DrawImage(96,0,32,16,imageBatCharge,0);
							else
								ssd1306_DrawImage(96,0,32,16,imageBatCharge,0);
						}
				}

		if (MainLoop_Config.SendDataToBluetooth)
		{
			MainLoop_Config.SendDataToBluetooth = 0;

			if (BLinit == 1) {
			USART1_Init();
					USART1_TurnOn();
					MK_DelayMS(500);
					USART1_SendString("AT+NAME=SEBand", 14);
					MK_DelayMS(100);
					USART1_SendString("AT+ROLE=S", 9);
					MK_DelayMS(100);
					USART1_SendString("AT+CONT=0", 9);
					MK_DelayMS(100);
					USART1_SendString("AT+RFPM=1", 9);
					MK_DelayMS(100);
					USART1_SendString("AT+RFPM=1", 9);
					MK_DelayMS(100);
					USART1_SendString("AT+BAUD=115200.E", 16);
					USART1_BoostOn(115200);
					BLinit=0;
			}

			if ((arrayStatus[0] == 3 || arrayStatus[1] == 3) && BLinit == 0)
			{
				u16 loadingArrayNum;
				u32 d = 0;

				if (arrayStatus[0] == 3) {
					loadingArrayNum = 0;
				} else {
					loadingArrayNum = 1;
				}

				dataLoading = true;
				ssd1306_Draw6x8Str(88,0,"#",0,0);

				char numToStr[10];
				USART1_SendString("[time]\n", 8);
				sprintf(numToStr, "%d\n", countSec);
				USART1_SendString(numToStr, strlen(numToStr));

				USART1_SendString("[irs]\n", 7);
				for (d = 0; d < ir_temp_capacity; d++) {
					sprintf(numToStr, "%d\n", ir_temp[loadingArrayNum][d]);
					USART1_SendString(numToStr, strlen(numToStr));
				}

				USART1_SendString("[rs]\n", 6);
				for (d = 0; d < red_temp_capacity; d++) {
					sprintf(numToStr, "%d\n", red_temp[loadingArrayNum][d]);
					USART1_SendString(numToStr, strlen(numToStr));
				}

				USART1_SendString("[srs]\n", 7);
				for (d = 0; d < sr_temp_capacity; d++) {
					sprintf(numToStr, "%d\n", sr_temp[loadingArrayNum][d]);
					USART1_SendString(numToStr, strlen(numToStr));
				}

				dataLoading = false;
				ssd1306_Draw6x8Str(88,0," ",0,0);

				setArrayStatusToReady(loadingArrayNum);
			}
		}
	}
	return 0;
}
