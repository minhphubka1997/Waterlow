#include "dwt_stm32_delay.h"
 
 /**
 * @brief This function provides a delay (in microseconds)
 * @param microseconds: delay in microseconds
 */
void DWT_Delay_us( uint32_t microseconds)
{
 uint32_t clk_cycle_start = DWT->CYCCNT;
 /* Go to number of cycles for system */
 microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);
 /* Delay till end */
 while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
}
 

/**
 * @brief Initializes DWT_Clock_Cycle_Count for DWT_Delay_us function
 * @return Error DWT counter
 * 1: clock cycle counter not started
 * 0: clock cycle counter works
 */
uint32_t DWT_Delay_Init(void) 
{
	// Disable TRC
	CoreDebug->DEMCR &= CoreDebug_DEMCR_TRCENA_Msk; //
	
	// Enable TRC
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;	//
	
	// Disable clock cycle counter
	DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
	
	// Enable clock cycle counter
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	
	// Reset the clock cycle counter value
	DWT->CYCCNT = 0;
	
	// 3 no operation
	__NOP();
	__NOP();
	__NOP();
	
	// Check if clock cycle counter has started
	if (DWT->CYCCNT)
	{
		return 0; // Clock cycle counter started
	}
	else
	{
		return 1;	// Clock cycle counter not started
	}
	
}
//
 
 