#include <p18cxxx.h>
#include <delays.h>
#include "i2c_data.h"
#include "sw_i2c.h"

/********************************************************************
*     Function Name:    signed char SWAckI2C(void)                  *
*     Return Value:     error condition status                      *
*     Parameters:       void                                        *
*     Description:      This function generates a bus acknowledge   *
*                       sequence.                                   *
********************************************************************/
signed char SWAckI2C( void )
{
  SCLK_LAT = 0;                   // set clock pin latch to 0  
  CLOCK_LOW;                      // set clock pin to output to drive low
  DATA_HI;                        // release data line to float high 
  Delay10TCY();                   // user may need to modify based on Fosc
  CLOCK_HI;                      // release clock line to float high
  Delay1TCY();                    // 1 cycle delay
  Delay1TCY();                    // 1 cycle delay

  if ( DATA_PIN )                 // error if ack = 1, slave did not ack
  {
    return ( -1 );                // return with acknowledge error
  }
  else
  {
    return ( 0 );                 // return with no error
  }
}

void SWIssueAckI2C(void)
{
      CLOCK_LOW;                  // make clock pin output to drive low
      DATA_LAT = 0;               // set data pin latch to 0
      DATA_LOW;                   // make data pin output to drive low
      Delay10TCY();               // user may need to modify based on Fosc
      CLOCK_HI;                   // release clock line to float high 
      Delay10TCY();               // user may need to modify based on Fosc    
}

/********************************************************************
*   Function Name:      signed char Clock_test(void)                * 
*   Return Value:       error condition status                      *
*   Parameters:         void                                        *
*   Description:        This function allows for a slave I2C device *
*                       to stretch the clock low. The time period to*
*                       wait may need to be adjusted.               *
********************************************************************/
signed char Clock_test( void )
{
  Delay10TCYx(3);                 // user may need to adjust timeout period

  if ( !SCLK_PIN )                // if clock is still low after wait 
  {
    return ( -2 );                // return with clock error
  }
  else
  {
    return ( 0 );                 // return with no error
  } 
}

/* far */ unsigned char I2C_BUFFER;    // temp buffer for R/W operations
/* far */ unsigned char BIT_COUNTER;   // temp bufffer for bit counting

/********************************************************************
*     Function Name:    unsigned int SWReadI2C(void)                *
*     Return Value:     data byte or error condition                *
*     Parameters:       void                                        *
*     Description:      Read single byte from I2C bus.              *
********************************************************************/
signed int SWReadI2C( void )
{
  BIT_COUNTER = 8;                // set bit count for byte 
  SCLK_LAT = 0;                   // set clock pin latch to 0

  do
  {
    CLOCK_LOW;                    // set clock pin output to drive low
    DATA_HI;                      // release data line to float high
    Delay10TCY();                 // user may need to modify based on Fosc
    CLOCK_HI;                     // release clock line to float high
    Delay1TCY();                  // user may need to modify based on Fosc
    Delay1TCY();

    if ( !SCLK_PIN )              // test for clock low
    {
      if ( Clock_test( ) )        // clock wait routine
      {
        return ( -2 );            // return with error condition       
      }
    }

    I2C_BUFFER <<= 1;             // shift composed byte by 1
    I2C_BUFFER &= 0xFE;           // clear bit 0

    if ( DATA_PIN )               // is data line high
     I2C_BUFFER |= 0x01;          // set bit 0 to logic 1
   
  } while ( --BIT_COUNTER );      // stay until 8 bits have been acquired

  return ( (unsigned int) I2C_BUFFER ); // return with data
}


/********************************************************************
*     Function Name:    signed char SWGetsI2C(unsigned char *rdptr, *
*                                             unsigned char length) *
*     Return Value:     return with error condition                 *
*     Parameters:       address of read string storage location and *
*                       length of string bytes to read              *
*     Description:      This routine reads a string from the I2C    *
*                       bus.                                        *
********************************************************************/
signed char SWGetsI2C( unsigned char *rdptr,  unsigned char length )
{
  unsigned int thold;             // define auto variable
  for (;length > 0; length--)              // stay in loop until byte count is zero
  {
    // thold = (unsigned int)SWGetcI2C();          // read and save 1 byte
    thold = (unsigned int)SWReadI2C();
    if ( thold & 0xFF00 )
    {
      return ( -1 );              // return with error condition
    }
    else
    {
      *rdptr++ = thold;           // save off byte read
    }

    if ( !length )                // initiate NOT ACK
    {
      CLOCK_LOW;                  // make clock pin output to drive low
      DATA_HI;                    // release data line to float high 
      Delay10TCY();               // user may need to modify based on Fosc
      CLOCK_HI;                   // release clock line to float high 
      Delay10TCY();               // user may need to modify based on Fosc
    }
    else                          // else initiate ACK condition
    {
      CLOCK_LOW;                  // make clock pin output to drive low
      DATA_LAT = 0;               // set data pin latch to 0
      DATA_LOW;                   // make data pin output to drive low
      Delay10TCY();               // user may need to modify based on Fosc
      CLOCK_HI;                   // release clock line to float high 
      Delay10TCY();               // user may need to modify based on Fosc
    }
  }   
  return( 0 );                    // return with no error
}

