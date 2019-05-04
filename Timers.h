#include <stdbool.h>

extern u32 countSec;
extern bool is_TIM3_TurnedOn;					//TIM3 включен?
extern bool is_TIM3_Started;					//TIM3 запущен?
extern bool is_MK_Charge;						//зарядка идет?

extern const unsigned char HelloText[];			//текст приветствия
extern const unsigned char GoodByText[];		//текст прощания


extern struct MainLoop_ConfigStructure MainLoop_Config;

#ifndef MainLoop_ConfigStructure
typedef struct MainLoop_ConfigStructure			//структура конфигурации основного цикла работы
{
	bool ChekBatteryVoltage;					//проверка напр. баттареи
	bool ChekBatteryCharge;						//проверка заряда батареи
	bool DispatchData;							//проверка частоты пульса
	bool SendDataToBluetooth;					//отправка данных в модуль блютуза
	bool UpdateOLED;							//обновление экрана
};
#endif

extern struct OLED_ConfigStructure OLED_Config;

#ifndef OLED_ConfigStructure
typedef struct OLED_ConfigStructure				//структура конфигурации вывода инф. на дисплей
{
	bool ShowBatteryVoltage;					//отображать напряжения на батарее
	bool ShowBatteryCharge;						//отображать зарядку батареи
	bool ShowDataGraph;							//отображать частоту пульса сердца
};
#endif

/**
 * Ф-ция инициализирующая таймер3, изначально таймер остановлен.
 */
extern void TIM3_Init(void);


/**
 * Ф-ция включающая таймер3
 */
extern void TIM3_TurnOn(void);


/**
 * Ф-ция выключающая таймер3
 */
extern void TIM3_TurnOff(void);


/**
 * Ф-ция запускающая таймер3
 */
extern void TIM3_Start(void);


/**
 * Ф-ция останавливающая таймер3
 */
extern void TIM3_Stop(void);


/**
 *  Ф-ция изначально инициализирующая структуры
 */
extern void MK_StructInit(void);


/**
 *  Ф-ция изначально инициализирующая структуры при включенной зарядке
 */
extern void MK_ExceptChargeStructInit(void);
