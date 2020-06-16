#include "main.h"
#include "app.h"
#include "ring_buffer.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;



bool LoraTransmitDone = true;
bool SIMTransmitDone = true;

extern ring_buffer_typdef 	sim_data_buffer;
extern ring_buffer_typdef 	lora_data_buffer;

char LrData[3];
char SimData[3];


void LoraReceiveDMA(void)
{
	#if USING_LORA_RECEIVE_DMA > 0
	HAL_UART_Receive_DMA(&huart1, (uint8_t *)LrData, 2);
	#endif
}
//

void SIMReceiveDMA(void)
{
	#if USING_SIM_RECEIVE_DMA > 0
	HAL_UART_Receive_DMA(&huart2, (uint8_t *)SimData, 2);
	#endif
}
//


void LoraReceiveIT(void)
{
	#if USING_LORA_RECEIVE_IT > 0
	HAL_UART_Receive_IT(&huart1, LrData, 1);
	#endif
}
//

void SIMReceiveIT(void)
{
	#if USING_SIM_RECEIVE_IT > 0
	HAL_UART_Receive_IT(&huart2, SimData, 1);
	#endif
}
//


void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		ring_buffer_push(&lora_data_buffer, LrData[0]);
	}
	else
	if (huart ->Instance == USART2)
	{
		#if USING_SIM_RECEIVE_DMA > 0
		ring_buffer_push(&sim_data_buffer, SimData[0]);
		#endif
	}
}
//

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		ring_buffer_push(&lora_data_buffer, LrData[1]);
	}
	else
	if (huart->Instance == USART2)
	{
		#if USING_SIM_RECEIVE_DMA > 0
		ring_buffer_push(&sim_data_buffer, SimData[1]);
		#endif
		
		#if USING_SIM_RECEIVE_IT
		ring_buffer_push(&sim_data_buffer, SimData[0]);
		#endif
	}
}
//

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		LoraTransmitDone = true;
	}
	else
	if (huart->Instance == USART2)
	{
		SIMTransmitDone = true;
	}
}
//





