/* 
 * File:   DS2788_OneWireProtocol.h
 * Author: Twix
 *
 * Created on 14 ??? 2015 ?., 18:03
 */

#ifndef DS2788_ONEWIREPROTOCOL_H
#define	DS2788_ONEWIREPROTOCOL_H

#include "OneWire.h"
#include "common.h"

#define VAL_IAE     0x17
#define VAL_VAE     0xB2  //0xAF
#define VAL_IMIN    0xFF
#define VAL_VCHG    0xD2
#define VAL_RSNS    0x16
#define VAL_AS      0x79


typedef enum { false = 0, true } boolean;

struct Status_Register
{
    boolean CHGTF; // Charge Termination Flag
    boolean AEF; // Active Empty Flag
    boolean SEF; // Standby Empty Flag
    boolean LEARNF; // Learn Flag?When set to 1, a charge cycle can be used to learn battery capacity
    boolean UVF; // Undervoltage Flag
    boolean PORF; // Power-On Reset Flag?Useful for reset detection, see text below
};

struct Battery_Model
{
    struct Status_Register STATUS;
    unsigned int RAAC; //Remaining Active Absolute Capacity
    unsigned int RSAC; //Remaining Standby Absolute Capacity
    unsigned char RARC; //Remaining Active Relative Capacity
    unsigned char RSRC; //Remaining Standby Relative Capacity
    int IAVG; //Average Current
    int TEMP; //Temperature
    int VOLT; //Voltage
    int CURRENT; //Current
    unsigned char ACR_MSB; //Low Accumulated Current Register MSB
    unsigned char ACR_LSB; //Low Accumulated Current Register LSB
    unsigned char ACRL_MSB; //Low Accumulated Current Register MSB
    unsigned char ACRL_LSB; //Low Accumulated Current Register LSB
    unsigned char AS; //Age Scalar
    unsigned char SFR; //Special Feature Register
    unsigned char FULL_MSB; //Full Capacity MSB
    unsigned char FULL_LSB; //Full Capacity LSB
    unsigned char AE_MSB; //Active Empty MSB
    unsigned char AE_LSB; //Active Empty LSB
    unsigned char SE_MSB; //Standby Empty MSB
    unsigned char SE_LSB; //Standby Empty LSB
};

struct Control_Register
{
    boolean NBEN;
    boolean UVEN;
    boolean PMOD;
    boolean RNAOP;
    boolean DC;
};

union Control_Register_model
{
    struct Control_Register control_reg;
    unsigned char cntrl_rg;
};

struct EEPROM_Block1
{
    unsigned char Control_Reg;
    unsigned char AB;
    unsigned char AC_MSB;
    unsigned char AC_LSB;
    unsigned char VCHG;
    unsigned char IMIN;
    unsigned char VAE;
    unsigned char IAE;
    unsigned char Active_Empty_50;
    unsigned char RSNSP;
    unsigned char Full_50_MSB;
    unsigned char Full_50_LSB;
    unsigned char Full_Segment_4;
    unsigned char Full_Segment_3;
    unsigned char Full_Segment_2;
    unsigned char Full_Segment_1;
    unsigned char AE_Segment_4;
    unsigned char AE_Segment_3;
    unsigned char AE_Segment_2;
    unsigned char AE_Segment_1;
    unsigned char SE_Segment_4;
    unsigned char SE_Segment_3;
    unsigned char SE_Segment_2;
    unsigned char SE_Segment_1;
    unsigned char RSGAIN_MSB;
    unsigned char RSGAIN_LSB;
    unsigned char RSTC;
    unsigned char COB;
    unsigned char TBP_23;
    unsigned char TBP_12;
    unsigned char VGAIN_MSB;
    unsigned char VGAIN_LSB;
};

union EEPROM_Block1_model
{
    struct EEPROM_Block1 EBM;
    unsigned char result[32];
};

struct Battery_State
{
    struct Status_Register STATUS;
    unsigned int RAAC; //Remaining Active Absolute Capacity
    unsigned int RSAC; //Remaining Standby Absolute Capacity
    unsigned char RARC; //Remaining Active Relative Capacity
    unsigned char RSRC; //Remaining Standby Relative Capacity
    int IAVG; //Average Current
    int TEMP; //Temperature
    int VOLT; //Voltage
    int CURRENT; //Current
};

const float coefficient_I = 0.071;
const float coefficient_T = 0.125;
const float coefficient_U = 4.88;
const float coefficient_ACR = 0.2841;
const float coefficient_Remaining = 1.6;
const float coefficient_AS = 0.78125;

#define timecopy  10

void copy_EEPROM(unsigned char data);
void recall_EEPROM(unsigned char addr);
void write_data(unsigned char addr, unsigned char data);

void write_IAE();
void write_VAE();
void write_IMIN();
void write_VCHG();
void write_RSNSP();
void write_DC();
void write_AC();
void write_AS();
void write_ACR( uint8_t ACR_MSB, uint8_t ACR_LSB);
void write_FULL50(unsigned char FULL50_MSB, unsigned char FULL50_LSB);

boolean write_to_user_EEPROM( unsigned char addr, unsigned char value );
unsigned char read_from_user_EEPROM( unsigned char addr );
struct Battery_State read_Battery_State();

void config_DS2788();

union EEPROM_Block1_model read_EEPROM_Block_1();
struct Control_Register read_ControlRegister();
struct Battery_Model read_Battery_Model();
struct Status_Register set_Status(unsigned char byte);
struct Status_Register read_Battery_status_only();
int read_current_only();
int read_ACR_only();
int read_volt_only();
unsigned int read_RAAC_only();
unsigned int read_RSAC_only();
unsigned char read_RARC_only();
unsigned char read_RSRC_only();

#endif	/* DS2788_ONEWIREPROTOCOL_H */

