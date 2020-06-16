#ifndef _HL_PRINTF_
#define _HL_PRINTF_

#include "stdint.h"

typedef struct hlbufst
{
	uint8_t payload[1024];
	int length ;
} hlbuf;
//

char *hlp_convert(unsigned int num, int base);
void hlp_putchar(hlbuf *buf,char a);
void hlp_puts(hlbuf *buf, char * str);
void hlp_resetbuf(hlbuf *buf);



#endif


