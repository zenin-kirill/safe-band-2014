
#include <stdbool.h>

extern bool is_ADC1_TurnedOn;		//���1 �������?

#define ir_temp_capacity 512	// ������� ��������� ��������� ��. ���������
#define red_temp_capacity 512 	// ������� ��������� ��������� ��. ���������
#define sr_temp_capacity 8  	// ������� ��������� ��������� ��

extern u16 arrayStatus[2];

extern u16 ir_temp[2][ir_temp_capacity];		// ��. ��������. ��������� ���������
extern u16 ir_temp_count[2];

extern u16 red_temp[2][red_temp_capacity];		// ��. ��������. ��������� ���������
extern u16 red_temp_count[2];

extern u16 sr_temp[2][sr_temp_capacity];		// ���.	��������� ����������� ���������
extern u16 sr_temp_count[2];

extern u16 BatteryVoltage;			//���������� Vbat , ���������� 0.

extern u16 getAnyArrayNum(u16 num);

/**
 * �-��� ������������� ADC1: 11,12,13,14 ��������.
 */
extern void ADC1_Init(void);


/**
 * �-��� ��������� ���1
 */
extern void ADC1_TurnOn(void);


/**
 * �-��� ���������� ���1
 */
extern void ADC1_TurnOff(void);


