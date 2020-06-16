/*ring_buffer.h*/
#ifndef __ULTILS_H
#define __ULTILS_H

#include "Arduino.h"

#define BUFFER_SIZE 1024


class ringbuffer 
{
	public:
		unsigned int count;
		void push(unsigned char data);
		unsigned char pop (void);
		void clear();
		
	private:
		unsigned char buffer[BUFFER_SIZE];
		unsigned int head;
		unsigned int tail;
};


#endif
