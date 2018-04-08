/*
 * File:   main.c
 * Author: moskw
 * Peripherial Libraries Help: User Guide, p.472 
 * Configuration registers Help: Datasheet, p.251 
 * Created on 5 апреля 2018 г., 15:30
 */

#include <xc.h>
#include <p18cxxx.h>
#include "config.h"
#include "charger.h"

// for test only
#include "OneWire.h"

void main(void) {
    err_t err;
    
    // setup internal HFINTOSC postscaler 
    OSCCONbits.IRCF = 0x6;  // 8Mhz
    while(OSCCONbits.HFIOFS != 1);
    
    // all analog inputs as digital
    ANSEL = 0;
 
    // initialize charger
    err = ChargerReset();
    err = ChargerConfig(DEFAULT_V0, DEFAULT_I0);
    
    while (1)
    {
        if (IsChargerStatusChange() == ERR_TRUE)
        {
          
        }
      
    }
}
