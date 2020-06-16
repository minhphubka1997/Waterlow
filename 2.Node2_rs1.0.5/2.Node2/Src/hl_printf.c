#include "hl_printf.h"
#include "string.h"


char *hlp_convert(unsigned int num, int base) 
{ 
	static char Representation[]= "0123456789ABCDEF";
	static char buffer[50]; 
	char *ptr; 
	
	ptr = &buffer[49]; 
	*ptr = '\0'; 
	
	do 
	{ 
		*--ptr = Representation[num%base]; 
		num /= base; 
	}while(num != 0); 
	
	return(ptr); 
}
//

void hlp_putchar(hlbuf *buf,char a)
{
	buf->payload[buf->length ++] = a;
}
//

void hlp_puts(hlbuf *buf, char * str)
{
	int len = strlen(str);
	for (int i = 0; i< len; i++) buf->payload[buf->length++] = str[i];
}
//

void hlp_resetbuf(hlbuf *buf)
{
	memset(buf, 0, sizeof(hlbuf));
}
//

//






