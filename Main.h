#include "stm32f10x.h"

	extern u32 i;
	extern u32 j;



	extern u8 count20;
	extern u32 countAll;

// Значения АЦП
	extern u16 red_hr_temp[3];
	extern u16 red_hr[500];

	extern u16 ik_hr_temp[3];
	extern u16 ik_hr[500];

	extern u16 kgr_temp[3];
	extern u16 kgr[2560];

//// Функции

// Ф-ция ожидания
	extern void MK_DelayMIPS(volatile uint32_t nCount);
// Ф-ция передачи строки
	extern void USART2_SendString(const unsigned char *pucBuffer, unsigned long ulCount);
