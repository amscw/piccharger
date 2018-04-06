/* 
 * File:   charger.h
 * Author: moskw
 *
 * Created on 6 апреля 2018 г., 11:18
 */

#ifndef CHARGER_H
#define	CHARGER_H


#include "common.h"

#ifdef	__cplusplus
extern "C" {
#endif

    typedef union 
    {
        struct fields {
            unsigned CHARGE_INHIBITED : 1;
            unsigned MASTER_MODE : 1;
            unsigned VOLTAGE_NOTREG : 1;
            unsigned CURRENT_NOTREG : 1;
            unsigned LEVEL_2 : 1;
            unsigned LEVEL_3 : 1;
            unsigned CURRENT_OR : 1;
            unsigned VOLTAGE_OR : 1;
            unsigned THERMISTOR_OR : 1;
            unsigned THERMISTOR_COLD : 1;
            unsigned THERMISTOR_HOT : 1;
            unsigned THERMISTOR_UR : 1;
            unsigned ALARM_INHIBITED : 1;
            unsigned POWER_FAIL : 1;
            unsigned BATTERY_PRESENT : 1;
            unsigned AC_PRESENT : 1;
        } bits;
        uint16_t word;
    } chargerStatus_t;
    
    typedef enum cmd_
    {
        CHARGER_MODE = 0x12,
        CHARGER_CURRENT = 0x14,
        CHARGER_VOLTAGE = 0x15,
        CHARGER_ALARM_WARNING = 0x16,
        CHARGER_STATUS = 0x13,
    } chargerCmd_t;
    
   err_t ChargerStatus(chargerStatus_t *pStatus);
    
#ifdef	__cplusplus
}
#endif

#endif	/* CHARGER_H */

