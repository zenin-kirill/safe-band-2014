/**
 * ����, ����������� ������ ������� ����������.
 *
 * �����: ����� ������
 * ������: 1.1
 *
 * ���������:
 * 	EXTI0 - ������ ���������, ���/���� ����������. PC0
 * 	EXTI15-10 - ������� �����, ������������ ������� �������� ������� �� USB: Vbus. PC10
 */


#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_exti.h"
#include "misc.h"

#include <stdbool.h>
#include "Algorithmes.h"
#include "Timers.h"
#include "SSD1306_I2C.h"


	u8 EXTI0_IRQn_Priority = 5;		//��������� ������ ���������
	u8 EXTI10_IRQn_Priority = 5;	//��������� ������� ����� USB
	bool is_MK_TurnedOn = 0;		//�� �������!? ���������� ���.
	bool is_Vbus = 0;				//���� �� Vbus?
	u16 lcdOutput = 0; 	// 0 - ir, 1 - red, 2 - sr, 3 - ������


/**
 * �-��� ������������� ������ ��������� - EXTI0
 */
void PowerButton_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN , ENABLE); 	//��������� �����������. �-��� �����

	AFIO->EXTICR[0>>0x02]|=AFIO_EXTICR1_EXTI0_PC;			//���������� � EXTI �� PC ����������
	EXTI->IMR|=(1<<0);										//���������� �� ����� 0 ����������
	EXTI->RTSR|=(1<<0);										//���������� ���������� �� �������������
	NVIC_EnableIRQ (EXTI0_IRQn);							//�������� ����������
	NVIC_SetPriority (EXTI0_IRQn,EXTI0_IRQn_Priority);
}

/**
 * �-��� ������������� ������� ����� USB
 */
void USBInputLine_Init(void)
{
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource10);	//������������� PC10

	EXTI_InitTypeDef   EXTI_InitStructure;

	EXTI_InitStructure.EXTI_Line = EXTI_Line10;						//��������� ����� 10
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				//������� ����������
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;			//�� ������
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_EnableIRQ(EXTI15_10_IRQn);
}


/**
 * �������-���������� ���������� �� �����0 (������ ���������)
 */
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0))
	{
		EXTI_ClearITPendingBit(EXTI_Line0);
	    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0))
	    {
	    	MK_DelayMS(1000);
	    	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0))
	    	{
	    		if (is_MK_TurnedOn)
	    			MK_TurnOff();
	    		else
	    			MK_TurnOn();
	    	}
	    	else {
	    		switch (lcdOutput) {
	    			case 0: lcdOutput = 1; break;
	    			case 1: lcdOutput = 2; break;
	    			case 2: lcdOutput = 3; break;
	    			case 3: lcdOutput = 0; break;
	    			default: lcdOutput = 0; break;
	    		}
	    		ssd1306_Clear();

	    		MainLoop_Config.UpdateOLED = true;
	    	}
	    }
	 }
}


/**
 * �������-���������� ���������� �� ������ 10-15 (������. Vbus)
 */
void EXTI15_10_IRQHandler()
{
	if (EXTI_GetITStatus(EXTI_Line10))
	{
	    MK_DelayMS(1000);
	    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10))
	    {
	    	if (!is_MK_TurnedOn)
	    	{
	    		GPIOAB_TurnOn();
	    		ADC1_TurnOn();
	    		TIM3_TurnOn();
	    		I2C1_TurnOn();
	    		ssd1306_TurnOn();
	    		ssd1306_Init();
	    	}

	    	MainLoop_Config.ChekBatteryCharge = 1;
	    	TIM3_Start();
	    }
	}
	EXTI_ClearITPendingBit(EXTI_Line10);
}
