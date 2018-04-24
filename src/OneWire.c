/*
 * File:   1-Wire.c
 * Author: a.v.chebryakov
 *
 * Created on 3 7 2014, 14:10
 */

#include "OneWire.h"
#include "common.h"
#include <xc.h>

//----------------------------------------------------------------------------//
// Function:    OneWire_Init()
// Return:      void
// Arg:         void
//----------------------------------------------------------------------------//
int OneWire_Init()
{
    int res = 0;
    ONE_IO = 0;
    ONE_DATA = 1;
//    ONE_PULL = 1;
    
    __delay_ms(50);
    OneWire_Reset();
    res = OneWire_Presence();
    ONE_DATA = 1;
    ONE_IO = 1;
    return res;
}

//----------------------------------------------------------------------------//
// Function:    OneWire_Reset()
// Return:      void
// Arg:         void
//----------------------------------------------------------------------------//
void OneWire_Reset()
{
    ONE_IO = 0;
    ONE_DATA = 0;
    __delay_us(TIME_RESET);
   // ONE_DATA = 1;
    __delay_us(TIME_IDLE);
    ONE_IO = 1;
}

//----------------------------------------------------------------------------//
// Function:    OneWire_Presence()
// Return:      unsigned char:  1  -  successful
//                              0  -  unsuccessful
// Arg:         void
//----------------------------------------------------------------------------//
unsigned char OneWire_Presence()
{
    int count = 0;
    ONE_IO = 1;
    //__delay_us(60);
    
    while( ONE_DATA == 0 )
    {
        count++;
        if( count == 1000 )
        {
            return 0;
        }
    }
    __delay_us(TIME_RESET);
    return 1;
}

//----------------------------------------------------------------------------//
// Function:    OneWire_SendBit()
// Return:      void
// Arg:         short data  -   data bit
//----------------------------------------------------------------------------//
void OneWire_SendBit( short data )
{
    ONE_IO = 0;
    ONE_DATA = 0;
    __delay_us(TIME_IDLE);
    ONE_DATA = data;
    __delay_us(TIME_SLOT);
    /*ONE_IO = 0;
    ONE_DATA = 1;
    __delay_us(TIME_IDLE);*/
    ONE_IO = 1;
}

//----------------------------------------------------------------------------//
// Function:    OneWire_Send8()
// Return:      void
// Arg:         unsigned char data  -   data byte
//----------------------------------------------------------------------------//
int OneWire_Send8(uint8_t data)
{
    int res = 0;
    for( int i = 0; i < 8; i++ )
    {
        if( ((data>>(i))&0x01) == 1 )
        {
            OneWire_SendBit(1);
        }
        else OneWire_SendBit(0);
    }
    return res;
}

//----------------------------------------------------------------------------//
// Function:    OneWire_Send16()
// Return:      void
// Arg:         unsigned char cmd   -   command value
//              unsigned char addr  -   memory address
//----------------------------------------------------------------------------//
void OneWire_Send16( unsigned char cmd, unsigned char addr )
{
    for( int i = 0; i < 8; i++ )
    {
        if( cmd>>i&0x01 == 1 )
        {
            OneWire_SendBit(1);
        }
        else OneWire_SendBit(0);
    }
    for( int i = 0; i < 8; i++ )
    {
        if( addr>>i&0x01 == 1 )
        {
            OneWire_SendBit(1);
        }
        else OneWire_SendBit(0);
    }
}

//----------------------------------------------------------------------------//
// Function:    OneWire_SendX()
// Return:      void
// Arg:         unsigned char length   -   count of bit
//              unsigned char data     -   data bit's
//----------------------------------------------------------------------------//
void OneWire_SendX( unsigned char length, unsigned char data )
{
    for( int i = 0; i < length; i++ )
    {
        if( data>>i&0x01 == 1 )
        {
            OneWire_SendBit(1);
        }
        else OneWire_SendBit(0);
    }
}

