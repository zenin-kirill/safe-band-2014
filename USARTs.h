#include <stdbool.h>


extern bool is_USART1_TurnedOn; 				// USART1 �������?
extern unsigned char USART1_ReciveBuf[32];
extern u8 USART1_ReciveCount;					//������� �������� ������

extern void USART1_BoostOn(u32 speed);

/**
 * �-��� ������������� USART1 �� �������� 9600 ���
 */
extern void USART1_Init(void);


/**
 * �-��� ��������� USART1
 */
extern void USART1_TurnOn(void);


/**
 * �-��� ���������� USART1
 */
extern void USART1_TurnOff(void);


/**
 * �-��� �������� ������ �� USART
 */
extern void USART1_SendString(const unsigned char *pucBuffer, u32 ulCount);


/**
 * �������-���������� �������� ���������� USART1
 */
extern void USART1_IRQHandler(void);
