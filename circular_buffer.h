/*
 * Adapted from Embedded Artistry:
 * https://embeddedartistry.com/blog/2017/05/17/creating-a-circular-buffer-in-c-and-c/
 *
 * and from "Making Embedded Systems" by Elecia White
 *
 * Robert Brothers
 * 4-8-2020
 */

#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include <stdint.h>

/// @brief Circular buffer type
struct circular_buffer_t
{
  uint8_t* buffer;
  uint8_t head;
  uint8_t tail;
  // USE A POWER OF 2
  uint16_t max_size;
};

/// @brief Circular buffer handler type
typedef struct circular_buffer_t* cbuffer_handle_t;

/// @brief Create a circular buffer
/// @param byte_buffer is the buffer to encapsulate
/// @param size is the buffers maximum size in bytes - MUST BE POWER OF 2
/// @returns circular_buffer_t - a circular buffer
struct circular_buffer_t circular_buffer_init(uint8_t* byte_buffer, uint8_t size);

// @brief Free a circular buffer structure
//
// Does not free dynamically allocated memory (user is responsible for that)
// @param cbuffer is the circular buffer handle to free
// void circular_buffer_free(cbuffer_handle_t cbuffer);

/// @brief Reset a circular buffer structure
/// @param cbuffer is the circular buffer handle to reset (HEAD == TAIL)
void circular_buffer_reset(cbuffer_handle_t cbuffer);

/// @brief Write or add data to the circular buffer
/// @param data is the new data to put at the HEAD of the buffer
void circular_buffer_write(cbuffer_handle_t cbuffer, uint8_t data);

/// @brief Retrieve data from the circular buffer
/// @param cbuffer is the circular buffer handle
/// @param data_ptr is a pointer to the output data
/// @returns true_false 0 if data is successfully returned, -1 if buffer is empty
int circular_buffer_read(cbuffer_handle_t cbuffer, uint8_t* data);

/// @brief Determine if the buffer is empty
/// @param cbuffer is the circular buffer handle to check
/// @returns true_false 0 if buffer is empty, -1 if buffer has data
int circular_buffer_empty(cbuffer_handle_t cbuffer);

/// @brief Determine if the buffer is full
/// @param cbuffer is the circular buffer handle to check
/// @returns true_false 0 if buffer is full, -1 if buffer is not full
int circular_buffer_full(cbuffer_handle_t cbuffer);

/// @brief Determine the total capacity of the buffer
/// @param cbuffer is the circular buffer handle to check
/// @returns the total capacity of the buffer
uint16_t circular_buffer_capacity(cbuffer_handle_t cbuffer);

/// @brief Determine the size of (number of elements in) the buffer
/// @param cbuffer is the circular buffer handle to check
/// @returns the size of the buffer
uint8_t circular_buffer_size(cbuffer_handle_t cbuffer);

#endif
