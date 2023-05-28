/*
 *  Filename:    Sensing.c
 *
 *  Description: Source file for My Module.
 *
 *  Created on:  [May 2023]
 *  Author:      [Phoenix Seybold]
 */

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
#include "sensing.h"

//#include "drivers/motor_current.h"

/* HANDLES */
Swi_Handle          PowerSwi, SpeedSwi;

Clock_Handle        clkSpeed, clkPower;
Clock_Struct        clkSpeedStruct, clkPowerStruct;

Event_Struct        evtStruct;
Event_Handle        evtHandle;


/* ------------------ Setup Sensor Swi for Clocks ------------------ */

/*Clock Interrupt for RPM @ 100Hz*/
void rpmFxn(UArg arg0){
    // Call Software Interrupt to get RPM from Halls
    Swi_post(SpeedSwi);
}

/*Clock Interrupt for current @ 200Hz*/
void currFxn(UArg arg0){
    // Call Software Interrupt to get Power from ADCs
    Swi_post(PowerSwi);
}

//void DistanceFxn(UArg arg0){
//    // Call Software Interrupt to get RPM from Halls
//    Swi_post{DistSwi}
//    Event_post(evtHandle, Dist_Event);
//}
/* ------------------                   ------------------ */
/* Function Definitions */


void PowerBuffer(UArg arg0, UArg arg1){

}

void SpeedBuffer(UArg arg0, UArg arg1){

}

/* initADC(void)
    -----------------------------------------------------------
    Description:
    ------------
    Intialise the ADC GPIO for Trigger input
    - Configure and enable the sequencer (sequencer, channel, trigger type, priority)
    - Configure each step
    - Setup ADC1 PE3

    Notes:
    ------
    - ADC 0 used by both touchscreen & Motor; WATCHOUT FOR CONFLICTS
    - microcontroller has 2 ADC modules ADC0 and ADC1.
          They have 20 shared analog channels AIN0-19.
    - ADC is 12 bit with max sampling rate of 2M samples/sec
    - Can transfer data to memory without CPU processing via DMA
*/
void initADC1 (void){
    
        int ADC_SEQ   =  1;
        int ADC_STEP  =  0;

//        SysCtlPeripheralEnable( SYSCTL_PERIPH_ADC0 );
        SysCtlPeripheralEnable( SYSCTL_PERIPH_ADC1 );

        /* CONFIGURE BOTH ADC GPIOS FOR ANALOG INPUT */
//        /* Enable ADC0 */
//        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
//        GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_7);

        /* Enable ADC1 */
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
        GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

        /* Enable ADC Sequence */
        ADCSequenceConfigure(ADC1_BASE, ADC_SEQ, ADC_TRIGGER_PROCESSOR, 0);

                                /*  (Base, Sequence, Step, Config)
                                 *  CONFIG:
                                 *  --------
                                 *  ADC_CTL_IE  = Interrupt enable
                                 *  ADC_CTL_CH0 = Input channel 0
                                 *  ADC_CTL_END = Sequence end select    */
        ADCSequenceStepConfigure(ADC1_BASE, ADC_SEQ, ADC_STEP, ADC_CTL_IE | ADC_CTL_CH0 | ADC_CTL_END);

        /* Oversampling - for 150Hz */
        ADCHardwareOversampleConfigure(ADC1_BASE, 8);

        ADCSequenceEnable(ADC1_BASE, ADC_SEQ);

        ADCIntEnable(ADC1_BASE, 1);
}

