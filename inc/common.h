/*
 * common.h
 *
 *  Created on: 21 июн. 2018 г.
 *      Author: moskvin
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdint.h>
#if defined(__MINGW32__)
	#include "windows.h"
#elif defined(__XC8__)
    #include <xc.h>
    #include <p18cxxx.h>
#endif

//-----------------------------------------------------------------------------
// types
//-----------------------------------------------------------------------------
typedef enum err_ {
    ERR_OK,
    ERR_TRUE,
    ERR_NULLPTR,
	ERR_CONFIGURE,
    ERR_SWI2C_READ,
    ERR_SWI2C_WRITE,
    ERR_SWI2C_NOACK,
	ERR_CORE_NO_FREE_TIMERS,
	ERR_CORE_NO_MEM,
} err_t;

//-----------------------------------------------------------------------------
// macro
//-----------------------------------------------------------------------------
// #define __DBG_MSG__
#if defined(__MINGW32__)
	#define TRACE(fmt,args...) {\
		printf("%s(%s)-"fmt"\n", __FILE__, __FUNCTION__, ##args);\
		fflush(stdout);\
	}
#else
	// #define TRACE(fmt,args...) printf("%s(%s)-"fmt"\n", __FILE__, __FUNCTION__, ##args)
#endif

#if defined(__XC8__)
    // generic definitions
    #define	OUTPUT	0
    #define	INPUT 	1
    #define _XTAL_FREQ 32000000

    // pins definitions
    #define DBG_PIN     LATB6
    #define DBG_PIN_C   TRISB6
    #define DBG_PIN_HIGH()      (DBG_PIN = 1)
    #define DBG_PIN_LOW()       (DBG_PIN = 0)
    #define DBG_PIN_TOGGLE()    (DBG_PIN=!DBG_PIN)
    
    #define VIN_ADC     PORTBbits.RB7
    #define VIN_ADC_C   TRISC7
#endif

#endif /* COMMON_H_ */
