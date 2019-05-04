
#include <stdbool.h>

extern bool is_ADC1_TurnedOn;		//АЦП1 включен?

#define ir_temp_capacity 512	// емкость хранилища измерений ик. диапазона
#define red_temp_capacity 512 	// емкость хранилища измерений кр. диапазона
#define sr_temp_capacity 8  	// емкость хранилища измерений ск

extern u16 arrayStatus[2];

extern u16 ir_temp[2][ir_temp_capacity];		// ик. диапазон. хранилище измерений
extern u16 ir_temp_count[2];

extern u16 red_temp[2][red_temp_capacity];		// кр. диапазон. хранилище измерений
extern u16 red_temp_count[2];

extern u16 sr_temp[2][sr_temp_capacity];		// кгр.	хранилище усредненных измерений
extern u16 sr_temp_count[2];

extern u16 BatteryVoltage;			//напряжение Vbat , изначально 0.

extern u16 getAnyArrayNum(u16 num);

/**
 * Ф-ция инициализации ADC1: 11,12,13,14 каналлов.
 */
extern void ADC1_Init(void);


/**
 * Ф-ция включения АЦП1
 */
extern void ADC1_TurnOn(void);


/**
 * Ф-ция выключения АЦП1
 */
extern void ADC1_TurnOff(void);


