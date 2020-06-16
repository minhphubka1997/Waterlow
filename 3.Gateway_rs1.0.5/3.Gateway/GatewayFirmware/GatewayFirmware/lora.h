#ifndef __LORA_H
#define __LORA_H

#include "stdint.h"
#include "stdio.h"

#define LINE_LEN	30




typedef struct
{
	char buffer[LINE_LEN];
	uint16_t	length;
} BUFFER_typdef;
//


/*------------------------- COMMAND STRUCT---------------------*/
#define START_CODE	'#'			// Define as a string
#define END_CODE	'!'			// Define as a string




/*---------- PUBLIC function declaration --------------*/
void lora_process(void);
bool lora_is_connected(void);

#endif
//
