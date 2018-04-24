#include "DS2788_OneWireProtocol.h"

void copy_EEPROM(unsigned char data)
{
    OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x48, data);
}

void recall_EEPROM(unsigned char addr)
{
    OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0xB8, addr);
}

void write_data(unsigned char addr, unsigned char data)
{
    OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x6C, addr);
    OneWire_Send8(data);
}

uint8_t read_data(uint8_t addr)
{
    unsigned char result = 0;
    OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x69,addr);
    result  = OneWire_Receive8();

    return result;
}

// Write IAE register - "discharge current threshold used to detect the active empty point" and Copy to EEPROM
void write_IAE()
{
   /* OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x6C, 0x67);
    OneWire_Send8(0x05);//0x2C//0xDC*/
    write_data(0x67,VAL_IAE);//0x05
    copy_EEPROM(0x67);
}

// Write VAE register - "voltage threshold used to detect the active empty point" and Copy to EEPROM
void write_VAE()
{
  /*  OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x6C, 0x66);
    OneWire_Send8(0xB3);//0xB3//0x99*/
    write_data(0x66,VAL_VAE); //0xB3 // 0x9A = 3 V malo
    copy_EEPROM(0x66);
}

// Write IMIN register - "charge current threshold used to detect a fully charged state" and Copy to EEPROM
void write_IMIN()
{
    /*OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x6C, 0x65);
    OneWire_Send8(0xDC);//0xDC*/
    write_data(0x65,VAL_IMIN); //0xDC
    copy_EEPROM(0x65);
}

// Write VCHG register - "charge voltage threshold used to detect a fully charged state" and Copy to EEPROM
void write_VCHG()
{
   /* OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x6C, 0x64);
    OneWire_Send8(0xD5);*/
    write_data(0x64,VAL_VCHG); //0xD5
    copy_EEPROM(0x64);
}

// Write RSNSP - "value of the sense resistor" and Copy to EEPROM
void write_RSNSP()
{
    /*OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x6C, 0x69);
    OneWire_Send8(0x2D);*/
    write_data(0x69,VAL_RSNS); //0x2d
    copy_EEPROM(0x69);
}

void write_DC()
{
    /*OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x6C, 0x60);
    OneWire_Send8(0x00);*/
    write_data(0x60,0x00);
    copy_EEPROM(0x60);
}

void write_AS()
{
    write_data(0x14,VAL_AS); // 0x4B
    copy_EEPROM(0x14);
}

void write_AC()
{
    /*OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x6C, 0x63);
    OneWire_Send8(0x7F);*/
    write_data(0x63,0x80); //0x80//0x7F
    copy_EEPROM(0x63);

    __delay_ms(timecopy);
    
    /*OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x6C, 0x62);
    OneWire_Send8(0x5d);*/
    write_data(0x62,0x5D); //0x89//0x5D
    copy_EEPROM(0x62);
}

void write_ACR( uint8_t ACR_MSB, uint8_t ACR_LSB)
{
    /*OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x6C, 0x10);
    OneWire_Send8(ACR_MSB);*/
    write_data(0x10, ACR_MSB);
    copy_EEPROM(0x10);

    /*OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x6C, 0x11);
    OneWire_Send8(ACR_LSB);*/
    write_data(0x11,ACR_LSB);
    copy_EEPROM(0x11);
}

void write_FULL50(unsigned char FULL50_MSB, unsigned char FULL50_LSB)
{
    write_data(0x6A,FULL50_MSB);
    copy_EEPROM(0x6A);

     __delay_ms(timecopy);

    write_data(0x6B,FULL50_LSB);
    copy_EEPROM(0x6B);
}

void clear_UVF_PORF()
{
    uint8_t result = 0;
    OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x69,0x01);
    result = OneWire_Receive8();
    
    result = (uint8_t)(result & 0xF9);
    
    OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x6C,0x01);
    OneWire_Send8(result);
    copy_EEPROM(0x60);
}

void config_DS2788()
{
     write_IAE();
     __delay_ms(timecopy);
     write_VAE();
     __delay_ms(timecopy);
     write_IMIN();
     __delay_ms(timecopy);
     write_VCHG();
     __delay_ms(timecopy);
     write_RSNSP();
     __delay_ms(timecopy);
     /*write_DC();
     __delay_ms(timecopy);*/
     write_AS();
     __delay_ms(timecopy);
     write_AC();
     __delay_ms(timecopy);
}

