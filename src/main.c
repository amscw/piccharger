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

static chargerStatus_t g_ChargerStatus;

void main(void) {
    ChargerStatus(&g_ChargerStatus);
    return;
}
