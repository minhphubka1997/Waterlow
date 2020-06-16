#include "main.h"
#include "dwt_stm32_delay.h"


#define DS18B20_PORT	GPIOB
#define DS18B20_PIN		GPIO_PIN_9

#define _delay_us DWT_Delay_us

void Set_Pin_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DS18B20_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
}
//

void Set_Pin_Input(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DS18B20_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
}
//

uint8_t DS18B20_Start (void)	
{
	uint8_t Response = 0;
	Set_Pin_Output();   // set the pin as output
	HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);  // pull the pin low
	_delay_us (480);   // _delay_usaccording to datasheet

	Set_Pin_Input();    // set the pin as input
	_delay_us(80);    // _delay_usaccording to datasheet

	if (!(HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN))) Response = 1;    // if the pin is low i.e the presence pulse is detected
	else Response = -1;

	_delay_us(400); // 480 us _delay_ustotally.

	return Response;
}
//



void DS18B20_Write (uint8_t data)
{
	Set_Pin_Output();  // set as output

	for (int i=0; i<8; i++)
	{

		if ((data & (1<<i))!=0)  // if the bit is high
		{
			// write 1

			Set_Pin_Output();  // set as output
			HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);  // pull the pin LOW
			_delay_us(1);  // wait for 1 us

			Set_Pin_Input();  // set as input
			_delay_us(50);  // wait for 60 us
		}

		else  // if the bit is low
		{
			// write 0

			Set_Pin_Output();
			HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);  // pull the pin LOW
			_delay_us(50);  // wait for 60 us

			Set_Pin_Input();
		}
	}
}
//



uint8_t DS18B20_Read (void)
{
	uint8_t value=0;
	Set_Pin_Input ();

	for (int i=0;i<8;i++)
	{
		Set_Pin_Output ();   // set as output

		HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);  // pull the data pin LOW
		_delay_us(2);  // wait for 2 us

		Set_Pin_Input ();  // set as input
		if (HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN))  // if the pin is HIGH
		{
			value |= 1<<i;  // read = 1
		}
		_delay_us(60);  // wait for 60 us
	}
	return value;
}
//

float DS18B20_ReadTemperature(void)
{
	uint8_t Temp_byte1 = 0, Temp_byte2 = 0;
	uint16_t Temp = 0;
		
	DS18B20_Start ();
	HAL_Delay (1);
	DS18B20_Write (0xCC);  // skip ROM
	DS18B20_Write (0x44);  // convert t
	_delay_us (800);

	DS18B20_Start ();
	HAL_Delay(1);
	DS18B20_Write (0xCC);  // skip ROM
	DS18B20_Write (0xBE);  // Read Scratch-pad

	Temp_byte1 = DS18B20_Read();
	Temp_byte2 = DS18B20_Read();
	Temp = Temp_byte2;
	Temp = Temp << 8;
	Temp |= Temp_byte1;
	
	return (float) (Temp/16.0);
}
//