//----------------------------------------------------------------------------//
// Function:    OneWire_ReceiveBit()
// Return:      unsigned char:  data bit
// Arg:         void
//----------------------------------------------------------------------------//
uint8_t OneWire_ReceiveBit()
{
    uint8_t data = 0;
    ONE_IO = 0;
    ONE_DATA = 0;
    __delay_us(TIME_IDLE);

    ONE_DATA = 1;
    ONE_IO = 1;
    __delay_us(TIME_RECIVE);
    data = (uint8_t)ONE_DATA;
    
    __delay_us(TIME_SLOT-TIME_RECIVE);
    /*ONE_IO = 0;
    ONE_DATA = 1;
    __delay_us(TIME_IDLE);*/
    return data;
}

#ifdef SEARCH_ROM_ENABLE
//----------------------------------------------------------------------------//
// Function:    OneWire_Receive2()
// Return:      unsigned char:  data 2 bit's
// Arg:         void
//----------------------------------------------------------------------------//
unsigned char OneWire_Receive2()
{
    unsigned char data = 0;
    for( int i = 0; i < 2; i++ )
    {
        data |= (OneWire_ReceiveBit()<<i);
    }
    return data;
}
#endif

//----------------------------------------------------------------------------//
// Function:    OneWire_ReceiveBit()
// Return:      unsigned char:  data byte
// Arg:         void
//----------------------------------------------------------------------------//
unsigned char OneWire_Receive8()
{
    uint8_t data = 0;
    for( uint8_t i = 0; i < 8; i++ )
    {
        data |= (uint8_t)(OneWire_ReceiveBit() << i);
    }
    return data;
}

//----------------------------------------------------------------------------//
// Function:    OneWire_ReceiveBit()
// Return:      int:    1  -  successful
//                      0  -  unsuccessful
// Arg:         int input1  -   LSB 4 byte's
//              int input2  -   MSB 4 byte's
//----------------------------------------------------------------------------//
int OneWire_CRC8( long input1, long input2)
{
    int crc = 0;
    int crc_slave = 0;
    int xor = 0;
    crc_slave = (input2)&0xFF;
    input2 = (input2>>8)&0xFFFFFF;

    int b = 0;

    for( int i = 0; i < 4; i++ )
    {
        b = ((input1>>(8*(3-i))))&0xFF;
        for ( int p = 8; p; p--)
        {
            xor = b^crc;
            xor &= 1;
            crc >>= 1;
            b >>= 1;
            if( xor == 1 )
                crc ^= 0x8c;
        }
    }
    for( int i = 0; i < 3; i++ )
    {
         b = ((input2>>(8*(2-i))))&0xFF;
        for ( int p = 8; p; p--)
        {
            xor = b^crc;
            xor &= 1;
            crc >>= 1;
            b >>= 1;
            if( xor == 1 )
                crc ^= 0x8c;
        }
    }

    if( crc == crc_slave )
    {
        return 1;
    }
    return 0;
}

//----------------------------------------------------------------------------//
// Function:    OneWire_ReceiveBit()
// Return:      int:    1  -  successful
//                      0  -  unsuccessful
// Arg:         void
//----------------------------------------------------------------------------//
int OneWire_ReadNetAddress()
{
   /* OneWire_Reset();
    if(!OneWire_Presence())
    {
        return 0;
    }*/
    OneWire_Init();
    OneWire_Send8( READ_ROM );
    for( int i = 0; i < 4; i++ )
    {
        addr_msb = (addr_msb<<8) + OneWire_Receive8();
    }
    for( int i = 0; i < 4; i++ )
    {
        addr_lsb = (addr_lsb<<8) + OneWire_Receive8();
    }
    return OneWire_CRC8(addr_msb, addr_lsb);
}

//----------------------------------------------------------------------------//
// Function:    OneWire_ReceiveBit()
// Return:      unsigned char   1  -  successful
//                              0  -  unsuccessful
// Arg:         void
//----------------------------------------------------------------------------//
unsigned char OneWire_SkipNetAddress()
{
   /* OneWire_Reset();
    if(!OneWire_Presence())
    {
        return 0;
    }*/
    OneWire_Init();
    OneWire_Send8( SKIP_ROM );
    return 1;
}

