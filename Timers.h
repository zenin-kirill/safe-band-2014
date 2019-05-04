#include <stdbool.h>

extern u32 countSec;
extern bool is_TIM3_TurnedOn;					//TIM3 �������?
extern bool is_TIM3_Started;					//TIM3 �������?
extern bool is_MK_Charge;						//������� ����?

extern const unsigned char HelloText[];			//����� �����������
extern const unsigned char GoodByText[];		//����� ��������


extern struct MainLoop_ConfigStructure MainLoop_Config;

#ifndef MainLoop_ConfigStructure
typedef struct MainLoop_ConfigStructure			//��������� ������������ ��������� ����� ������
{
	bool ChekBatteryVoltage;					//�������� ����. ��������
	bool ChekBatteryCharge;						//�������� ������ �������
	bool DispatchData;							//�������� ������� ������
	bool SendDataToBluetooth;					//�������� ������ � ������ �������
	bool UpdateOLED;							//���������� ������
};
#endif

extern struct OLED_ConfigStructure OLED_Config;

#ifndef OLED_ConfigStructure
typedef struct OLED_ConfigStructure				//��������� ������������ ������ ���. �� �������
{
	bool ShowBatteryVoltage;					//���������� ���������� �� �������
	bool ShowBatteryCharge;						//���������� ������� �������
	bool ShowDataGraph;							//���������� ������� ������ ������
};
#endif

/**
 * �-��� ���������������� ������3, ���������� ������ ����������.
 */
extern void TIM3_Init(void);


/**
 * �-��� ���������� ������3
 */
extern void TIM3_TurnOn(void);


/**
 * �-��� ����������� ������3
 */
extern void TIM3_TurnOff(void);


/**
 * �-��� ����������� ������3
 */
extern void TIM3_Start(void);


/**
 * �-��� ��������������� ������3
 */
extern void TIM3_Stop(void);


/**
 *  �-��� ���������� ���������������� ���������
 */
extern void MK_StructInit(void);


/**
 *  �-��� ���������� ���������������� ��������� ��� ���������� �������
 */
extern void MK_ExceptChargeStructInit(void);
