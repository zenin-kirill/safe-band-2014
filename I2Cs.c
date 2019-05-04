/**
 * Файл, описывающий работу интерфейсов I2C.
 *
 * Автор: Зенин Кирилл
 * Версия: 1.0
 *
 * Реализованы:
 * 	I2C1 - работа с дисплеем.
 */


#include <stdbool.h>
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"

#include "Algorithmes.h"


/**
 * Ф-ция инициализации I2C1, скорость работы 100КГц
 */

 bool is_I2C1_TurnedOn = 0;

void I2C1_Init(void)
{
	I2C_InitTypeDef I2C;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	I2C_StructInit(&I2C);
	I2C.I2C_Mode = I2C_Mode_I2C;
	I2C.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C.I2C_ClockSpeed = 1000000;
	I2C.I2C_OwnAddress1 = 0x0;
	I2C.I2C_Ack = I2C_Ack_Enable;
	I2C.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1, &I2C);


	//I2C.I2C_Mode = I2C_Mode_I2C;
	//I2C.I2C_DutyCycle = I2C_DutyCycle_16_9;
	//I2C.I2C_OwnAddress1 = 0;
	//I2C.I2C_Ack = I2C_Ack_Enable;
	//I2C.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	//I2C.I2C_ClockSpeed = 100000;


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);
}


/**
 * Ф-ция включения I2C1
 */
void I2C1_TurnOn(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	//I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_Cmd(I2C1, ENABLE);

	is_I2C1_TurnedOn = 1;
}


/**
 * Ф-ция выключения I2C1
 */
void I2C1_TurnOff(void)
{
	I2C_Cmd(I2C1, DISABLE);
	//I2C_AcknowledgeConfig(I2C1, DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);

	is_I2C1_TurnedOn = 0;
}


/**
 * Ф-ция запуска передачи данных.
 * @return 1, если запуск прошел успешно; 0, если запуск не удался.
 */
void I2C1_Start(u8 HW_address)
{
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));					// Ждем освобождения шины
	I2C_GenerateSTART(I2C1, ENABLE);								// Генерируем старт
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));		// Ожидаем ответа
	I2C_Send7bitAddress(I2C1, HW_address, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
}


/**
 * Ф-ция передачи байта данных.
 * @param data Байт, который требуется передать.
 */
void I2C1_Write(unsigned char *data, unsigned char count)
{
	u32 i;
	for(i = 0; i < count; i++)
	{
		I2C_SendData(I2C1, data[i]);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
}


/**
 * Ф-ция окончания передачи данных.
 * @return 1, если окончание прошло успешно; 0, если окончить не удалось.
 */
void I2C1_Stop(void)
{
	I2C_GenerateSTOP(I2C1, ENABLE);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
}