/* 
    MainSense(void)
    -----------------------------------------------------------
    Description:
    ------------
    Call this Function inside main to ensure Sensing API is initialised & can function properly
    - Essentially contains all main functions for Sensing Subsystem and prevents Main() from becoming too cluttered.

    Returns: NONE
    --------

    Notes:
    ------
    Any additional information or considerations about the function.
*/
void MainSense(void){
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
        SpeedSwi = Swi_create(SpeedBuffer, &swiParams, NULL);
        if (SpeedSwi == NULL) { System_abort("SWI Failure: HALLS");
        }

        /*Power ADC swi*/
        PowerSwi = Swi_create(PowerBuffer, &swiParams, NULL);
        if (PowerSwi == NULL) { System_abort("SWI Failure: ADC");
        }

        /*----------------------- INIT-CLOCKS --------------------------*/

        /* Default off; Wait until motor start is called */
        Clock_Params_init(&clkParams);
        clkParams.startFlag = FALSE;

        /*
         * Period = (1/Hz) * 1000us per clk
         */

        /*  Speed Clock (Hall Sensors) - 100hz */
        int speed_Hz              = 100;          //Define the frequency to Hwi this clock
        clkParams.period    = 1000/speed_Hz;      // 1000 Tick-> 1 second
        Clock_construct(&clkSpeedStruct, (Clock_FuncPtr)rpmFxn, 10, &clkParams);
        clkSpeed            = Clock_handle(&clkSpeedStruct);

        /* Current Clock (DRV8323) - 150hz */
        int Power_Hz              = 150;          //Define the frequency to Hwi this clock
        clkParams.period    = 1000/Power_Hz;      // 1000 Tick-> 1 second
        Clock_construct(&clkPowerStruct, (Clock_FuncPtr)currFxn, 10, &clkParams);
        clkPower            = Clock_handle(&clkPowerStruct);

}


/* MainSense(void)
    -----------------------------------------------------------
    Description:
    ------------
    Call to read ADC values & return
    - Function updates ADC values for the motor
    - Will also Buffer and filter ADC current readings.

    Returns:
    --------
    CurrentADC : uint32
        (Amps) Filtered motor current as read through ADC

    Notes:
    ------
    - ADC 0 used by both touchscreen & Motor; WATCHOUT FOR CONFLICTS
    - section 8.3.4.1 of DRV232RH driver datasheet (page 41) gives equation of Motor current from voltage
    - Shunt resistor = 0.007 Ohms per current sense line
    - Gain 10; for current Amplifier
*/
uint32_t ReadADC(void) {

    /*  ADC Sequence from Lecture - Another method possible via ISR or Hwi if using TI-RTOS  */
    uint32_t ui32ADC1_CurrentSense[1];
    // Trigger ADC Conversion
    ADCProcessorTrigger(ADC1_BASE, 1);
    // wait for conversion
    while(!ADCIntStatus(ADC1_BASE, 1, false)){ }
    //Clear Interrupt
    ADCIntClear( ADC1_BASE, 2);
    // Read ADC FIFO buffer from sample sequence
    ADCSequenceDataGet(ADC1_BASE, 1, ui32ADC1_CurrentSense);
    return ui32ADC1_CurrentSense[0];
}

/* getCurrent()
    -----------------------------------------------------------
    Description:
    ------------
    getCurrent() will read ADC ( from ReadADC() ) to calculate over shunt resistor after an ISR;
    Motor current from voltage is calculated (in AMPs) using equation specified section 8.3.4.1 DRV232RH Datasheet (p41)

    Returns:
    --------
    CurrentADC : uint32
        (Amps) Filtered motor current as read through ADC

    Notes:
    ------
    - ADC 0 used by both touchscreen & Motor; WATCHOUT FOR CONFLICTS
        Stats Defined in Sensing.h:
        - R_Sense = 0.007       Ohms shunt resistance per current sense line
        - G_csa   = 10          Set Gain for current Amplifier
        - V_vref  = 24V         Motor reference voltage
        - ADC_voltage    = 3.3V   Logic voltage for ADC channel
        - ADC_Resolution = 4096   ADC sampling resolution
*/
//void getCurrent(){
//    float Current;
//    float Power;
//    float avg_shunt;
//    float V_sox;
//
//    V_sox   = ((ADC_voltage * avg_shunt)/ ADC_Resolution);
//    Current = fabs((0.5*V_vref - V_sox) / ( G_csa * R_Sense )) * 1000;
//
//    Power = (V_vref * Current) / 1000;
//}



