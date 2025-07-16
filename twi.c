
#include <avr/io.h>
#include "twi.h"

// (private) global state that determines if the bus is currently in a transmission
static uint8_t twi_in_transmission;
// (private) global state that determines if the bus is currently enabled (Just use TWEN bit?)
//static uint8_t twi_initialized;

// Initialize the SCL and SDA pins and setup i2c bus
int twi_init(void)
{
  // Check if bus has already been initialized
  if (TWCR & _BV(TWEN))
  {
    return TW_BUS_ERROR;
  }

  // Initialize SCL and SDA as input pins
  DDRC &= ~(_BV(PC5) | _BV(PC4)); 
  // And enable internal pull up resistors
  PORTC |= (_BV(PC5) | _BV(PC4));

  // Setup bit rate it bit rate register
  // first clear the prescaler bits (prescaler = 1)
  TWSR &= ~(_BV(TWPS0) | _BV(TWPS1));
  // Calculate bit rate register with clock frequency and bus rate
  TWBR = ((F_CPU/TWI_SCL_FREQ)-16)/2;
  
  // enable the bus
  TWCR = _BV(TWEN) | _BV(TWEA); 
  // setup global state
  twi_in_transmission=0;
  // Return no error
  return TWI_OK;
}

// Initiate master receiver mode (read)
int twi_read(uint8_t address, uint8_t *data_buffer, uint8_t data_len)
{
  /* START */
  // Check to see if already in transmission or if a start condition
  // needs to be signaled
  if (!twi_in_transmission)
  {
    TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWSTA) | _BV(TWEN);
  }
  // wait for start condition to be initiated
  loop_until_bit_is_set(TWCR, TWINT);
  if (TW_STATUS != TW_START) // TODO might also need to check for repeat start code
  {
    // Handle error and/or return it
    if (TW_STATUS == TW_BUS_ERROR)
      twi_stop();
    return TW_STATUS;
  }
  twi_in_transmission = 1;

  /* ADDRESS */
  // Create address plus read bit
  TWDR = (address << 1) | TW_READ;
  // Send address (clear interrupt flag, enable ack, enable bus)
  TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN);
  // Wait for address to be acknowledged and enter master receiver mode
  loop_until_bit_is_set(TWCR, TWINT);
  if (TW_STATUS != TW_MR_SLA_ACK)
  {
    twi_stop();
    return TW_STATUS;
  }

  /* DATA */
  // TODO invalidate the data_len=0 case from the start of function
  for (uint8_t i=0; i < data_len; ++i)
  {
    // Check to see if we need to return a NACK (next last received byte)
    if ((int)i >= ((int)data_len-1))
    {
      // Send NACK to get last byte of data
      TWCR = _BV(TWINT) | _BV(TWEN);
    }
    else
    {
      // Send ACK to keep getting data
      TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN);
    }
    // wait for data to become available
    loop_until_bit_is_set(TWCR, TWINT);
    // Check status
    if (TW_STATUS == TW_MR_DATA_ACK || TW_STATUS == TW_MR_DATA_NACK) // TODO make these two seperate cases? what happens when device doesn't keep returning data?
    {
      // Valid data, fill buffer
      data_buffer[i] = TWDR;
    }
    else
    {
      // handle error
      twi_stop();
      return TW_STATUS;
    }
  }
  
  /* STOP */
  twi_stop();
  return TWI_OK;
}

// Initial master transmitter mode (write)
int twi_write(uint8_t address, uint8_t *data_buffer, uint8_t data_len)
{
  /* START */
  // Check to see if already in transmission or if a start condition
  // needs to be signaled
  if (!twi_in_transmission)
  {
    // Signal start condition
    TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWSTA) | _BV(TWEN);    
  }
  // Wait for start condition to be initiated
  loop_until_bit_is_set(TWCR, TWINT);
  // check that the start condition status was successful
  if (TW_STATUS != TW_START)
  {
    // Handle error and/or return it
    if (TW_STATUS == TW_BUS_ERROR)
      twi_stop();
    return TW_STATUS;
  }
  twi_in_transmission = 1;

  /* ADDRESS */
  // Create address plus write bit
  TWDR = (address << 1) | TW_WRITE;
  // Transmit address (clear interrupt flag, enable ack, and enable bus)
  TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN);
  // Check that address was acknowledged by slave device
  loop_until_bit_is_set(TWCR, TWINT);
  if (TW_STATUS != TW_MT_SLA_ACK)
  {
    twi_stop();
    return TW_STATUS;
  }

  /* DATA */
  for (uint8_t i=0; i < data_len; ++i)
  {
    // Load and send data
    TWDR = data_buffer[i];
    TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN);
    // wait for bus to send data
    loop_until_bit_is_set(TWCR, TWINT);
    if (TW_STATUS != TW_MT_DATA_ACK)
    {
      // handle error
      // TODO if errors out here, maybe take lenght in by pointer so
      // it can be modified to notify the user where the transmission
      // failed...
      twi_stop();
      return TW_STATUS;
    }
  }

  /* STOP */
  twi_stop();
  return TWI_OK;
}

// Set a stop condition on the bus
// TODO make this function static and add to read/write API to indicate stop
int twi_stop(void)
{
  // Set stop condition 
  TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWSTO) | _BV(TWEN); 
  while (TWCR & _BV(TWSTO))
  {
    // stopping...
    continue;
  }
  twi_in_transmission = 0;
  return TWI_OK;
}

// Disable the I2C bus
void twi_close(void)
{
  // do I need to check if there is an ongoing transmission?
  // Should I write just TWINT to clear any operation on the bus?
  TWCR = 0;
}

