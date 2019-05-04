#include "stm32f10x.h"

extern bool is_GPIOAB_TurnedOn; 				//����� � B ��������?
extern u16 ChargedBatteryVoltage;		//���������� ���������� �������
extern u16 DischargedBatteryVoltage;		//���������� ����������� �������


/**
 * ������� �������������: ���������������� ������ ��.
 */
extern void MK_MainInit(void);

/**
* �-��� ���������� �����.
*/
extern void GPIOAB_TurnOn(void);


/**
 * �-��� ����������� �����.
 */
extern void GPIOAB_TurnOff(void);


/**
 * �-��� ������� � ������
 * 					~10 ����. ��. �� �����
 * 					~8 ����. ��. �� �����
 * 					~6 ����. ��. �� ������
 * @param nCount ���������� ������ �������
 */
extern void MK_Delay8MIPS(volatile uint32_t nCount);


/**
 * �-��� ������� � ��. ��� ������� 72 ���. ����� ��������!!!!!
 * @param nCount ���������� ����������� �������
 */
extern void MK_DelayMS(volatile uint32_t nCount);


/**
* �-��� ������� � ���. ��� ������� 72 ���. ����� ��������!!!!!
* @param nCount ���������� ����������� �������
*/
extern void MK_DelayMKS(volatile uint32_t nCount);


/**
 * �-��� ��������� ���������������� � ������� ��������� ����� ������������ ����������.
 */
extern void MK_TurnOn(void);


/**
 * �-��� ���������� ���������������� � ���������
 */
extern void MK_TurnOff(void);
