/* 
 * File:   common.h
 * Author: moskw
 *
 * Created on 6 апреля 2018 г., 12:51
 */

#ifndef COMMON_H
#define	COMMON_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define _XTAL_FREQ 32000000
    
//|+----------------------------------------------------------------------+|
//|Some def functions
//|+----------------------------------------------------------------------+|
#define bit_set(var,bitno)  ((var) |=   1UL << (bitno))
#define bit_clr(var,bitno)  ((var) &= ~(1UL << (bitno)))
#define bit_get(data,bitno) ((data >> bitno) & 0x01)

// LED indicators
#define IND1    LATC6 //OUT
#define IND2    LATB5 //OUT    
#define IND3    LATC2 //OUT   
#define IND4    LATC5 //OUT    
#define IND5    LATC4 //OUT    
#define IND6    LATA4 //OUT

#define IND1_C  TRISCbits.TRISC6
#define IND2_C  TRISBbits.TRISB5
#define IND3_C  TRISCbits.TRISC2
#define IND4_C  TRISCbits.TRISC5
#define IND5_C  TRISCbits.TRISC4
#define IND6_C  TRISAbits.TRISA4

#define IND_C() { IND1_C = 0; IND2_C = 0; IND3_C = 0; IND4_C = 0; IND5_C = 0; IND6_C = 0; }    
#define IND_DIS() { IND1 = 0; IND2 = 0; IND3 = 0; IND4 = 0; IND5 = 0; IND6 = 0; }

    
// +24V detect input
#define VIN_ADC     RC7 //IN
#define VIN_ADC_C   TRISC7

#define DBG_SIG     LATB6
#define DBG_SIG_C   TRISB6
#define DBG_SIG_HIGH()      (DBG_SIG = 1)
#define DBG_SIG_LOW()       (DBG_SIG = 0)
#define DBG_SIG_TOGGLE()    (DBG_SIG=!DBG_SIG)
    
    
// one-wire pins
//#define TRISDATA    TRISB7
//#define CLOCK   RC3 //OUT
//#define DATA    RB7 //OUT

// host SMBus pins:
// not used
#define PR_SCL  PORTBbits.RB6 //IN
// used as +3V detect input
#define PR_SDA  PORTBbits.RB4 //IN
    
// ???
// #define BATLOW  RA5 //IN
// #define SLEEP   RA2 //IN

#define default_RAAC     8000   
#define charge_current_on 1000
#define charge_current_off 1500
    
typedef enum err_ { 
    ERR_OK,
    ERR_TRUE,
    ERR_NULLPTR,
    ERR_SWI2C_READ,        
    ERR_SWI2C_WRITE,
    ERR_SWI2C_NOACK,
} err_t;



#ifdef	__cplusplus
}
#endif

#endif	/* COMMON_H */

