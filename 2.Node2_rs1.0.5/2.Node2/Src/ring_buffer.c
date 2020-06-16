/* ring_buffer.c*/

#include "ring_buffer.h"
#include <stdio.h>



void ring_buffer_push(ring_buffer_typdef * buff, unsigned char data)
{
  unsigned int next = (unsigned int)(buff->head + 1) % BUFFER_SIZE;
  if (next != buff->tail)
  {
      buff->buffer[buff->head] = data;
      buff->head = next;
			buff->count += 1;
  }
}
//


char ring_buffer_pop(ring_buffer_typdef * buff)
{
  // if the head isn't ahead of the tail, we don't have any characters
	if (buff->head == buff->tail) 
	{
		buff->count = 0;
		return 0;        // quit with an error
	}
	else 
	{
		char data = buff->buffer[buff->tail];
		buff->tail = (unsigned int)(buff->tail + 1) % BUFFER_SIZE;
		buff->count -= 1;
		return data;
	}
}
//


void ring_buffer_clear(ring_buffer_typdef * buff)
{
	buff->count = buff->head = buff->tail = 0;
}
//