struct Control_Register read_ControlRegister()
{
    unsigned char res = 0;
    struct Control_Register contr;
    
    OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x69,0x60);
    res = OneWire_Receive8();

    contr.NBEN = bit_get(res,7);
    contr.UVEN = bit_get(res,6);
    contr.PMOD = bit_get(res,5);
    contr.RNAOP = bit_get(res,4);
    contr.DC = bit_get(res,3);

    return contr;
}

union EEPROM_Block1_model read_EEPROM_Block_1()
{
    union EEPROM_Block1_model EBM;
    for( uint8_t i = 0; i < 32; i++ )
    {
        /*OneWire_Init();
        OneWire_SkipNetAddress();
        OneWire_Send16(0x69,0x60+i);
        result[i]  = OneWire_Receive8();*/
        EBM.result[i] = read_data((uint8_t)(0x60+i));
    }
    return EBM;
}

struct Status_Register read_Battery_status_only()
{
    unsigned char res = read_data(0x01);
    /*OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x69,0x01);
    res = OneWire_Receive8();*/
    struct Status_Register status = set_Status(res);
    return status;
}

int read_current_only()
{
    unsigned char result[2];
    int res;
    int sign = 0;

    /*OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x69,0x0E);
    result[0] = OneWire_Receive8();

    OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x69,0x0F);
    result[1] = OneWire_Receive8();*/

    result[0] = read_data(0x0E);
    result[1] = read_data(0x0F);

    sign = ((result[0]&0x80)>>7);
    res =  (int) ( (((result[0] & 0x7F) << 8) + result[1] - (32768 * sign) ) * coefficient_I );
    return res;
}

int read_ACR_only()
{
    unsigned char ACR_MSB, ACR_LSB;
    int res = 0;

    /*OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x69,0x10);
    ACR_MSB = OneWire_Receive8();

    OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x69,0x11);
    ACR_LSB = OneWire_Receive8();*/

    ACR_MSB = read_data(0x10);
    ACR_LSB = read_data(0x11);

    res = ( ACR_MSB << 8 ) | ACR_LSB;
    return res;
}

int read_volt_only()
{
    unsigned char result[2];
    int res;
    int sign = 0;

    /*OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x69,0x0C);
    result[0] = OneWire_Receive8();

    OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x69,0x0D);
    result[1] = OneWire_Receive8();*/

    result[0] = read_data(0x0C);
    result[1] = read_data(0x0D);

    sign = ((result[0]&0x80)>>7);
    res =  (int)( (((result[0]&0x7F)<<3)+((result[1]&0xE0)>>5))*coefficient_U );
    res = res - 2*res*sign;
    return res;
}

unsigned int read_RAAC_only()
{
    unsigned char result[2];
    unsigned int res;

    /*OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x69,0x02);
    result[0] = OneWire_Receive8();

    OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x69,0x03);
    result[1] = OneWire_Receive8();*/

    result[0] = read_data(0x02);
    result[1] = read_data(0x03);

    res =  (unsigned int)(((result[0]<<8)+result[1])*coefficient_Remaining);
    return res;
}

unsigned int read_RSAC_only()
{
    unsigned char result[2];
    unsigned int res;

    /*OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x69,0x04);
    result[0] = OneWire_Receive8();

    OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x69,0x05);
    result[1] = OneWire_Receive8();*/

    result[0] = read_data(0x04);
    result[1] = read_data(0x05);

    res =  (unsigned int) ( ((result[0]<<8)+result[1])*coefficient_Remaining );
    return res;
}

unsigned char read_RARC_only()
{
    unsigned char res = read_data(0x06);

    /*OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x69,0x06);
    res = OneWire_Receive8();*/

    return res;
}

unsigned char read_RSRC_only()
{
    unsigned char res = read_data(0x07);

    /*OneWire_Init();
    OneWire_SkipNetAddress();
    OneWire_Send16(0x69,0x07);
    res = OneWire_Receive8();*/

    return res;
}


struct Battery_Model read_Battery_Model()
{
    unsigned char result[27];
    struct Battery_Model battery;
    int sign = 0;
    for( uint8_t i = 0; i < 27; i++ )
    {
       /* OneWire_Init();
        OneWire_SkipNetAddress();
        OneWire_Send16(0x69,i+1);
        result[i] = OneWire_Receive8();*/
        result[i] = read_data((uint8_t)(i+1));
    }

