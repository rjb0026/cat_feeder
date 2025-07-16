
#include "circular_buffer.h"

// Return a circular buffer
struct circular_buffer_t circular_buffer_init(uint8_t* byte_buffer, uint8_t size)
{
  // TODO
  // assert(byte_buffer && size);
  
  struct circular_buffer_t new_buffer;
  new_buffer.buffer = byte_buffer;
  new_buffer.max_size = size;
  new_buffer.head = 0;
  new_buffer.tail = 0;

  return new_buffer;
}

void circular_buffer_reset(cbuffer_handle_t cbuffer)
{
  // TODO
  // assert(cbuffer);

  cbuffer->head = 0;
  cbuffer->tail = 0;
}

void circular_buffer_write(cbuffer_handle_t cbuffer, uint8_t data)
{
  // TODO
  // assert(cbuffer);
  
  // If buffer is currently full,
  // Write data and advance both the tail and head pointers
  // otherwise, just advance the head pointer
  cbuffer->buffer[cbuffer->head] = data;
  // Manually inlining full check to avoid changing header file to add inline function
  if (circular_buffer_size(cbuffer) == (cbuffer->max_size-1))
  {
    cbuffer->tail = (cbuffer->tail+1) & (cbuffer->max_size-1);
  }
  cbuffer->head = (cbuffer->head+1) & (cbuffer->max_size-1);
}

int circular_buffer_read(cbuffer_handle_t cbuffer, uint8_t* data)
{
  // TODO
  // assert(cbuffer);
  // Check to make sure there is data in the buffer before reading
  if (circular_buffer_size(cbuffer) == 0)
  {
    return -1;
  }
  *data = cbuffer->buffer[cbuffer->tail];
  cbuffer->tail = (cbuffer->tail+1) & (cbuffer->max_size-1);
  return 0;
}

int circular_buffer_empty(cbuffer_handle_t cbuffer)
{
  // TODO
  // assert(cbuffer);
  if (circular_buffer_size(cbuffer) == 0)
  {
    return 0;
  }
  else
  {
    return -1;
  }
}

int circular_buffer_full(cbuffer_handle_t cbuffer)
{
  // TODO
  // assert(cbuffer);
  if (circular_buffer_size(cbuffer) == (cbuffer->max_size-1))
  {
    return 0;
  }
  else
  {
    return -1;
  }
}

uint16_t circular_buffer_capacity(cbuffer_handle_t cbuffer)
{
  // TODO
  // assert(cbuffer);

  return cbuffer->max_size;
}

uint8_t circular_buffer_size(cbuffer_handle_t cbuffer)
{
  // TODO
  // assert(cbuffer);

 return ((cbuffer->head-cbuffer->tail) & (cbuffer->max_size-1)); 
}
