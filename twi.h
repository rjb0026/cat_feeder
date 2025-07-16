
#ifndef _ATMEGA328_TWI_H_
#define _ATMEGA328_TWI_H_

#include <stdint.h>
#include <util/twi.h>

// Defines for AtMega328P Specific layout
#ifndef TWI_SCL_FREQ
#define TWI_SCL_FREQ 100000L
#endif

// default clock speed for atmega328p (changed at top of main)
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// Defines for all the TWI Status codes
// #define TWI_START 0x08
// #define TWI_REPEAT_START 0x10
// #define TWI_MT_SLA_ACK 0x18
// #define TWI_MT_SLA_NACK 0x20
// #define TWI_MT_DATA_ACK 0x28
// #define TWI_MT_DATA_NACK 0x30
// #define TWI_ARB_LOST 0x38
// #define TWI_MR_SLA_ACK 0x40
// #define TWI_MR_SLA_NACK 0x48
// #define TWI_MR_DATA_ACK 0x50
// #define TWI_MR_DATA_NACK 0x58
// #define TWI_NO_STATE_INFO 0xF8
// #define TWI_ERROR 0x00
#define TWI_OK 0x01

/// @brief Initialize the two wire interface bus
int twi_init(void);

/// @brief Start a read transmission
/// @param address is the address of the device on the bus to read from
/// @param data_buffer is a buffer the read data will be copied out to
/// @param data_len is the length of data expected from the read
/// @returns an error code if the transmission fails
int twi_read(uint8_t address, uint8_t *data_buffer, uint8_t data_len);

/// @brief Start a write transmission
/// @param address is the address of the device on the bus to write to
/// @param data_buffer is the buffer of data to write to the device
/// @param data_len is the length of data to write
/// @returns an error code if the transmission fails
int twi_write(uint8_t address, uint8_t *data_buffer, uint8_t data_len);

/// @brief Stop a transmission
/// 
/// If a second or subsequent call to twi_read or twi_write
/// is made, a repeat start condition will be used. Otherwise, a stop
/// condition must be triggered.
int twi_stop(void);

/// @brief Close or disable the TWI (I2C bus)
void twi_close(void);

#endif
