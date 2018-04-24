/* 
 * File:   alarms.h
 * Author: moskw
 *
 * Created on 9 ?????? 2018 ?., 17:28
 */

#ifndef ALARMS_H
#define	ALARMS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common.h"    
    
enum alarm_t 
{
	ALARM_RESTART_CHARGER,
};

enum alarmState_t
{
	ALARM_STATE_OFF,
	ALARM_STATE_ON,
	ALARM_STATE_RING,
};

void AlarmsInit(void);

/**
 * setup alarm clock 
 * param alarm - clock id
 * param time_ms - time to ring-ring-ring in milliseconds
 */ 
void AlarmSet(enum alarm_t alarm, uint32_t time_ms);

/**
 * Get current time
 * return time in milliseconds since clock switched on
 */
uint32_t AlarmGet(enum alarm_t alarm);

/**
 * Check state of alarm clock
 * param alarm - clock id
 * return one of ALARM_STATE_t values:
 * ALARM_STATE_OFF - alarm is switched off
 * ALARM_STATE_ON - alarm is going on
 * ALARM_STATE_RING - alarm is reached alarm time; 
 */
enum alarmState_t AlarmCheck(enum alarm_t alarm);

/**
 * Disable alarm clock
 *  
 */
void AlarmOff(enum alarm_t alarm);
#ifdef	__cplusplus
}
#endif

#endif	/* ALARMS_H */

