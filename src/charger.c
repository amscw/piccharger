#include "charger.h"
#include "plib/SW_I2C/sw_i2c.h"

// I2C macro
#define RD                  1
#define WR                  0
#define CHARGER_ADDR        (0x9 << 1)

// code hacks...
#define CHECK_FAIL(foo) { err_t err = foo; if (err != ERR_OK) return err; }

static err_t writeCommand(chargerCmd_t cmd)
{
    // issue start condition
    SWStartI2C();
    
    // write address + opcode
    if (SWWriteI2C(CHARGER_ADDR | WR) < 0) 
        return ERR_SWI2C_WRITE;
    
    // check ack
    if (SWAckI2C() < 0)
        return ERR_SWI2C_NOACK;
    
    // write command
    if (SWWriteI2C((uint8_t)cmd) < 0)
        return ERR_SWI2C_WRITE;
    
    // check ack
    if (SWAckI2C() < 0)
        return ERR_SWI2C_NOACK;

    return ERR_OK;
}

static err_t readWord(uint16_t *pBuf)
{
    if (!pBuf)
        return ERR_NULLPTR;
    
    // issue start condition
    SWStartI2C();
    
    // write address + opcode
    if (SWWriteI2C(CHARGER_ADDR | RD) < 0) 
        return ERR_SWI2C_WRITE;
    
    // check ack
    if (SWAckI2C() < 0)
        return ERR_SWI2C_NOACK;
    
    // get all bytes
    if ( SWGetsI2C((uint8_t*)pBuf, sizeof *pBuf) < 0 )
        return ERR_SWI2C_READ;
    
    // issue stop condition
    SWStopI2C();
    
    return ERR_OK;
}



err_t ChargerStatus(chargerStatus_t *pStatus)
{
    if (!pStatus)
        return ERR_NULLPTR;
    
    CHECK_FAIL(writeCommand(CHARGER_STATUS));
    CHECK_FAIL(readWord(&pStatus->word));
    
    return ERR_OK;
}

