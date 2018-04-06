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

#define _XTAL_FREQ 8000000
    
//|+----------------------------------------------------------------------+|
//|Some def functions
//|+----------------------------------------------------------------------+|
#define bit_set(var,bitno)  ((var) |=   1UL << (bitno))
#define bit_clr(var,bitno)  ((var) &= ~(1UL << (bitno)))
#define bit_get(data,bitno) ((data >> bitno) & 0x01)
 
typedef enum err_ { 
    ERR_OK,
    ERR_NULLPTR,
    ERR_SWI2C_READ,        
    ERR_SWI2C_WRITE,
    ERR_SWI2C_NOACK,
} err_t;


#ifdef	__cplusplus
}
#endif

#endif	/* COMMON_H */

