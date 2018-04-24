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
#include "alarms.h"

// for test only
#include "OneWire.h"
#include "DS2788_OneWireProtocol.h"




static unsigned int RAAC;
static unsigned int RARC;
static unsigned int max_RAAC = default_RAAC;

unsigned char max_RAAC_addr1 = 0x20;
unsigned char max_RAAC_addr2 = 0x22;
unsigned char config_state_addr = 0x24;
unsigned char charger_state = 0;

unsigned char calibration_state = 0;
unsigned char atx_state = 0;
unsigned char config_state = 0;
unsigned char timer_2_s = 30;
unsigned char timer_flag = 0;
unsigned char set_charger = 0;
unsigned char battery_present = 0;

int one_wire_init;
unsigned char stat1;
unsigned char stat2;

int current;
int voltage;
int charger_voltage = 16800;
int charger_current = charge_current_off;

void disabled_indication() {
    IND1 = 0; IND2 = 0; IND3 = 0; IND4 = 0; IND5 = 0; IND6 = 0;
}

#define PORTS() PORTA = 0b00000000; PORTB = 0b00000000;  PORTC = 0b00000000;  // SIGNAL
#define TRIS()  TRISA = 0b11101111; TRISB = 0b01011111;  TRISC = 0b10000000;//TRISA = 0b00000000; TRISB = 0b00010000;  TRISC = 0b10001010;  // IN/OUT
#define WPU()   WPUA = 0b00000000; WPUB = 0b00000000;


void init_proc() {
    one_wire_init = OneWire_Init();
    disabled_indication();
}

void charge_indication() {
    if( RARC < 30 ) {
        IND3 = 0;
        IND2 = 0;
        IND1 = !IND1;
    } else if( RARC < 60 ) {
        IND3 = 0;
        IND2 = !IND2;
        IND1 = 1;
    } else if( RARC < 90 ) {
        IND3 = !IND3;
        IND2 = 1;
        IND1 = 1;
    } else {
        IND3 = 1;
        IND2 = 1;
        IND1 = 1;
    }
}

void discharge_indication() {
    
    if( RARC < 30 ) {
        IND3 = 0;
        IND2 = 0;
        IND1 = !IND1;
      /*  if( RARC < 5 )
        {
            BATLOW = 0;
        }
        else
        {
            BATLOW = 1;
        }*/
    } else if( RARC < 60 ) {
        IND3 = 0;
        IND2 = 0;
        IND1 = 1;
    } else if( RARC < 90 ) {
        IND3 = 0;
        IND2 = 1;
        IND1 = 1;
    } else {
        IND3 = 1;
        IND2 = 1;
        IND1 = 1;
    }
}

void one_wire() {
    if( one_wire_init == 1 ) {
        config_state = read_from_user_EEPROM(config_state_addr);
        if( config_state == 0x00 ) {
            config_DS2788();
            write_to_user_EEPROM(config_state_addr,1);
        }
            
        max_RAAC =(unsigned int)((read_from_user_EEPROM(max_RAAC_addr1)) << 8);
        max_RAAC &= 0xFF00;
        max_RAAC += (unsigned int)((read_from_user_EEPROM(max_RAAC_addr2)) & 0xFF);
      //  if(max_RAAC == 0xFFFF || max_RAAC == 0x0000) {
            max_RAAC = default_RAAC;
     //   }
    } else {
        one_wire_init = OneWire_Init();
    }
}

// WARNING! execution time ~1.2s
void batteryState() {
    di();
    if( one_wire_init == 1 ) {
        RAAC = read_ACR_only()*0.22;
        RARC = (RAAC) / (max_RAAC / 100);
        current = read_current_only();
        voltage = read_volt_only();
        
        
      
       // battery_status = read_Battery_status_only();
        //FIXME: setup properly DS2788
        /*
        if( VIN_ADC == 1 && voltage >= 4100 ) {
            max_RAAC =  (max_RAAC + (RAAC * 0.22)) / 2 ;
            write_to_user_EEPROM(max_RAAC_addr1, ( max_RAAC >> 8) & 0xFF);
            __delay_ms(10);
            write_to_user_EEPROM(max_RAAC_addr2, max_RAAC & 0xFF);
            __delay_ms(10);
        }
         **/
    } else {
        one_wire_init = OneWire_Init();
    }
    ei();
}


void main(void) {
    // static err_t err;
    static chargerStatus_t chargerStatus;
    
    // setup internal HFINTOSC postscaler 
    OSCCONbits.IRCF = 0x6;  // 8Mhz
    while(OSCCONbits.HFIOFS != 1);
    
    // all analog inputs as digital
    ANSEL = 0;
    ANSELH = 0;
    VIN_ADC_C = 1;
    // for debug only
    DBG_SIG_LOW();
    DBG_SIG_C = 0;
    
    // set indicator leds to output
    IND_DIS();
    IND_C();
    
    // Alarms
    AlarmsInit();
    
    // 1-wire init
    one_wire_init = OneWire_Init();
    one_wire();
    
    // initialize charger
    if (IsChargerStatusChange() == ERR_TRUE)
        chargerStatus = GetChargerStatus();
    ChargerReset();
    ChargerConfig(DEFAULT_V0, (PR_SDA == 1) ? (uint16_t)1000 : (uint16_t)1500);
    AlarmSet(ALARM_RESTART_CHARGER, 7030);
        
    // Enable interrupts
    INTCONbits.PEIE = 1;
    ei();
    
    while (1)
    {
        // update battery state
        // WARNING! execution time 650ms @ 32MHz osc. All interruts are disabled!
        batteryState();
        
        // update charger state
        if (IsChargerStatusChange() == ERR_TRUE)
            chargerStatus = GetChargerStatus();
        
        if( VIN_ADC == 1 ) {
            // on-line
            
            charge_indication();
            
           // uint8_t bNeedRestart = chargerStatus.bits.VOLTAGE_NOTREG || (!chargerStatus.bits.VOLTAGE_NOTREG && !chargerStatus.bits.CURRENT_NOTREG);          
            
            if (( current >= 0 && current < 100 && !chargerStatus.bits.CURRENT_NOTREG ) || chargerStatus.bits.POWER_FAIL)
            {
#ifdef DEBUG_CHARGER_RESTART
                DBG_SIG_HIGH();
#endif
                // check for time elapsed from last restart
                if ( AlarmCheck(ALARM_RESTART_CHARGER) == ALARM_STATE_RING ) {
                    ChargerReset();
                    ChargerConfig(DEFAULT_V0, DEFAULT_I0);
                    AlarmSet(ALARM_RESTART_CHARGER, 7030);
#ifdef DEBUG_CHARGER_RESTART
                    DBG_SIG_LOW();
                }
            } else {
             DBG_SIG_LOW();   
            }
#else
                }
            }
#endif
            
            
            // set charging current, if need
            if (PR_SDA == 1 && ChargerGetCurrent() != 1000) {
                ChargerSetCurrent(1000);
            } else if (PR_SDA == 0 && ChargerGetCurrent() != 1500){
                ChargerSetCurrent(1500);
            }     
        } else {
            // off-line
            if( PR_SDA == 1 ) {
                // pevm switched-on
                discharge_indication();
            } else {
                // pevm switched-off
                IND_DIS();
            }
        }
        __delay_ms(500);
    }
}
