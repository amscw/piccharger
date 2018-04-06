/* 
 * File:   OneWire.h
 * Author: a.v.chebryakov
 *
 * Created on 3 7 2014, 14:10
 */


#ifndef WIRE_H
#define	WIRE_H

#include <p18cxxx.h>

//#define SEARCH_ROM_ENABLE

//--- Pins ---//
#define ONE_DATA        LATB7 // RB7
#define ONE_IO          TRISB7

//--- Constants ---//
#define SEARCH_ROM      0xF0
#define READ_ROM        0x33
#define MATCH_ROM       0x55
#define SKIP_ROM        0xCC
#define ALARM_STATE     0xEC
#define RESUME          0xA5
#define READ            0x69
#define WRITE           0x6C
#define COPY            0x48
#define RECALL          0xB8
#define LOCK            0x6A

#define TIME_SLOT       90
#define TIME_RESET      640
#define TIME_RECIVE     8
#define TIME_IDLE       5

//--- Variables ---//
long addr_lsb            = 0;
long addr_msb            = 0;

#ifdef SEARCH_ROM_ENABLE
int net_addr[64]        = 0;
#endif

//--- Functions ---//
int                     OneWire_Init();
void                    OneWire_Reset();
unsigned char           OneWire_Presence();
void                    OneWire_SendBit( short data );
int                     OneWire_Send8( unsigned char data );
void                    OneWire_Send16( unsigned char cmd, unsigned char addr );
void                    OneWire_SendX( unsigned char length, unsigned char data );
unsigned char           OneWire_ReceiveBit();
unsigned char           OneWire_Receive8();
int                     OneWire_CRC8( long input1, long input2);
unsigned char           OneWire_SkipNetAddress();
int                     OneWire_ReadNetAddress();
int                     OneWire_MatchNetAddress();

#ifdef SEARCH_ROM_ENABLE
unsigned char           OneWire_Receive2();
unsigned char           OneWire_SearchNetAddress();
#endif

#endif	/* WIRE_H */