#include "charger.h"
#include "sw_i2c.h"

// I2C macro
enum op_t { WRITE, READ };

#define CHARGER_ADDR        (0x9 << 1)

// code hacks...
#define CHECK_FAIL(foo) { err_t err = foo; if (err != ERR_OK) return err; }

// globals
// TODO: need comment
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

chargerStatus_t g_ChargerStatus;

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

err_t ChargerStatus(chargerStatus_t *pStatus)
{
    if (!pStatus)
        return ERR_NULLPTR;
    
    CHECK_FAIL(writeCommand(CHARGER_STATUS));
    CHECK_FAIL(readWord(&pStatus->word));
    
    return ERR_OK;
}

err_t ChargerMode(chargerMode_t chargerMode)
{
    CHECK_FAIL(writeCommand(CHARGER_MODE));
    CHECK_FAIL(writeWord(chargerMode.word));
    
    return ERR_OK;
}

err_t ChargerReset()
{
    mode.bits.POR_RESET = 1;
    mode.bits.INHIBIT_CHARGE = 1;
    CHECK_FAIL(ChargerMode(mode));
//    __delay_ms(100);
    mode.bits.INHIBIT_CHARGE = 0;
    CHECK_FAIL(ChargerMode(mode));
    
    return ERR_OK;
}

err_t ChargingCurrent(uint16_t I0)
{
    CHECK_FAIL(writeCommand(CHARGING_CURRENT));
    CHECK_FAIL(writeWord(I0));
    
    return ERR_OK;
}

err_t ChargerVoltage(uint16_t V0)
{
    CHECK_FAIL(writeCommand(CHARGER_VOLTAGE));
    CHECK_FAIL(writeWord(V0));
    
    return ERR_OK;
}


