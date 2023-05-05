///////////////////////////////////////////////////////////////////////////////////////////////////
///  @file		Buffer.h
///  
///  @brief		Buffer circulaire sans écrasement
///  
///  @date		05/05/2023
///  @author	Cyprien Heusse
///  @copyright	N/A
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _BUFFER_H
#define _BUFFER_H

#include <Os/Common/Utils/GenericTypedefs.h>

// User definable size, always the same tho
#define BUFFER_SIZE 6

typedef struct {
	uint8 buffer[BUFFER_SIZE];
	uint8 first;
	uint8 last;
	char  full;  // bool
  char  empty;
} circular_buffer_t;

void init_buffer(circular_buffer_t* c_buffer);

void push_element(circular_buffer_t* c_buffer, uint8 element);
uint8 pop_element(circular_buffer_t* c_buffer);

char is_buffer_full(circular_buffer_t* buffer);
char is_buffer_empty(circular_buffer_t* buffer);
 
#endif

