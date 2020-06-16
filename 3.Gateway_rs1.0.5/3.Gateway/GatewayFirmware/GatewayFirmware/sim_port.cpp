
#include "stdarg.h"
#include "hl_printf.h"
#include "app.h"


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
         tempbuf.addchar(*p);
         continue;
      }
      switch (*++p) 
      {
				case 'c' :
						//hlp_putchar(&tempbuf,*(p+1));
            tempbuf.addchar(*(p+1));
						break; 
				case 'd' : ival = va_arg(ap,int); 		//Fetch Decimal/Integer argument
						if(ival<0) 
						{ 
							ival = -ival;
							//hlp_putchar(&tempbuf,'-'); 
              tempbuf.addchar('-');
						} 
						//hlp_puts(&tempbuf,hlp_convert(ival,10));
            tempbuf.addstring(tempbuf.convert(ival,10));
						break; 
        case 'f':
//            dval = va_arg(ap, double);
						//hlp_puts(&tempbuf," [not support printf double] ");
            tempbuf.addstring(" [not support printf double] ");
            //printf("%f", dval);
            break;
        case 's':
						sval = va_arg(ap, char*);
            //hlp_puts(&tempbuf,sval);
            tempbuf.addstring(sval);
            break;
				case 'x': ival = va_arg(ap,unsigned int); //Fetch Hexadecimal representation
						//hlp_puts(&tempbuf,hlp_convert(ival,16));
            tempbuf.addstring(tempbuf.convert(ival,16));
						break; 
				
        default:
            //hlp_putchar(&tempbuf,*p);
            tempbuf.addchar(*p);
            break;
      }
   }
   va_end(ap); /* clean up when done */
	 
	 sim_send_data((char*)tempbuf.payload);
}
 //
