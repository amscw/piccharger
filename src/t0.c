#include <p18cxxx.h>
#include <timers.h>


/********************************************************************
*    Function Name:  CloseTimer0                                    *
*    Return Value:   void                                           *
*    Parameters:     void                                           *
*    Description:    This routine disables the Timer0 interrupt.    *
********************************************************************/
#if  defined (TMR_V1) || defined (TMR_V2) || defined (TMR_V3) ||\
     defined (TMR_V4) || defined (TMR_V5) || defined (TMR_V6) ||\
	 defined (TMR_V7) || defined (TMR_V7_1) || defined (TMR_V7_2)\
	  || defined (TMR_V7_3) || defined (TMR_V7_4) || defined (TMR_V7_5)

void CloseTimer0(void)
{
  T0CONbits.TMR0ON = 0;  // Disable Timer1
  INTCONbits.TMR0IE = 0;   // Disable Timer1 overflow interrupts
}

#endif

/********************************************************************
*    Function Name:  OpenTimer0                                     *
*    Return Value:   void                                           *
*    Parameters:     config: bit definitions to configure Timer0    *
*    Description:    This routine resets the Timer0 regs to the     *
*                    POR state and configures the interrupt, clock  *
*                    source, edge and prescaler.                    *
*    Notes:          The bit definitions for config can be found    *
*                    in the timers.h file.                          *
********************************************************************/
#if  defined (TMR_V1) || defined (TMR_V2) || defined (TMR_V3) ||\
     defined (TMR_V4) || defined (TMR_V5) || defined (TMR_V6) ||\
	 defined (TMR_V7) || defined (TMR_V7_1) || defined (TMR_V7_2)\
	 || defined (TMR_V7_3) || defined (TMR_V7_4) || defined (TMR_V7_5)
 
void OpenTimer0(unsigned char config)
{
  T0CON = (0x7f & config);  // Configure timer, but don't start it yet
  TMR0H = 0;                // Reset Timer0 to 0x0000
  TMR0L = 0;
  INTCONbits.TMR0IF = 0;      // Clear Timer0 overflow flag

  if(config&0x80)           // If interrupts enabled
    INTCONbits.TMR0IE = 1;    // Enable Timer0 overflow interrupt
  else
    INTCONbits.TMR0IE = 0;

  T0CONbits.TMR0ON = 1;
}

#endif

/********************************************************************
*    Function Name:  ReadTimer0                                     *
*    Return Value:   int: Timer0 16-bit value                       *
*    Parameters:     void                                           *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer0.                                        *
********************************************************************/
#if  defined (TMR_V1) || defined (TMR_V2) || defined (TMR_V3) ||\
     defined (TMR_V4) || defined (TMR_V5) || defined (TMR_V6) ||\
	 defined (TMR_V7) || defined (TMR_V7_1) || defined (TMR_V7_2)\
	  || defined (TMR_V7_3) || defined (TMR_V7_4) || defined (TMR_V7_5)

unsigned int ReadTimer0(void)
{
  union Timers timer;

  timer.bt[0] = TMR0L;  // Copy Timer0 low byte into union
  timer.bt[1] = TMR0H;  // Copy Timer0 high byte into union

  return (timer.lt);    // Return the int
}

#endif

/********************************************************************
*    Function Name:  WriteTimer0                                    *
*    Return Value:   void                                           *
*    Parameters:     int: value to write to Timer0                  *
*    Description:    This routine writes a 16-bit value to Timer0   *
*                    Timer0.                                        *
********************************************************************/
#if  defined (TMR_V1) || defined (TMR_V2) || defined (TMR_V3) || \
     defined (TMR_V4) || defined (TMR_V5) || defined (TMR_V6) ||\
	 defined (TMR_V7) || defined (TMR_V7_1) || defined (TMR_V7_2)\
	  || defined (TMR_V7_3) || defined (TMR_V7_4) || defined (TMR_V7_5)

void WriteTimer0(unsigned int timer0)
{
  union Timers timer;

  timer.lt = timer0;    // Copy timer value into union

  TMR0H = timer.bt[1];  // Write high byte to Timer0
  TMR0L = timer.bt[0];  // Write low byte to Timer0
}
#endif

