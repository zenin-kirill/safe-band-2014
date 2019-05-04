#include <stdbool.h>


extern bool is_USART1_TurnedOn; 				// USART1 включен?
extern unsigned char USART1_ReciveBuf[32];
extern u8 USART1_ReciveCount;					//счетчик принятых данных

extern void USART1_BoostOn(u32 speed);

/**
 * Ф-ция инициализации USART1 на скорость 9600 бод
 */
extern void USART1_Init(void);


/**
 * Ф-ция включения USART1
 */
extern void USART1_TurnOn(void);


/**
 * Ф-ция выключения USART1
 */
extern void USART1_TurnOff(void);


/**
 * Ф-ция отправки строки по USART
 */
extern void USART1_SendString(const unsigned char *pucBuffer, u32 ulCount);


/**
 * Функция-обработчик входного прерывания USART1
 */
extern void USART1_IRQHandler(void);
