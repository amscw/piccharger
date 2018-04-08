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
        struct {
            unsigned CHARGE_INHIBITED : 1;
            unsigned MASTER_MODE : 1;
            unsigned VOLTAGE_NOTREG : 1;        // "not regulated" 1 - ???????? ?????? ????????????? ?? ???? 
            unsigned CURRENT_NOTREG : 1;        // "not regulated" 1 - ???????? ?????? ????????????? ?? ?????????? 
            unsigned LEVEL_2 : 1;
            unsigned LEVEL_3 : 1;
            unsigned CURRENT_OR : 1;
            unsigned VOLTAGE_OR : 1;
            unsigned THERMISTOR_OR : 1;
            unsigned THERMISTOR_COLD : 1;
            unsigned THERMISTOR_HOT : 1;
            unsigned THERMISTOR_UR : 1;
            unsigned ALARM_INHIBITED : 1;
            unsigned POWER_FAIL : 1;            // 1 - Vbatt > 89% Vdcin
            unsigned BATTERY_PRESENT : 1;
            unsigned AC_PRESENT : 1;
        } bits;
        uint16_t word;
    } chargerStatus_t;
    
    typedef union
    {
        struct {
            unsigned INHIBIT_CHARGE : 1;
            unsigned ENABLE_POLLING : 1;        // must be 0
            unsigned POR_RESET : 1;
            unsigned RESET_TO_ZERO : 1;         // must be 0
            unsigned bit_4 : 1;                 // must be 1
            unsigned BATTERY_PRESENT_MASK : 1;
            unsigned POWER_FAIL_MASK : 1;
            unsigned _7 : 1;                    // must be 1
            unsigned _8_9 : 2;                  // must be 1
            unsigned HOT_SHOP : 1;
            unsigned _11_15 : 5;
        } bits;
        uint16_t word;
    } chargerMode_t;
    
    typedef enum cmd_
    {
        CHARGER_MODE = 0x12,
        CHARGING_CURRENT = 0x14,
        CHARGING_VOLTAGE = 0x15,
        CHARGER_ALARM_WARNING = 0x16,
        CHARGER_STATUS = 0x13,
    } chargerCmd_t;
   
       
    err_t ChargerStatus(chargerStatus_t *pStatus);
    err_t ChargerMode(chargerMode_t chargerMode);
    err_t ChargerReset();
    err_t ChargingCurrent(uint16_t I0);
    err_t ChargingVoltage(uint16_t V0);
    err_t IsChargerStatusChange();
    inline chargerStatus_t GetChargerStatus();
    


    
#ifdef	__cplusplus
}
#endif

#endif	/* CHARGER_H */

