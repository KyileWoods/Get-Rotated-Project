// Shoopy doo
/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

#include "drivers/opt3001.h"
#include "drivers/i2cOptDriver.h"
#include <stdio.h>

#define TASKSTACKSIZE   512
Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];
Task_Struct task1Struct;
Char task1Stack[TASKSTACKSIZE];
Task_Struct task2Struct;
Char task2Stack[TASKSTACKSIZE];
#define STACKSIZE 1024
Char taskStack[STACKSIZE];


/*
 *  ======== heartBeatFxn ========
 *  Toggle the Board_LED0/1. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
Void heartBeatFxn(UArg limit, UArg LED_inc)
{
    /*A function which can be entered into by multiple threads, and light up unique LED's.
    The maximal-allowed time interval and LED# are passed as arg's, then the blinking
    rate oscillates between zero and 'limit'
    */
    int direction = 1; //Define whether the blinking interval grows or shrinks, at the beginning
    int ms_time = 1 + (LED_inc*limit);
    while (1) {
        Task_sleep((unsigned int)ms_time);
        GPIO_toggle(Board_LED0+LED_inc);
        if(direction == 1){
            ms_time++;
            if(ms_time>limit){
                direction = -1;
            }
        }
        if(direction == -1){
            ms_time--;
            if(ms_time<2){
                direction = 1;
            }
        }

    }
}

void i2c_init(void) {
    // Enable I2C peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Configure I2C pins
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // Initialize and configure I2C master
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
}

Void opt3001ReadFxn(UArg arg0, UArg arg1) {
    uint16_t rawData;
    float lux;

    // Initialize OPT3001 sensor
    sensorOpt3001Init();

    // Enable the OPT3001 sensor
    sensorOpt3001Enable(true);

    while (1) {
        // Read raw data from the OPT3001 sensor
        if (sensorOpt3001Read(&rawData)) {
            // Convert raw data to Lux
            sensorOpt3001Convert(rawData, &lux);

            // Print the Lux value or perform any other required operation
            // For example: UARTprintf("Lux: %f\n", lux);
        } else {
            // Handle the error if necessary, e.g., log an error message
        }

        // Add a delay to control the frequency of sensor readings
        // Replace DELAY_VALUE with an appropriate number of system ticks
        Task_sleep(100);
    }
}

/*
 *  ======== main ========
 */
int main(void)
{
    Task_Params taskParams;

    // Initialize I2C peripheral
    i2c_init();

    // Initialize the task parameters
    Task_Params_init(&taskParams);
    taskParams.stackSize = STACKSIZE;
    taskParams.stack = &taskStack;
    taskParams.priority = 1;

    // Create the opt3001ReadFxn task
    Task_Handle opt3001TaskHandle = Task_create((Task_FuncPtr)opt3001ReadFxn, &taskParams, NULL);

    /* Call board init functions */
    Board_initGeneral();
    // Board_initEMAC();
    Board_initGPIO();
    Board_initI2C();

    sensorOpt3001Init();  // Initialize OPT3001 sensor
    // Board_initSDSPI();
    // Board_initSPI();
    // Board_initUART();
    // Board_initUSB(Board_USBDEVICE);
    // Board_initUSBMSCHFatFs();
    // Board_initWatchdog();
    // Board_initWiFi();

    /*
     * Creating two threads to operate on the same thread, with some experimental parameters.
     * At the moment: giving different parameters, and different stack.
     * The arg1 is the LED number (LED0/1).
     * */
    sensorOpt3001Enable(true);



    /* Construct heartBeat0 Task  thread */
    Task_Params_init(&taskParams);
    taskParams.arg0 = 300;
    taskParams.arg1 = 0;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)heartBeatFxn, &taskParams, NULL);

    /* Construct heartBeat1 Task  thread */
    Task_Params_init(&taskParams);
    taskParams.arg0 = 300;
    taskParams.arg1 = 1;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task1Stack;
    Task_construct(&task1Struct, (Task_FuncPtr)heartBeatFxn, &taskParams, NULL);

    /* Start BIOS */
    BIOS_start();

    return (0);
}
