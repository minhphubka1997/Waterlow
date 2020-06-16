#include "hl_printf.h"
#include "string.h"


char *hlbuf::convert(unsigned int num, int base) 
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

void hlbuf::addchar(char a)
{
	payload[length ++] = a;
}
//

void hlbuf::addstring( char  str[])
{
	int len = strlen(str);
	for (int i = 0; i< len; i++) payload[length++] = str[i];
}
//

void hlbuf:: clear(void)
{
	length = 0;
	for (int i = 0; i< HLBUF_LEN; i++) payload[i] = 0;
}
//

//
