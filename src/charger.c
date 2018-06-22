#include "charger.h"
#include "sw_i2c.h"

//------------------------------------------------------------------------------
// types
//------------------------------------------------------------------------------
enum op_t { WRITE, READ };

//------------------------------------------------------------------------------
// macro
//------------------------------------------------------------------------------
#define CHARGER_ADDR        (0x9 << 1)
#define CHECK_FAIL(foo) { err_t err = foo; if (err != ERR_OK) return err; }
#define MAX_ATTEMPTS    3

//------------------------------------------------------------------------------
// local variables
//------------------------------------------------------------------------------
static chargerMode_t mode = {
    0 /* Allow normal operation */,
    0 /* Not implemented. Write 0 */,
    1 /* Change the voltage and current settings to 0xFFFF and 0x0007; clear THERMISTOR_HOT and ALARM_INHIBITED */,
    0 /* Not implemented. Write 0 */,
    1 /* Not implemented. Write 1 */,
    0 /* Interrupt on either edge of BATTERY_PRESENT status bit */,
    0 /* Interrupt on either edge of POWER_FAIL status bit*/,
    1 /* Not implemented. Write 1 */,
    0x3 /* Not implemented. Write 1 */,
    0 /* THERMISTOR_HOT does not turn the charger off */,
    0x1F /* Not implemented. Write 1 */
};

static chargerStatus_t status;

static struct setPoint_
{
    uint16_t I0;
    uint16_t V0;
} setPoint;

//------------------------------------------------------------------------------
// channel level support
//------------------------------------------------------------------------------

/**
 * implementation of write-command protocol
 * @param cmd 
 * @return status
 */
static err_t writeCommand(chargerCmd_t cmd)
{
    // issue start condition
    SWStartI2C();
    
    // write address + opcode
    if (SWWriteI2C(CHARGER_ADDR | WRITE) < 0) 
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

/**
 * implementation of read-word protocol
 * @param pBuf
 * @return 
 */
static err_t readWord(uint16_t *pBuf)
{
    if (!pBuf)
        return ERR_NULLPTR;
    
    // issue start condition
    SWRestartI2C();
    
    // write address + opcode
    if (SWWriteI2C(CHARGER_ADDR | READ) < 0) 
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

/**
 * implementation of write-word protocol
 * @param buf
 * @return 
 */
static err_t writeWord(uint16_t buf)
{
    // write low data byte
    if (SWWriteI2C((uint8_t)buf)) 
        return ERR_SWI2C_WRITE;
    
    // check ack
    if (SWAckI2C() < 0)
        return ERR_SWI2C_NOACK;
    
    // write high data byte
    if (SWWriteI2C((uint8_t)(buf >> 8))) 
        return ERR_SWI2C_WRITE;
    
    // check ack
    if (SWAckI2C() < 0)
        return ERR_SWI2C_NOACK;
    
    // issue stop condition
    SWStopI2C();
    
    return ERR_OK;
}

//------------------------------------------------------------------------------
// charger-specific level support
//------------------------------------------------------------------------------
static err_t ChargerStatus(chargerStatus_t *pStatus)
{
    if (!pStatus)
        return ERR_NULLPTR;
    
    CHECK_FAIL(writeCommand(CHARGER_STATUS));
    CHECK_FAIL(readWord(&pStatus->word));
    
    return ERR_OK;
}

static err_t ChargerMode(chargerMode_t chargerMode)
{
    CHECK_FAIL(writeCommand(CHARGER_MODE));
    CHECK_FAIL(writeWord(chargerMode.word));
    
    return ERR_OK;
}

static err_t ChargingCurrent(uint16_t I0)
{
    CHECK_FAIL(writeCommand(CHARGING_CURRENT));
    CHECK_FAIL(writeWord(I0));
    
    return ERR_OK;
}

static err_t ChargingVoltage(uint16_t V0)
{
    CHECK_FAIL(writeCommand(CHARGING_VOLTAGE));
    CHECK_FAIL(writeWord(V0));
    
    return ERR_OK;
}

//------------------------------------------------------------------------------
// application level support
//------------------------------------------------------------------------------
/**
 * try to set points V0 and I0
 * @param V0 in mV
 * @param I0 im mA
 * @return status
 */
err_t ChargerConfig(uint16_t V0, uint16_t I0)
{
    err_t err;
    uint8_t attempts = MAX_ATTEMPTS;

    while( ((err = ChargingVoltage(V0)) != ERR_OK) && attempts-- )
        __delay_ms(1);

    if (err == ERR_OK) 
        setPoint.V0 = V0;
    else return err;
    
    attempts = MAX_ATTEMPTS;
    while( ((err = ChargingCurrent(I0)) != ERR_OK) && attempts-- )
        __delay_ms(1);
    
    if (err == ERR_OK)
        setPoint.I0 = I0;
    
    return err;
}

inline uint16_t ChargerGetCurrent() {
    return setPoint.I0;
}

inline uint16_t ChargerGetVoltage() {
    return setPoint.V0;
}

err_t ChargerSetCurrent(uint16_t I0)
{
    err_t err;
    uint8_t attempts = MAX_ATTEMPTS;
    
    while( ((err = ChargingCurrent(I0)) != ERR_OK) && attempts-- )
        __delay_ms(1);
    
    if (err == ERR_OK)
        setPoint.I0 = I0;
    
    return err;
}


/**
 * reset I0 to 7mA, V0 to max; re-switch charger
 * @return status
 */
err_t ChargerReset()
{
    err_t err;
    uint8_t attempts = MAX_ATTEMPTS;
    
    // switch charger OFF
    mode.bits.POR_RESET = 1;
    mode.bits.INHIBIT_CHARGE = 1;
    
    while( ((err = ChargerMode(mode)) != ERR_OK) && attempts-- )
        __delay_ms(1);
    
    if (err != ERR_OK) return err;
    attempts = MAX_ATTEMPTS;
    __delay_ms(1000);   // wait for discharge caps..?
    
    // switch charger ON
    mode.bits.POR_RESET = 0;
    mode.bits.INHIBIT_CHARGE = 0;
    while( ((err = ChargerMode(mode)) != ERR_OK) && attempts-- )
        __delay_ms(1);
    
    return err;
}

/**
 * check change in status. Updates status if changed. 
 * @return ERR_OK if changed, ERR_NO if no change, ERR_XXX if error
 */
inline err_t IsChargerStatusChange()
{
    err_t err;
    uint8_t attempts = 3;
    chargerStatus_t tmp;
    
    while ( ((err = ChargerStatus(&tmp)) != ERR_OK) && attempts--) 
        __delay_ms(1);
    
    if (err == ERR_OK)
    {    
        if (tmp.word != status.word)
        {
            status.word = tmp.word;
            return ERR_TRUE;
        } else return ERR_OK;
    }
    
    return err;
}

inline chargerStatus_t GetChargerStatus()
{
    return status;
}


