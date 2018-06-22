/*
 ============================================================================
 Name        : p18core.c
 Author      : moskvin
 Version     :
 Copyright   : ELINS
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "tasks.h"
#include "config.h"

int main(void) {
    // setup internal HFINTOSC postscaler 
    OSCCONbits.IRCF = 0x6;  // 8Mhz
    while(OSCCONbits.HFIOFS != 1);
    
    // all analog inputs as digital
    ANSEL = 0; ANSELH = 0;
    
    // setup debug pin
    DBG_PIN_LOW();
    DBG_PIN_C = 0;

    // hardware initialization
    ChargerReset();
    ChargerConfig(DEFAULT_V0, DEFAULT_I0);

    // schedule tasks
	SchedulerInit();
    // charger task
    static chargerMonitorParams_t chargerMonitorParams = { {sizeof chargerMonitorParams} };
    chargerMonitorParams.base.flags.bits.bIsLoop = 1;
    Schedule(ChargerMonitorTask, (baseParam_t*)&chargerMonitorParams, 100);

    // Enable interrupts
    INTCONbits.PEIE = 1;
    ei();
	
    while(1)
	{
		Scheduler();
	}

	return EXIT_SUCCESS;
}
