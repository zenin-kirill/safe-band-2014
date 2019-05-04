#include <stdbool.h>
#include "stm32f10x.h"

extern  bool is_I2C1_TurnedOn;		//I2C1 �������?
extern  bool is_SSD1308_DisplayOn;	//������� �������?

/**
 * �-��� ������������� I2C1, �������� ������ 100���
 */
extern void I2C1_Init(void);


/**
 * �-��� ������ ������� I2C1
 */
extern void I2C1_TurnOn(void);


/**
 * �-��� ���������� ������� I2C1
 */
extern void I2C1_TurnOff(void);


/**
 * �-��� ������� �������� ������.
 * @param HW_address ����� ����������.
 */
extern void I2C1_Start(u8 HW_address);

/**
 * �-��� �������� ������.
 * @param *data ��������� �� ������ ������, ������� ��������� ��������.
 * @param count ���������� ������������ ������.
 */
extern void I2C1_Write(unsigned char *data, unsigned char count);


/**
 * �-��� ��������� �������� ������.
 */
extern ErrorStatus I2C1_Stop(void);