    battery.STATUS = set_Status(result[0]);
    battery.RAAC = (unsigned int)( ((result[1]<<8)+result[2])*coefficient_Remaining );
    battery.RSAC = (unsigned int)( ((result[3]<<8)+result[4])*coefficient_Remaining );
    battery.RARC = result[5];
    battery.RSRC = result[6];

    sign = ((result[7]&0x80)>>7);
    battery.IAVG = (int)( (((result[7]&0x7F)<<8)+result[8]-(32768*sign))*coefficient_I );
    battery.IAVG =  battery.IAVG - 2* battery.IAVG*sign;

    sign = ((result[9]&0x80)>>7);
    battery.TEMP = (int)( (((result[9]&0x7F)<<3)-(2047*sign))*coefficient_T );

    sign = ((result[11]&0x80)>>7);
    battery.VOLT = (int) ( (((result[11]&0x7F)<<3)+((result[12]&0xE0)>>5))*coefficient_U );
    battery.VOLT =  battery.VOLT - 2* battery.VOLT*sign;

    sign = ((result[13]&0x80)>>7);
    battery.CURRENT = (int) ( (((result[13]&0x7F)<<8)+result[14]-(32768*sign))*coefficient_I);
    battery.CURRENT =  battery.CURRENT - 2* battery.CURRENT*sign;

    battery.ACR_MSB = result[15];
    battery.ACR_LSB = result[16];
    battery.ACRL_MSB = result[17];
    battery.ACRL_LSB = result[18];
    battery.AS = (uint8_t) (result[19]*coefficient_AS);
    battery.SFR = result[20];
    battery.FULL_MSB = result[21];
    battery.FULL_LSB = result[22];
    battery.AE_MSB = result[23];
    battery.AE_LSB = result[24];
    battery.SE_MSB = result[25];
    battery.SE_LSB = result[26];

    return battery;
}

struct Status_Register set_Status(unsigned char byte)
{
    struct Status_Register stat;
    stat.CHGTF = bit_get(byte,7);
    stat.AEF = bit_get(byte,6);
    stat.SEF = bit_get(byte,5);
    stat.LEARNF = bit_get(byte,4);
    stat.UVF = bit_get(byte,2);
    stat.PORF = bit_get(byte,1);
    return stat;
}

boolean write_to_user_EEPROM( unsigned char addr, unsigned char value )
{
    boolean res = false;
    if( addr >= 0x20 && addr <= 0x2F )
    {
        write_data(addr,value);
        copy_EEPROM(addr);

        res = true;
    }

    return res;
}

unsigned char read_from_user_EEPROM( unsigned char addr )
{
    unsigned char res = read_data(addr);
    return res;
}

struct Battery_State read_Battery_State()
{
    unsigned char result[15];
    struct Battery_State battery;
    int sign = 0;
    for( uint8_t i = 0; i < 15; i++ )
    {
       /* OneWire_Init();
        OneWire_SkipNetAddress();
        OneWire_Send16(0x69,i+1);
        result[i] = OneWire_Receive8();*/
        result[i] = read_data((uint8_t)(i+1));
    }

    battery.STATUS = set_Status(result[0]);
    battery.RAAC =  (unsigned int)( ((result[1]<<8)+result[2])*coefficient_Remaining );
    battery.RSAC =  (unsigned int)( ((result[3]<<8)+result[4])*coefficient_Remaining );
    battery.RARC = result[5];
    battery.RSRC = result[6];

    sign = ((result[7]&0x80)>>7);
    battery.IAVG = (int) ( (((result[7]&0x7F)<<8)+result[8]-(32768*sign))*coefficient_I);
    battery.IAVG =  battery.IAVG - 2* battery.IAVG*sign;

    sign = ((result[9]&0x80)>>7);
    battery.TEMP = (int) ( (((result[9]&0x7F)<<3)-(2047*sign)) * coefficient_T );

    sign = ((result[11]&0x80)>>7);
    battery.VOLT = (int) ( (((result[11]&0x7F)<<3)+((result[12]&0xE0)>>5))*coefficient_U);
    battery.VOLT =  battery.VOLT - 2* battery.VOLT*sign;

    sign = ((result[13]&0x80)>>7);
    battery.CURRENT = (int)( (((result[13]&0x7F)<<8)+result[14]-(32768*sign))*coefficient_I) ;
    battery.CURRENT =  battery.CURRENT - 2* battery.CURRENT*sign;

    return battery;
}