/**********************************************************************
*     Function Name:    signed char SWWriteI2C(unsigned char data_out)*
*     Return Value:     error condition if bus error occurred         *
*     Parameters:       Single data byte for I2C bus.                 *
*     Description:      This routine writes a single byte to the      *
*                       I2C bus.                                      *
**********************************************************************/
signed char SWWriteI2C( unsigned char data_out )
{
  BIT_COUNTER = 8;                // initialize bit counter
  I2C_BUFFER = data_out;          // data to send out
  SCLK_LAT = 0;                   // set latch to 0
                                 
  do
  {
    if ( !SCLK_PIN )              // test if clock is low
    {                             // if it is then ..
      if ( Clock_test( ) )        // wait on clock for a short time
      {
        return ( -1 );            // return with error condition      
      }
    }

    else 
    {
#ifndef _OMNI_CODE_
     I2C_BUFFER &= 0xFF;          // generate movlb instruction
      _asm
      rlcf I2C_BUFFER,1,1         // rotate into carry and test  
      _endasm
#else
      if (STATUSbits.C == 1)
              I2C_BUFFER = (unsigned char)((I2C_BUFFER << 1) | 0x1);
      else
              I2C_BUFFER = (unsigned char)(I2C_BUFFER << 1);
#endif

      if ( STATUS & 0x01 )        // if carry set, transmit out logic 1
      {
       CLOCK_LOW;                 // set clock pin output to drive low
       DATA_HI;                   // release data line to float high 
       Delay10TCY();              // user may need to modify based on Fosc
       CLOCK_HI;                  // release clock line to float high 
       Delay10TCY();              // user may need to modify based on Fosc
      }
      else                        // transmit out logic 0
      {
        CLOCK_LOW;                // set clock pin output to drive low
        DATA_LAT = 0;             // set data pin latch to 0
        DATA_LOW;                 // set data pin output to drive low 
        Delay10TCY();             // user may need to modify based on Fosc
        CLOCK_HI;                 // release clock line to float high 
        Delay10TCY();             // user may need to modify based on Fosc
      }

     BIT_COUNTER --;              // reduce bit counter by 1
    }
  } while ( BIT_COUNTER );        // stay in transmit loop until byte sent 

  return ( 0 );                   // return with no error
}

/********************************************************************
*     Function Name:    signed char SWPutsI2C(unsigned char *wrptr) *
*     Return Value:     end of string indicator or bus error        *
*     Parameters:       address of write string storage location    *
*     Description:      This routine writes a string to the I2C bus.*
********************************************************************/
signed char SWPutsI2C( unsigned char *wrptr )
{
  while ( *wrptr )
  {               
    if ( SWPutcI2C( *wrptr++) )   // write out data string to I2C receiver
    {
      return( -1 );               // return if there was an error in Putc()
    }
    else
    {
      if ( SWAckI2C( ) )          // go read bus ack status
      {
        return( -1 );             // return with possible error condition 
      }
    }
  }                              
  return ( 0 );                   // return with no error
}

/********************************************************************
*     Function Name:    void SWRestartI2C(void)                     *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Send I2C bus restart condition.             *
********************************************************************/
void SWRestartI2C( void )
{
  SCLK_LAT = 0;                   // set clock pin latch to 0
  CLOCK_LOW;                      // set clock pin to output to drive low
  DATA_HI;                        // release data pin to float high
  Delay10TCY();                   // user may need to modify based on Fosc
  CLOCK_HI;                       // release clock pin to float high
  Delay10TCY();                   // user may need to modify based on Fosc
  DATA_LAT = 0;                   // set data pin latch to 0
  DATA_LOW;                       // set data pin output to drive low
  Delay10TCY();                   // user may need to modify based on Fosc
}

/********************************************************************
*     Function Name:    void SWStopI2C(void)                        *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Send I2C bus stop condition.                *
********************************************************************/
void SWStopI2C( void )
{
  SCLK_LAT = 0;                   // set clock pin latch to 0
  CLOCK_LOW;                      // set clock pin to output to drive low
  DATA_LAT = 0;                   // set data pin latch to 0
  DATA_LOW;                       // set data pin output to drive low
  Delay10TCY();                   // user may need to modify based on Fosc
  CLOCK_HI;                       // release clock pin to float high
  Delay10TCY();                   // user may need to modify based on Fosc
  DATA_HI;                        // release data pin to float high
  Delay1TCY();                    // user may need to modify based on Fosc
  Delay1TCY();
}

/********************************************************************
*     Function Name:    void SWStartI2C(void)                       *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Send I2C bus start condition.               *
********************************************************************/
void SWStartI2C( void )
{
  DATA_LAT = 0;                   // set data pin latch to 0
  DATA_LOW;                       // set pin to output to drive low
  Delay10TCY();                   // user may need to modify based on Fosc
}

