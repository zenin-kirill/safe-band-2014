/**
 * ����, ����������� ������ �������-��������� ���������������.
 *
 * �����: ����� ������
 * ������: 1.0
 *
 * ���������:
 * 	ADC1:
 * 	 	PC1 = ����� �11, 7.5 ������ !������ ���!
 * 		PC2 = ����� �12, 7.5 ������ !������ ���!
 * 		PC3 = ����� �13, 7.5 ������ !������ �������!
 * 		PC4 = ����� �14, 7.5 ������ !������ ���������� ������������!
 */

#include <stdbool.h>
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"

#include "ADCs.h"

	bool is_ADC1_TurnedOn = 0;

	u16 arrayStatus[2] = {0, 0};			// ������� ���� ��������

	u16 ir_temp[2][ir_temp_capacity];		// ��. ��������. ��������� ���������
	u16 ir_temp_count[2] = {0,0};

	u16 red_temp[2][red_temp_capacity];		// ��. ��������. ��������� ���������
	u16 red_temp_count[2] = {0,0};

	u16 sr_temp[2][sr_temp_capacity];		// ���.	��������� ����������� ���������
	u16 sr_temp_count[2] = {0,0};

	u16 BatteryVoltage = 0; 				//���������� ������� �� ������������


/**
 * �-��� ������������� ADC1: 11,12,13,14 ��������.
 */
void ADC1_Init(void)
{
	ADC_InitTypeDef ADC;

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//��������� ADC1

	ADC.ADC_Mode = ADC_Mode_Independent;											//�� � Dual Mode
	ADC.ADC_ScanConvMode = ENABLE;													//������������ �������.
	ADC.ADC_ContinuousConvMode = DISABLE;											//��������� ��������������.
	ADC.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;							//������ ��������
	ADC.ADC_DataAlign = ADC_DataAlign_Right;										//������������ �� ������� ����.
	ADC.ADC_NbrOfChannel = 4;														//����� �������.
	ADC_Init(ADC1, &ADC);															//��������������.
	ADC_InjectedSequencerLengthConfig(ADC1, 4); 									//������������� ���������� �������, ������� �� ���������� ���������.
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_239Cycles5);	//!���!
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_12, 2, ADC_SampleTime_239Cycles5);	//!���� ������� && ��!
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_13, 3, ADC_SampleTime_7Cycles5);	//!���������!
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_14, 4, ADC_SampleTime_7Cycles5);	//!������ ���������� ������������!
	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_None);		//������������ �������������� = 7.5 ������
	ADC_AutoInjectedConvCmd(ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);


	//ADC_SoftwareStartConvCmd(ADC1, ENABLE); ������ ��������������
	//ADC_GetSoftwareStartConvStatus()
}


/**
 * �-��� ��������� ���1
 */
void ADC1_TurnOn(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	//ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
	//NVIC_EnableIRQ(ADC1_2_IRQn);
	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

	is_ADC1_TurnedOn = 1;
}


/**
 * �-��� ���������� ���1
 */
void ADC1_TurnOff(void)
{
	ADC_Cmd(ADC1, DISABLE);
	//NVIC_DisableIRQ(ADC1_2_IRQn);
	//ADC_ITConfig(ADC1, ADC_IT_JEOC, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);

	is_ADC1_TurnedOn = 0;
}

/**
 * �������-���������� ���������� �� ��������� �������������� ���
 *
void ADC1_2_IRQHandler(void)
{
	ADC1->SR&=~ADC_SR_JEOC;
	// = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
	// = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2);
	// = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_3);
	// = (1800 / ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_4))*4048;
	//ADC_Data_TX = EN;
} */
