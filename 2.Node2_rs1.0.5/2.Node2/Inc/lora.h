#ifndef _LORA_H_
#define _LORA_H_

#include "stdint.h"
#include "stdbool.h"

#define LORA_M0_PORT		GPIOA
#define LORA_M0_PIN			GPIO_PIN_6
#define LORA_M1_PORT		GPIOA
#define LORA_M1_PIN			GPIO_PIN_7

#define Lora_Mode0() { HAL_GPIO_WritePin(LORA_M0_PORT, LORA_M0_PIN, GPIO_PIN_RESET); HAL_GPIO_WritePin(LORA_M1_PORT, LORA_M1_PIN, GPIO_PIN_RESET); }
#define Lora_Mode3() { HAL_GPIO_WritePin(LORA_M0_PORT, LORA_M0_PIN, GPIO_PIN_SET); HAL_GPIO_WritePin(LORA_M1_PORT, LORA_M1_PIN, GPIO_PIN_SET); }

void lora_send_data(uint8_t data[], int lenght);
void lora_printf(char *fmt, ...);
bool lora_is_connected(void);
void lora_receive_process(void);

#endif

