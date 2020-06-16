#include "main.h"
#include "lora.h"
#include "stdarg.h"
#include "hl_printf.h"
#include "string.h"
#include "ring_buffer.h"
#include "STM_MY_LCD16X2.h"

extern UART_HandleTypeDef huart1;
extern bool LoraTransmitDone;					// extern from file my_uart.c
extern bool Node1PumpNeedOn;

void lora_send_data(uint8_t data[], int lenght)
{
	if (LoraTransmitDone == true)
	{
		LoraTransmitDone = false;
		HAL_UART_Transmit_DMA(&huart1, data, lenght);
	}
	//HAL_UART_Transmit(&huart1, data, lenght, lenght*2 );
}
//




/*
 *	lora_printf [Public function]
 *	this function print data via lora PORT
*/
void lora_printf(char *fmt, ...)
{
	hlbuf tempbuf;
	tempbuf.length = 0;
	
	
   va_list ap; /* points to each unnamed arg in turn */
   char *p, *sval;
   int ival;
//   double dval;
   va_start(ap, fmt); /* make ap point to 1st unnamed arg */
   for (p = fmt; *p; p++) 
   {
      if (*p != '%') 
      {
         hlp_putchar(&tempbuf,*p);
         continue;
      }
      switch (*++p) 
      {
				case 'c' :
						hlp_putchar(&tempbuf,*(p+1));
						break; 
				case 'd' : ival = va_arg(ap,int); 		//Fetch Decimal/Integer argument
						if(ival<0) 
						{ 
							ival = -ival;
							hlp_putchar(&tempbuf,'-'); 
						} 
						hlp_puts(&tempbuf,hlp_convert(ival,10));
						break; 
        case 'f':
//            dval = va_arg(ap, double);
						hlp_puts(&tempbuf," [not support printf double] ");
            //printf("%f", dval);
            break;
        case 's':
						sval = va_arg(ap, char*);
            hlp_puts(&tempbuf,sval);
            break;
				case 'x': ival = va_arg(ap,unsigned int); //Fetch Hexadecimal representation
						hlp_puts(&tempbuf,hlp_convert(ival,16));
						break; 
				
        default:
            hlp_putchar(&tempbuf,*p);
            break;
      }
   }
   va_end(ap); /* clean up when done */
	 
	 lora_send_data(tempbuf.payload, tempbuf.length);
}
 //




#define LINE_LEN		128
typedef struct
{
	char buffer[LINE_LEN];
	uint16_t	length;
} BUFFER_typdef;
//

#define START_CODE	'#'			// Define as a string
#define END_CODE	'!'			// Define as a string

#define ALIVE_STR		"GWTH"
#define ALIVE_TO			30000				// Lora keep alive timeout = 30s 


BUFFER_typdef      	lora_data_line;
ring_buffer_typdef 	lora_data_buffer;
bool LoraConnected;


void lora_connected_callback(void);
void lora_loss_connect_callback(void);
void lora_decode(char str[]);



void lora_receive_process(void)
{
char c;
  
	while ((c=ring_buffer_pop(&lora_data_buffer)) >0)
	{
		if (c != 0)
		{
			if (c == START_CODE)
      {
        lora_data_line.length = 0;
        lora_data_line.buffer[lora_data_line.length++] = c;
      }
      else
      if (c == END_CODE)
      {
				lora_data_line.buffer[lora_data_line.length++] = 0;
        lora_decode(lora_data_line.buffer);
        lora_data_line.length = 0;
      }
      else
      {
        lora_data_line.buffer[lora_data_line.length++] = c;
        if (lora_data_line.length >= LINE_LEN) lora_data_line.length = 0;
      }
		}
	}
}
//

extern uint16_t ID;
uint32_t LastTimeGetAliveMessage = 0;
void lora_decode(char str[])
{
	char *p;
	
	if ((p = strstr(str, ALIVE_STR)) != NULL)
	{
		uint8_t NoNodeTh = 0;
		char astr[10];
		
		p = p + strlen(ALIVE_STR);
		
		astr[0] = *(p);
		astr[1] = *(p + 1);
		astr[2] = 0;
		NoNodeTh = atoi(astr);
		
		if (NoNodeTh > 0)
		{
			p = p + 2;
			for (int i = 0; i< NoNodeTh; i++)
			{
				// Get ID
				int id = 0;
				astr[0] = *p;
				astr[1] = *(p+1);
				astr[2] = *(p+2);
				astr[3] = 0;
				id = atoi(astr);
				if (ID == id)
				{
					if (LoraConnected == false)
					{
						lora_connected_callback();
						LoraConnected = true;
					}
					
					// Get Pump CMD
					p = p + 3;
					if ((*p) == '1') Node1PumpNeedOn = true;
					else Node1PumpNeedOn = false;
					
					LastTimeGetAliveMessage = HAL_GetTick();
					p = p+ 1;
				}
				else 
				{
					p = p + 4;
					continue;
				}
			}
		}

		
//		if (* (p + strlen(ALIVE_STR) ) == '1')
//		{
//			Node1PumpNeedOn = true;
//		}
//		else
//			Node1PumpNeedOn = false;
//		
//		if (LoraConnected == false)
//		{
//			lora_connected_callback();
//			LoraConnected = true;
//		}
//		
	}
	
	if (HAL_GetTick() - LastTimeGetAliveMessage >= ALIVE_TO)
	{
		if (LoraConnected )
		{
			LoraConnected = false;
			lora_loss_connect_callback();
		}
	}
}
//

void lora_cmd_turnon_pum1_callback()
{
	
}
//

extern bool isNotifyLoraNoConnect ;

void lora_loss_connect_callback(void)
{
	isNotifyLoraNoConnect  = false;	// 
	//LCD1602_1stLine();
//	LCD1602_setCursor(1,16);
//	LCD1602_print("0");
}
//

void lora_connected_callback(void)
{
	//LCD1602_1stLine();
//	LCD1602_setCursor(1,16);
//	LCD1602_print("1");
}
//


bool lora_is_connected(void) { return LoraConnected;}
