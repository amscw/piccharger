/**
 *
 */

#include <stdio.h>
#include <string.h>
#include "scheduler.h"
#if defined(__XC8__)
    #include <scheduler.h> 
#endif


//-----------------------------------------------------------------------------
// macro
//-----------------------------------------------------------------------------
#define TIMERS_COUNT		5
#define TIMER_1TICK_TIME	1	// interrupt frequency in ms
#define TIMER_NCYCLES       2000
#define TASK_BUF_SIZE		32

#if defined(__DBG_MSG__)
	#define TASK_SET_STATE(t,s) { TRACE("task \"%c\" switched to: %s", t.idx, strState[s]); t.state = s; }
#else
	#define TASK_SET_STATE(t,s) { t.state = s; }
#endif




//-----------------------------------------------------------------------------
// types
//-----------------------------------------------------------------------------
typedef struct task_
{
	char idx;
	callback_t fn;
	paramItem_t params[TASK_BUF_SIZE];
	volatile enum { IDLE = 0, SCHEDULED, EXEC } state;
} task_t;

//-----------------------------------------------------------------------------
// variables
//-----------------------------------------------------------------------------
#if defined(__DBG_MSG__)
static const char *strState[] = {
		"IDLE", "SCHEDULED", "EXEC",
};
#endif 

static union Timers TMR0Value = {0xFFFF - TIMER_NCYCLES};

static struct timer_
{
	struct timer_ *pNext, *pPrev;
	task_t task;
	volatile struct 
	{
		uint16_t nTicksToExec;
		uint16_t nTicksCurrent;
		uint8_t bON;
	} timeBase;
}
#if defined(__GNUC__)
	__attribute__((packed)) timers[TIMERS_COUNT] = { 0 };
#else
	timers[TIMERS_COUNT] = { 0 };
#endif

static struct timer_ *pFirst = NULL;
#if defined(__MINGW32__)
static UINT_PTR timerID = 0;
#endif

//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
#if defined(__DBG_MSG__)
void ShowTaskList(void)
{

}
#endif

/**
 * schedule task with selected params
 * @param fn action to execution
 * @param pBP pointer to parameter structure
 * @param timeToExec time in ms to execution
 * @return error code (see common.h)
 */
err_t Schedule(callback_t fn, baseParam_t *pBP, uint16_t timeToExec)
{
	struct timer_ *pFreeTimer;
	
	if (pBP == NULL)
	{
#if defined(__DBG_MSG__)
		TRACE("parameters is null!");
#endif
		return ERR_NULLPTR;
	}

	if (pBP->length > TASK_BUF_SIZE)
	{
#if defined(__DBG_MSG__)
		TRACE("too long parameters (%d/%d)!", pBP->length, TASK_BUF_SIZE);
#endif
		return ERR_CORE_NO_MEM;
	}

	// find free timer
	for (pFreeTimer = timers; pFreeTimer < (timers + TIMERS_COUNT) && pFreeTimer->task.state != IDLE; pFreeTimer++);
	if (pFreeTimer == timers + TIMERS_COUNT)
	{
		// no free timer found
#if defined(__DBG_MSG__)
		TRACE("no free timers!");
#endif
		// TODO: put the task into queue
		return ERR_CORE_NO_FREE_TIMERS;
	}
	
	// setup timer function
	pFreeTimer->task.fn = fn;

	// setup timer parameters
	memcpy(pFreeTimer->task.params, (paramItem_t*)pBP, pBP->length);

	// add timer to list
	pFreeTimer->pNext = NULL;
	if (pFirst != NULL) {
		struct timer_ *pT;
		for (pT = pFirst; pT->pNext != NULL; pT++);
		pT->pNext = pFreeTimer;
		pFreeTimer->pPrev = pT;
	} else {
		pFirst = pFreeTimer;
		pFreeTimer->pPrev = NULL;
	}

	// setup timer state
	if (timeToExec == 0) {
		TASK_SET_STATE(pFreeTimer->task, EXEC);
	} else {
		memset((void*)&pFreeTimer->timeBase, 0, sizeof pFreeTimer->timeBase);
		pFreeTimer->timeBase.nTicksToExec = timeToExec / TIMER_1TICK_TIME;
		TASK_SET_STATE(pFreeTimer->task, SCHEDULED);
		pFreeTimer->timeBase.bON = 1;
	}

	return ERR_OK;
}

inline void Scheduler(void)
{
#if defined(__MINGW32__)
	MSG msg;
	GetMessage(&msg, NULL, 0, 0);
	DispatchMessage(&msg);
#endif

	struct timer_ *pT;
	baseParam_t *pBP;

	for (pT = pFirst; pT; pT = pT->pNext)
	{
		if (pT->task.state == EXEC)
		{
			// run task
			pT->task.fn(pT->task.params);

			// repeat the task, if need
			pBP = (baseParam_t*)pT->task.params;
			if (pBP->flags.bits.bIsLoop)
			{
				// restart timer
				pT->timeBase.nTicksCurrent = 0;
				pT->timeBase.bON = 1;
				TASK_SET_STATE(pT->task, SCHEDULED);
			} else {
				// free timer
				if (pT->pNext != NULL)
					pT->pNext->pPrev = pT->pPrev;	// connect next timer with previous, if exist
				if (pT->pPrev != NULL)
					pT->pPrev->pNext = pT->pNext;	// connect previous timer with next, if exist
				else
					pFirst = pT->pNext;
				TASK_SET_STATE(pT->task, IDLE);
			}
		}
	}
}

#if defined(__MINGW32__)
/**
 * An application-defined callback function that processes WM_TIMER messages.
 * @param hwnd A handle to the window associated with the timer.
 * @param uMsg The WM_TIMER message.
 * @param idEvent The timer's identifier.
 * @param dwTime The number of milliseconds that have elapsed since the system was started.
 */
VOID CALLBACK t0Int(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
#elif defined (__XC8__)
void low_priority interrupt tcInt(void)
#endif
{
	static struct timer_ *pT;
    

#if defined(__XC8__)
    DBG_PIN_HIGH();
    // any timer 0 interrupts?
    if (TMR0IE && TMR0IF)
    {
        TMR0IF = 0;
#endif
    
        for (pT = pFirst; pT; pT = pT->pNext)
        {
            if (pT->timeBase.bON)
            {
                pT->timeBase.nTicksCurrent++;
                if (pT->timeBase.nTicksCurrent == pT->timeBase.nTicksToExec) {
                    pT->timeBase.bON = 0;	// ring-ring!
                    TASK_SET_STATE(pT->task, EXEC);
                }
            }
        }
#if defined(__XC8__)
        // WriteTimer0(0xFFFF - TIMER_NCYCLES);
        TMR0H = TMR0Value.bt[1];  // Write high byte to Timer0
        TMR0L = TMR0Value.bt[0];  // Write low byte to Timer0
        DBG_PIN_LOW();
    }
#endif
}

err_t SchedulerInit(void)
{
	for (int i = 0; i < TIMERS_COUNT; i++)
		timers[i].task.idx = i + 'A';

#if defined(__MINGW32__)
	timerID = SetTimer(NULL, 0, TIMER_1TICK_TIME, t0Int);
	if (timerID == 0)
	{
		TRACE("function fails to create a timer");
		return ERR_CONFIGURE;
	}
	return ERR_OK;
#elif defined(__XC8__)
    // p.478 in help
    OpenTimer0(TIMER_INT_ON | T0_16BIT | T0_SOURCE_INT | T0_PS_1_4);
    WriteTimer0(0xFFFF - TIMER_NCYCLES);
#endif
}
