/*
 *  Filename:    ACCELEROMETER.c
 *
 *  Description: Source file for My Module.
 *
 *  Created on:  [May 2023]
 *  Author:      [Phoenix Seybold, N9379410]
 */

/* Includes */

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Event.h>

/* Example/Board Header files */
#include "Board.h"

/* Basic Header Files */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* Library */
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"

#include <ti/drivers/GPIO.h>
#include "ACCELEROMETER.h"


/* HANDLES */
Swi_Handle          AccelSwi;

Clock_Handle        clkAccel;
Clock_Struct        clkAccelStruct;

Event_Struct        evtStruct;
Event_Handle        evtHandle;

/* Function implementations */

void AccelerometerFxn(UArg arg0){
    // Call Software Interrupt to get RPM from Halls
    Swi_post(AccelSwi);
//    Event_post(AccelSwi);
//    Event_post(evtHandle, Accel_Event);
}

void AccelFxn(){

}

void AccelBuffer(){

}


int32_t ReadAccelerometer(void)
{
    /* TODO: Initialize My Module */

    return 0; // Success
}

void AccelerometerMain(void){
    /*------------------------ PARAMS-EVENTS --------------------------*/

    Task_Params     taskParams;
    Event_Params    eventParams;
    Clock_Params    clkParams;
    //        Mailbox_Params  mbxParams;
    Swi_Params      swiParams;

    /*------------------------ INIT-EVENTS --------------------------*/

    Event_construct(&evtStruct, &eventParams); //Construct an event according to params, populate a struct with the event's unique information
    evtHandle = Event_handle(&evtStruct);// Pull the event handle out from the structure, for easy reference

    /*------------------------ INIT-SWIS --------------------------*/

    Swi_Params_init(&swiParams);

    /*Speed Swi' for converting revolution count to RPM*/
    AccelSwi = Swi_create(AccelBuffer, &swiParams, NULL);
    if (AccelSwi == NULL) { System_abort("SWI Failure: HALLS");
    }

    /*----------------------- INIT-CLOCKS --------------------------*/

    /* Default off; Wait until motor start is called */
    Clock_Params_init(&clkParams);
    clkParams.startFlag = FALSE;

    /*
     * Period = (1/Hz) * 1000us per clk
     */

    /* ACCELEROMETER Clock (DRV8323) - 200hz */
    int Power_Hz              = 200;          //Define the frequency to Hwi this clock
    clkParams.period    = 1000/Power_Hz;      // 1000 Tick-> 1 second
    Clock_construct(&clkAccelStruct, (Clock_FuncPtr)AccelFxn, 10, &clkParams);
    clkAccel            = Clock_handle(&clkAccelStruct);

    return 0; // Success
}
