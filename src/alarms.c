#include "alarms.h"
#include <p18cxxx.h>
#include <plib.h>
#include <string.h>

#define SYSTICK_ALARMS_COUNT	1
#define SYSTICK_NCYCLES         62500
#define SYSTICK_TICK_TIME       1000 // ms   

volatile static struct
{
	uint8_t bON;
	uint32_t alarmTime_ticks;
	uint32_t _ticks;
} alarms[SYSTICK_ALARMS_COUNT];

void AlarmSet(enum alarm_t alarm, uint32_t time_ms)
{
	alarms[alarm]._ticks = 0;
	alarms[alarm].alarmTime_ticks = time_ms/SYSTICK_TICK_TIME;
	alarms[alarm].bON = 1;
}	

uint32_t AlarmGet(enum alarm_t alarm)
{
	return alarms[alarm]._ticks;
}

enum alarmState_t AlarmCheck(enum alarm_t alarm)
{
	if (!alarms[alarm].bON && alarms[alarm]._ticks)
		return ALARM_STATE_RING;
	else if (alarms[alarm].bON)
		return ALARM_STATE_ON;
	else return ALARM_STATE_OFF;
}

void AlarmsInit(void)
{
    // p.478	
    OpenTimer0(TIMER_INT_ON | T0_16BIT | T0_SOURCE_INT | T0_PS_1_128);
    WriteTimer0(0xFFFF - SYSTICK_NCYCLES);
    memset(alarms, 0, sizeof *alarms * SYSTICK_ALARMS_COUNT);
}

void low_priority interrupt tcInt(void)
{
    // any timer 0 interrupts?
    if (TMR0IE && TMR0IF) 
    { 
        // 100ms elapsed
        int i;
        
        TMR0IF=0;
        
        // for debug only:
        // DBG_SIG_TOGGLE();
        
    	for (i = 0; i < SYSTICK_ALARMS_COUNT; i++)
            if(alarms[i].bON)
            {
                alarms[i]._ticks++;
                if (alarms[i]._ticks == alarms[i].alarmTime_ticks)
                {
                    alarms[i].bON = 0;	// ring-ring!
                    // SysTick->CTRL &= 0xFFFFFFFC;	// timer off
                }
            }
        WriteTimer0(0xFFFF - SYSTICK_NCYCLES);
    }
}
