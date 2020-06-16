#ifndef _HL_PRINTF_
#define _HL_PRINTF_

#include "stdint.h"

#define HLBUF_LEN		256

class hlbuf
{
	public :
		uint16_t length;
    char payload[HLBUF_LEN];
		void addchar(char ch);
		void addstring(char str[]);
		void clear(void);
		char* convert(unsigned int num, int base);

		
 private:
    
} ;
//

// char *hlp_convert(unsigned int num, int base);
// void hlp_putchar(hlbuf *buf,char a);
// void hlp_puts(hlbuf *buf, char * str);
// void hlp_resetbuf(hlbuf *buf);



#endif
