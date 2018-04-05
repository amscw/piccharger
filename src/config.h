/* 
 * File:   config.h
 * Author: moskw
 *
 * Created on 5 апреля 2018 г., 21:43
 */

#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif


#pragma config IESO=ON, FCMEN=ON, PCLKEN=OFF, PLLEN=ON, FOSC=IRC    // CONFIG1H 
#pragma config BORV=19, BOREN=SBORDIS, PWRTEN=ON                    // CONFIG2L
#pragma config WDTPS=1, WDTEN=OFF                                   // CONFIG2H
#pragma config MCLRE=ON, HFOFST=OFF                                 // CONFIG3H
#pragma config DEBUG=OFF, XINST=OFF, BBSIZ=OFF, LVP=OFF, STVREN=OFF // CONFIG4L
#pragma config CP0=OFF, CP1=OFF                                     // CONFIG5L
#pragma config CPB=OFF, CPD=OFF                                     // CONFIG5H
#pragma config WRT0=OFF, WRT1=OFF                                   // CONFIG6L
#pragma config WRTC=OFF, WRTB=OFF, WRTD=OFF                         // CONFIG6H
#pragma config EBTR0=OFF, EBTR1=OFF                                 // CONFIG7L
#pragma config EBTRB=OFF                                            // CONFIG7H    
    
    
#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

