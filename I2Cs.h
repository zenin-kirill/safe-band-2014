#include <stdbool.h>
#include "stm32f10x.h"

extern  bool is_I2C1_TurnedOn;		//I2C1 включен?
extern  bool is_SSD1308_DisplayOn;	//Дисплей включен?

/**
 * Ф-ция инициализации I2C1, скорость работы 100КГц
 */
extern void I2C1_Init(void);


/**
 * Ф-ция подачи питания I2C1
 */
extern void I2C1_TurnOn(void);


/**
 * Ф-ция выключения питания I2C1
 */
extern void I2C1_TurnOff(void);


/**
 * Ф-ция запуска передачи данных.
 * @param HW_address Адрес устройства.
 */
extern void I2C1_Start(u8 HW_address);

/**
 * Ф-ция передачи данных.
 * @param *data Указатель на массив байтов, который требуется передать.
 * @param count Количество передаваемых байтов.
 */
extern void I2C1_Write(unsigned char *data, unsigned char count);


/**
 * Ф-ция окончания передачи данных.
 */
extern ErrorStatus I2C1_Stop(void);
