/*
 * Useful links...
 *
 * TI-RTOS Example???: https://github.com/alexeicolin/tirtos/blob/master/packages/examples/i2ceeprom.c
 * TI-RTOS i2C Example???:https://github.com/alexeicolin/tirtos/blob/master/packages/examples/i2ctmp006.c
 *
 * TI-RTOS I2C Example (Best one????): https://github.com/alexeicolin/tirtos/blob/master/packages/examples/i2crf430cl330_load.c
 *
 * https://e2e.ti.com/support/microcontrollers/arm-based-microcontrollers-group/arm-based-microcontrollers/f/arm-based-microcontrollers-forum/450247/how-to-use-ti-rtos-i2c
 */



/*
 *  ======== empty_min.c ========
 */
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

//#include "drivers/opt3001.h"
#include "drivers/vl53lox.h"
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
//            System_printf("I2C write transfer fail!\n");
            return false;
        }
//        System_printf("I2C write transfer successful!\n");
        return true;
}



/*
 * Sets slave address to ui8Addr
 * Writes ui8Reg over i2c to specify register being read from
 * Reads three bytes from i2c slave. The third is redundant but
 * helps to flush the i2c register
 * Stores first two received bytes into *data
 */
bool readI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *data)
{

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
//            System_printf("I2C read transfer fail!\n");
            return false;
        }
//        System_printf("I2C read transfer successful!\n");
        return true;
}


/*
 *  ======== heartBeatFxn ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
Void I2CluxFxn(UArg arg0, UArg arg1)
{
//    while (1) {
//        Task_sleep((unsigned int)arg0);
//        GPIO_toggle(Board_LED0);
//        System_flush();
//    }

    bool      worked, success;
    uint16_t  rawData = 0;
    float     convertedLux = 0;
    char      tempStr[40];

    // Test that sensor is set up correctly
    System_printf("Testing VL53L0X ToF Sensor:\n");
    worked = sensorVL53L0XTest();

    while (!worked) {
//        SysCtlDelay(g_ui32SysClock);
        Task_sleep(100);
        System_printf("\nTest Failed, Trying again\n");
        worked = sensorVL53L0XTest();
    }

    System_printf("All Tests Passed!\n\n");

    // Initialize VL53L0X sensor
    sensorVL53L0XInit();
    sensorVL53L0XEnable(true);

    // Loop Forever
    while(1)
    {
//        SysCtlDelay(g_ui32SysClock/100);
        Task_sleep(100);

        //Read and convert OPT values
        success = sensorVL53L0XRead(&rawData);

        if (success) {
            sensorVL53L0XConvert(rawData, &convertedLux);

            // Construct Text
            sprintf(tempStr, "Lux: %5.2f\n", convertedLux);
            System_printf("%s\n", tempStr);
        }
        System_flush();
    }
}

/*
 *  ======== main ========
 */
int main(void)
{
    Task_Params taskParams;

    /* Call board init functions */
    Board_initGeneral();
    // Board_initEMAC();
    Board_initGPIO();
    Board_initI2C();

    // Board_initSDSPI();
    // Board_initSPI();
    // Board_initUART();
    // Board_initUSB(Board_USBDEVICE);
    // Board_initUSBMSCHFatFs();
    // Board_initWatchdog();
    // Board_initWiFi();

    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
    taskParams.arg0 = 1000;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)I2CluxFxn, &taskParams, NULL);

    System_printf("I2C task thread created successfully!");

    /* Turn on user LED  */
    GPIO_write(Board_LED0, Board_LED_ON);

//    i2c = i2c_open();

//    // Initialise and enable the I2C peripherial
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
//
//    // Configure the pin muxing for I2C0 functions on port B2 and B3
//    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
//    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
//
//    // Select the I2C function for these pins
//    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
//    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
//    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);


    /* Create and Open I2C port*/
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(Board_I2C_OPT3001, &i2cParams);  // Need to define Board_I2C_VL53L0X

    if (i2c == NULL) {
    System_abort("Error Initializing I2C\n");
    }
    if (i2c != NULL) {
    System_printf("I2C Initialized!\n");
    }

    /* Start BIOS */
    BIOS_start();

    return (0);
}