//----------------------------------------------------------------------------//
// Function:    OneWire_ReceiveBit()
// Return:      unsigned char   3  -  64 devices found
//                              2  -  device not found
//                              1  -  successful
//                              0  -  unsuccessful
// Arg:         void
//----------------------------------------------------------------------------//
#ifdef SEARCH_ROM_ENABLE
unsigned char OneWire_SearchNetAddress()
{
    int id_bit = 0;
    int cmp_id_bit = 0;
    int id_bit_number = 0;
    int LastDeviceFlag = 0;
    int last_zero = 0;
    int LastDiscrepancy = 0;
    int LastFamilyDiscrepancy = 0;
    int Search_direction = 0;
    int ROM_NO[64];
    int input1 = 0;
    int input2 = 0;
    int count_addr = 0;

    OneWire_Reset();
    if(!OneWire_Presence())
    {
        return 0;
    }

    while( LastDeviceFlag == 0 )
    {
        id_bit_number = 1;
        last_zero = 0;
        OneWire_Send8( SEARCH_ROM );
        for( int i = 0; i < 64; i++ )
        {
            id_bit = OneWire_Receive2();
            switch( id_bit )
            {
                case 0: id_bit = 0; cmp_id_bit = 0; break;
                case 1: id_bit = 0; cmp_id_bit = 1; break;
                case 2: id_bit = 1; cmp_id_bit = 0; break;
                case 3: id_bit = 1; cmp_id_bit = 1; break;
            }

            if( id_bit == 1 && cmp_id_bit == 1 )
            {
                LastDeviceFlag = 0;
                LastDiscrepancy = 0;
                LastFamilyDiscrepancy = 0;
                return 2;
            }
            if( id_bit == 0 && cmp_id_bit == 0 )
            {
                if( id_bit_number == LastDiscrepancy )
                {
                    Search_direction = 1;
                }
                else if( id_bit_number > LastDiscrepancy )
                {
                    Search_direction = 0;
                    last_zero = id_bit_number;
                    if( last_zero < 9 )
                    {
                        LastFamilyDiscrepancy = last_zero;
                    }
                }
                else
                {
                    Search_direction = ROM_NO[id_bit_number];
                    if( Search_direction == 0 )
                    {
                        last_zero = id_bit_number;
                        if( last_zero < 9 )
                        {
                            LastFamilyDiscrepancy = last_zero;
                        }
                    }
                }
            }
            else
            {
                Search_direction = id_bit;
            }
            ROM_NO[id_bit_number] = Search_direction;
            id_bit_number++;
            if( id_bit_number > 64 )
            {
                LastDiscrepancy = last_zero;
                if( LastDiscrepancy == 0 )
                {
                    LastDeviceFlag = 1;
                }
                if( LastDiscrepancy == LastFamilyDiscrepancy )
                {
                    LastFamilyDiscrepancy = 0;
                }
                for( int i = 0; i < 32; i++ )
                {
                    input1= input1<<1;
                    input1 |= ROM_NO[i];
                    input2= input2<<1;
                    input2 |= ROM_NO[32+i];
                }
                if( OneWire_CRC8( input1, input2) == 1 )
                {
                    net_addr[count_addr] = ROM_NO;
                    count_addr++;
                    if( count_addr == 64 )
                    {
                        return 3;
                    }
                }
                else
                {
                    LastDiscrepancy = 0;
                    LastFamilyDiscrepancy = 0;
                    LastDeviceFlag = 0;
                    return 0;
                }
            }
        }
    }
}
#endif

//----------------------------------------------------------------------------//
// Function:    OneWire_MatchNetAddress()
// Return:      void
// Arg:         void
//----------------------------------------------------------------------------//
int OneWire_MatchNetAddress()
{
   /* OneWire_Reset();
    if(!OneWire_Presence())
    {
        return 0;
    }*/
    OneWire_Init();
    OneWire_Send8( MATCH_ROM );
    for( uint8_t i = 0; i < 4; i++ )
    {
        OneWire_Send8( (uint8_t)((addr_msb >> ((3-i) * 8)) & 0xFF) );
    }
    for( int i = 0; i < 4; i++ )
    {
        OneWire_Send8( (uint8_t)((addr_lsb >> ((3-i) * 8)) & 0xFF) );
    }
    return 1;
}