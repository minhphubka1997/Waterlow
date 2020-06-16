#include "main.h"
#include "stdarg.h"
#include "hl_printf.h"

extern bool SIMTransmitDone;
extern UART_HandleTypeDef huart2;



void sim_send_data(uint8_t data[], int lenght)
{
	if (SIMTransmitDone == true)
	{
		SIMTransmitDone = false;
		HAL_UART_Transmit_DMA(&huart2, data, lenght);
	}
}
//



/*
 *	lora_printf [Public function]
 *	this function print data via lora PORT
*/
void sim_printf(char *fmt, ...)
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
	 
	 sim_send_data(tempbuf.payload, tempbuf.length);
}
 //


