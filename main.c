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
 *  ======== main.c ========
 */
/* Standard type of includes */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Queue.h>


/* OG ROTATED CREATIONS */
#include "OGcreations/OurEventsFuncs/OurEvents.h"
#include "OGcreations/OurMessagingFuncs/OurMessaging.h"
#include "OGcreations/OurTimingFuncs/OurTimers.h"
//#include "OGcreations/HeartBeatFuncs/heartbeatfuncs.h"

//#include "OGcreations/HeartBeatFuncs/heartbeatfuncs.h"


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
/* QUT driver files*/
#include "drivers/opt3001.h"
#include "drivers/i2cOptDriver.h"
#include "drivers/motorlib.h"


/* Task Definitions  */
#define TASKSTACKSIZE   512
Task_Struct MotorTask_Struct;
Char MotorTask_Stack[TASKSTACKSIZE];

Task_Struct task1Struct;
Char task1Stack[TASKSTACKSIZE];
Task_Struct task2Struct;
Char task2Stack[TASKSTACKSIZE];
#define STACKSIZE 1024
Char taskStack[STACKSIZE];

/* Events */
Event_Struct evtStruct;
Event_Handle evtHandle;

typedef enum {
    IDLING,
    STARTING,
    RUNNING,
    ZOOMING,
    SLOWING,
    E_STOP,
} MotorState;





/*
 *  ======== heartBeatFxn ========
 *  Toggle the Board_LED0/1. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */


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
    /* Params */
    Task_Params taskParams;
    Event_Params eventParams;
    Clock_Params clkParams;
    Mailbox_Params mbxParams;

    Board_initGeneral();

    Task_Params_init(&taskParams);
        taskParams.arg0 = (UArg)mbxHandle;
        taskParams.stackSize = TASKSTACKSIZE;
        taskParams.priority = 1;
        // Create the 2 events
        taskParams.stack = &MotorTask_Stack;
        Task_construct(&MotorTask_Struct, (Task_FuncPtr)writertask, &taskParams, NULL);
        taskParams.stack = &task1Stack;
        Task_construct(&task1Struct, (Task_FuncPtr)readertask, &taskParams, NULL);



    Clock_Params_init(&clkParams);
    clkParams.startFlag = TRUE;
    int Hz = 150; //Define the frequency to Hwi this clock
    clkParams.period = 1000/Hz; //100ms => 10 Hz
    Clock_construct(&clk0Struct, (Clock_FuncPtr)clk0_swi_clk_fxn,
                    50, &clkParams);
    clk0Handle = Clock_handle(&clk0Struct);

    // Initialize I2C peripheral
    i2c_init();
    //initialise the motor library
    /* ANY CONFUSING CODE BELOW HERE IS LIKELY TO BE DEBUG PURPOSES, FOR A SPECIFIC PROBLEM. DELTE JUDICIOUSLY

    This function requires an *eb errorbreak but it is breaking at this point.
    Probably an error is being thrown, but with NULL in EB it is being thrown to 0xffffff and crashing*/

    setDuty(50); //Debug only


    int enter = 0; //This is an if-guard which shouldn't exist, except this stupid error.
    if(enter){
        bool MotorLibSuccess ;
        Error_Block m_eb;

        Error_init(&m_eb);
        
        //enableMotor();
        MotorLibSuccess = initMotorLib(50, &m_eb);
        if (!MotorLibSuccess) {
            Error_print(&m_eb);
            System_abort("Error Initializing Motor\n");
        }
        //setDuty(50);
        //initMotorLib(50); 
    }
    setDuty(50);

    // Initialize the task parameters
    Task_Params_init(&taskParams);
    taskParams.stackSize = STACKSIZE;
    taskParams.stack = &taskStack;
    taskParams.priority = 1;

    // Create the opt3001ReadFxn task
    //Task_Handle opt3001TaskHandle = Task_create((Task_FuncPtr)opt3001ReadFxn, &taskParams, NULL);

    
    Event_Params evtParams; //Declare params locally
    Event_Params_init(&evtParams); //Initialise it with default values
    //Event_create is DYNAMIC; even_construct is STATIC memory allocation-- PROTOTYPE: Event_Handle Event_create(Event_Params *__prms, xdc_runtime_Error_Block *__eb );
    Event_construct(&evtStruct, &evtParams); //Construct an event according to params, populate a struct with the event's unique information
    evtHandle = Event_handle(&evtStruct);// Pull the event handle out from the structure, for easy reference
    if (evtHandle == NULL) {
        //System_printf("Event creation failed :( ");
    }

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
    //sensorOpt3001Enable(true);

    /* Construct heartBeat0 Task  thread */
    Task_Params_init(&taskParams);
    taskParams.arg0 = 300;
    taskParams.arg1 = 0;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &MotorTask_Stack;
    //Task_construct(&task0Struct, (Task_FuncPtr)heartBeatFxn, &taskParams, NULL);

    /* Construct heartBeat1 Task  thread */
    Task_Params_init(&taskParams);
    taskParams.arg0 = 100;
    taskParams.arg1 = 1;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task1Stack;
    //Task_construct(&task1Struct, (Task_FuncPtr)heartBeatFxn, &taskParams, NULL);

    /* Start BIOS */
    BIOS_start();

    return (0);
}
