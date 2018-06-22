#include "1-wire.h"

// https://www.maximintegrated.com/en/app-notes/index.mvp/id/126

//-----------------------------------------------------------------------------
// macro
//-----------------------------------------------------------------------------
#define DQ_PIN_C    TRISBbits.TRISB4
#define DQ_PIN_WR   LATBbits.LATB4
#define DQ_PIN_RD   PORTBbits.RB4

// see datasheet, page 3
#define tSLOT   60  // 120
#define tREC    1
#define tRSTL   480 // 960
#define tPDH    15  // 60
#define tPDL    60  // 240


//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------

/**
 * Generate a 1-Wire reset, return 1 if no presence detect was found,
 * return 0 otherwise.
 * @return 
 */
inline int OWIsPresence(void)
{
        int result;

        // di();
        DQ_PIN_WR = 0; 
        DQ_PIN_C = 0;
        __delay_us(tRSTL);
        DQ_PIN_C = 1;       // Releases the bus
        __delay_us(tPDH);
        result = DQ_PIN_RD; // Sample for presence pulse from slave
        __delay_us(tPDL);   // Complete the reset sequence recovery
        // ei();
        
        return result;      // Return sample presence pulse result
}

inline void OWWrite_1(void)
{
    
}

inline void OWWrite_0(void)
{
    
}
