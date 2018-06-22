/*
 * tasks.c
 *
 *  Created on: 21 июн. 2018 г.
 *      Author: moskvin
 */

#include "tasks.h"

//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
/**
 * does nothing, for example only
 * @param p dummy param
 */
void dummyTask(paramItem_t* p)
{
	// every task must do cast pointer to it self pointer type
	dummyParams_t *pDummyParam = (dummyParams_t*)p;
	// do something, use params as you wish
#if defined(__DBG_MSG__)
	TRACE("Hello! It's dummy task, P1=%d, P2=%d", pDummyParam->p1, pDummyParam->p2);
#endif
	// ...
	// set task options here, for example change priority
}

void ChargerMonitorTask(paramItem_t *p)
{
    static chargerStatus_t chargerStatus;
    // chargerMonitorParams_t *pChargerMonitorParams = (chargerMonitorParams_t*)p;
    
    if (IsChargerStatusChange() == ERR_TRUE)
        chargerStatus = GetChargerStatus();
    if(VIN_ADC == 1) 
    {
        // on-line
        // TODO: ???? ?????????? ???????? ????????, ??????? ?????? ????????? ??????
    }
}

void ChargerResetTask(paramItem_t *p)
{
    ChargerReset();
    
    chargerMonitorParams_t chargerMonitorParams = { {sizeof chargerMonitorParams} };
    chargerMonitorParams.base.flags.bits.bIsLoop = 1;
    Schedule(ChargerMonitorTask, (baseParam_t*)&chargerMonitorParams, 100);
}
           
//if (( current >= 0 && current < 100 && !chargerStatus.bits.CURRENT_NOTREG ) || chargerStatus.bits.POWER_FAIL)
//{
//    ChargerReset();
//    ChargerConfig(DEFAULT_V0, DEFAULT_I0);
//}

