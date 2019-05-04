#include <stdbool.h>


extern bool is_MK_TurnedOn;  	//МК включен!? изначально да.
extern bool is_Vbus;			//есть ли Vbus?
extern u16 lcdOutput;	 		// 0 - ir, 1 - red, 2 - sr, 3 - ничего

/**
 * Ф-ция инициализации кнопки включения - EXTI0
 */
extern void PowerButton_Init(void);


/**
 * Ф-ция инициализации входной линии USB, EXTI15-10
 */
extern void USBInputLine_Init(void);
