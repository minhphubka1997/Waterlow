/* ring_buffer.c*/

#include "ring_buffer.h"
#include <stdio.h>


void ringbuffer:: push(unsigned char data)
{
  unsigned int next = (unsigned int)(head + 1) % BUFFER_SIZE;
  if (next != tail)
  {
	buffer[head] = data;
	head = next;
	count += 1;
  }
}
		
		
unsigned char ringbuffer:: pop (void)
{
	if (head == tail) 
	{
		count = 0;
		return 0;        // quit with an error
	}
	else 
	{
		char data = buffer[tail];
		tail = (unsigned int)(tail + 1) % BUFFER_SIZE;
		count -= 1;
		return data;
	}
}

void ringbuffer:: clear()
{
  count = head = tail = 0;
}
