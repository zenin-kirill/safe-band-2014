#include "stm32f10x.h"

extern bool is_GPIOAB_TurnedOn; 				//порты А B включены?
extern u16 ChargedBatteryVoltage;		//напряжение заряженной батареи
extern u16 DischargedBatteryVoltage;		//напряжение разряженной батареи


/**
 * Главная инициализация: конфигурирование портов МК.
 */
extern void MK_MainInit(void);

/**
* Ф-ция включающая порты.
*/
extern void GPIOAB_TurnOn(void);


/**
 * Ф-ция выключающая порты.
 */
extern void GPIOAB_TurnOff(void);


/**
 * Ф-ция простоя в циклах
 * 					~10 атом. оп. на входе
 * 					~8 атом. оп. на цикле
 * 					~6 атом. оп. на выходе
 * @param nCount количество циклов простоя
 */
extern void MK_Delay8MIPS(volatile uint32_t nCount);


/**
 * Ф-ция простоя в мс. при частоте 72 МГц. Очень примерно!!!!!
 * @param nCount количество миллисекунд простоя
 */
extern void MK_DelayMS(volatile uint32_t nCount);


/**
* Ф-ция простоя в мкс. при частоте 72 МГц. Очень примерно!!!!!
* @param nCount количество микросекунд простоя
*/
extern void MK_DelayMKS(volatile uint32_t nCount);


/**
 * Ф-ция включения микроконтроллера и запуска периферии после программного выключения.
 */
extern void MK_TurnOn(void);


/**
 * Ф-ция выключения микроконтроллера и периферии
 */
extern void MK_TurnOff(void);
