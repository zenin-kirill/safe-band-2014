/**
 * ����, ����������� ������ ����������� USART.
 *
 * �����: ����� ������
 * ������: 1.0
 *
 * �����������:
 * 	USART1 - ����� ������� � ��������.
 */


#include <stdbool.h>
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"


	bool is_USART1_TurnedOn = 0;

	unsigned char USART1_ReciveBuf[128];											//������� ����� USART2
	const unsigned char USART1_OverFlowError[] = " Recive overflow error!\r\n";	//��������� � ������������ �������� ������

	u8 USART1_ReciveCount = 1;		//��������� ������� �����������
	u8 USART1_IRQn_Priority =0;		//��������� USART


/**
 * �-��� ������������� USART1, ������� �������� � ��������.
 */
void USART1_Init(void)
{

	USART1_ReciveBuf[0] = '1';

	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 	//����� - ����
	USART_InitStructure.USART_BaudRate = 9600;						//�������� = 9600
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//������� 1
	USART_InitStructure.USART_Parity = USART_Parity_No;				//��� �������� ��������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);

}

void USART1_BoostOn(u32 speed) {
	USART1_TurnOff();

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 	//����� - ����
	USART_InitStructure.USART_BaudRate = speed;						//�������� = 9600
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//������� 1
	USART_InitStructure.USART_Parity = USART_Parity_Even;			//��� �������� ��������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART1_TurnOn();
}


/**
 * �-��� ��������� USART1
 */
void USART1_TurnOn(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	//NVIC_EnableIRQ(USART1_IRQn);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
	is_USART1_TurnedOn = 1;
}


/**
 * �-��� ���������� USART2
 */
void USART1_TurnOff(void)
{
	USART_Cmd(USART1, DISABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	NVIC_DisableIRQ(USART1_IRQn);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
	is_USART1_TurnedOn = 0;
}


/**
 * �-��� �������� ������ �� USART1
 */
void USART1_SendString(const unsigned char *pucBuffer, u32 ulCount)
{
    while(ulCount--)
    {
        USART_SendData(USART1, (uint16_t) *pucBuffer++);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }
}


/**
 * �������-���������� �������� ���������� USART1
 */
void USART1_IRQHandler(void)
{
 if ((USART1->SR & USART_FLAG_RXNE) != (u16)RESET)
     {
	 USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	 if (USART1_ReciveCount<128)
	 {
		 USART1_ReciveBuf[USART1_ReciveCount] = (char) USART_ReceiveData(USART1);
		 USART1_ReciveCount++;
	 }
	 else
		 USART1_ReciveCount=0;
     }

// if ((USART1->SR & USART_FLAG_RXNE) != (u16)RESET)
//     {
//         i = USART_ReceiveData(USART1);
//         if(i == '1'){
//             GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_SET);        // ������������� '1' �� 8 ����
//             UARTSend("LED ON\r\n",sizeof("LED ON\r\n"));    // ������� ������� � UART
//         }
//         else if(i == '0'){
//             GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_RESET);      // ������������� '0' �� 8 ����
//             UARTSend("LED OFF\r\n",sizeof("LED OFF\r\n"));
//         }
//     }
}
