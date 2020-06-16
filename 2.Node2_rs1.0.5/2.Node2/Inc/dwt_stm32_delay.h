#ifndef DWT_STM32_DELAY_H
#define DWT_STM32_DELAY_H

 
#include "main.h"
/**
 * @brief Initializes DWT_Cycle_Count for DWT_Delay_us function
 * @return Error DWT counter
 * 1: DWT counter Error
 * 0: DWT counter works
 */
uint32_t DWT_Delay_Init(void);
uint32_t DWT_GetTickUs(void);

void DWT_Delay_us( uint32_t microseconds);

 
#endif


