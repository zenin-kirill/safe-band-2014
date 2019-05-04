#include <stdbool.h>


extern bool is_MK_TurnedOn;  	//�� �������!? ���������� ��.
extern bool is_Vbus;			//���� �� Vbus?
extern u16 lcdOutput;	 		// 0 - ir, 1 - red, 2 - sr, 3 - ������

/**
 * �-��� ������������� ������ ��������� - EXTI0
 */
extern void PowerButton_Init(void);


/**
 * �-��� ������������� ������� ����� USB, EXTI15-10
 */
extern void USBInputLine_Init(void);
