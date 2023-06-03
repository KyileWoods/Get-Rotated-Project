/**************************************************************************************************
*  Filename:       i2cOptDriver.c
*  By:             Jesse Haviland
*  Created:        1 February 2019
*  Revised:        23 March 2019
*  Revision:       2.0
*
*  Description:    i2c Driver for use with opt3001.c and the TI OP3001 Optical Sensor
*************************************************************************************************/



// ----------------------- Includes -----------------------
#include "i2cOptDriver.h"
#include "inc/hw_memmap.h"
#include "driverlib/i2c.h"
#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"
#include <ti/drivers/I2C.h>
#include <xdc/std.h>
#include <ti/sysbios/knl/Task.h>



/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
// #include <ti/drivers/EMAC.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
// #include <ti/drivers/SDSPI.h>
// #include <ti/drivers/SPI.h>
// #include <ti/drivers/UART.h>
// #include <ti/drivers/USBMSCHFatFs.h>
// #include <ti/drivers/Watchdog.h>
// #include <ti/drivers/WiFi.h>

/* Board Header file */
#include "Board.h"

#include "drivers/opt3001.h"
#include <stdio.h>


#define TASKSTACKSIZE   1024

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

I2C_Transaction             i2cTransaction;
uint8_t                     writeBuffer[3];
uint8_t                     readBuffer[2];
I2C_Handle                  i2c;
bool                        transferOK;
I2C_Params i2cParams;

/*
 * Sets slave address to ui8Addr
 * Puts ui8Reg followed by two data bytes in *data and transfers
 * over i2c
 */
bool writeI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *data)
{
    // add to writeBuffer

    // write buffer is an array so it needs to be indexed at the right location
    writeBuffer[0] = ui8Reg;
    writeBuffer[1] = data[0];
    writeBuffer[2] = data[1];

    // send data
    i2cTransaction.slaveAddress = ui8Addr; /* 7-bit peripheral slave address */
    i2cTransaction.writeBuf = writeBuffer; /* Buffer to be written */
    i2cTransaction.writeCount = 3; /* Number of bytes to be written */
    i2cTransaction.readBuf = readBuffer; /* Buffer to be read */
    i2cTransaction.readCount = 0; /* Number of bytes to be read */
    transferOK = I2C_transfer(i2c, &i2cTransaction); /* Perform I2C transfer */

    data[0] = readBuffer[0];
    data[1] = readBuffer[1];

    if (!transferOK) {
        //System_printf("I2C write transfer fail!\n");
        return false;
    }
    else
    {
        //System_printf("I2C write transfer successful!\n");
        return true;
    }
}



/*
 * Sets slave address to ui8Addr
 * Writes ui8Reg over i2c to specify register being read from
 * Reads three bytes from i2c slave. The third is redundant but
 * helps to flush the i2c register
 * Stores first two received bytes into *data
 */
bool readI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *data) {
    // add to writeBuffer

      // write buffer is an array so it needs to be indexed at the right location
       writeBuffer[0] = ui8Reg;

       i2cTransaction.slaveAddress = ui8Addr; /* 7-bit peripheral slave address */
       i2cTransaction.writeBuf = writeBuffer; /* Buffer to be written */
       i2cTransaction.writeCount = 1; /* Number of bytes to be written */
       i2cTransaction.readBuf = readBuffer; /* Buffer to be read */
       i2cTransaction.readCount = 2; /* Number of bytes to be read */
       transferOK = I2C_transfer(i2c, &i2cTransaction); /* Perform I2C transfer */

       data[0] = readBuffer[0];
       data[1] = readBuffer[1];

       if (!transferOK) {
            //System_printf("I2C read transfer fail!\n");
           return false;
       }
       else
       {
           //System_printf("I2C read transfer successful!\n");
           return true;
       }
}



