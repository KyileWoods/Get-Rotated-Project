#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"

#include <ti/drivers/GPIO.h>
//#include "drivers/motor_current.h"

/* ------------------ Setup Sensor Swi for Clocks ------------------ */

void CurrentFxn(UArg arg0){
    // Call Software Interupt to get Power from ADCs
    Swi_post{CurrSwi};
}

void SpeedFxn(UArg arg0){
    // Call Software Interupt to get RPM from Halls
    Swi_post{SpeedSwi};
}

/*Clock Interrupt for RPM @ 100Hz*/
void rpmFxn(UArg arg0){
    Swi_post(rpmSwi);
}

/*Clock Interrupt for current @ 200Hz*/
void currFxn(UArg arg0){
    Swi_post(currSwi);
}

//void AccelerometerFxn(UArg arg0){
//    // Call Software Interupt to get RPM from Halls
//    Event_post{AccelSwi}
//    Event_post(evtHandle, Accel_Event);
//}
//
//void DistanceFxn(UArg arg0){
//    // Call Software Interupt to get RPM from Halls
//    Swi_post{DistSwi}
//    Event_post(evtHandle, Dist_Event);
//}
/* ------------------                   ------------------ */

/*
 * To be called before attempting to read from sensors:
 * This function will intialise the ADC GPIO for Trigger input
 */
void initADC (void){
        int ADC_SEQ = 2;
        int ADC_STEP = 0;

        SysCtlPeripheralEnable( SYSCTL_PERIPH_ADC1 );

        /* CONFIGURE BOTH ADC GPIOS FOR ANALOG INPUT */
        //Make GPIO an INPUT and sets them to be ANALOG
//        SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
//        /* Enable ADC1 */
//        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
//        GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_7);
        /* Enable ADC2 */
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
        GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

        /* Enable ADC Sequence */
        ADCSequenceConfigure(ADC1_BASE, ADC_SEQ, ADC_TRIGGER_PROCESSOR, 0);

        ADCSequenceStepConfigure(ADC1_BASE, ADC_SEQ, 0, ADC_CTL_CH4);
        ADCSequenceStepConfigure(ADC1_BASE, ADC_SEQ, 1, ADC_CTL_IE | ADC_CTL_CH0 | ADC_CTL_END);

        /* Oversampling - for 150Hz */
        ADCHardwareOversampleConfigure(ADC1_BASE, 8);

        ADCSequenceEnable(ADC1_BASE, ADC_SEQ);

        ADCIntEnable(ADC1_BASE, 1);
}


/*
 * Call this within main to ensure proper Sensing API can function properly
 * Essentially contains all main functions for Sensing Subsystem and prevents Main() from becoming too cluttered.
 *
 * */
void MainSense{
        /*------------------------ INIT-EVENTS --------------------------*/

        Event_construct(&evtStruct, &evtParams); //Construct an event according to params, populate a struct with the event's unique information
        evtHandle = Event_handle(&evtStruct);// Pull the event handle out from the structure, for easy reference

        /*------------------------ INIT-SWIS --------------------------*/

        Swi_Params_init(&swiParams);

        /*Speed Swi' for converting revolution count to RPM*/
        SpeedSwi = Swi_create(SpeedBuffer, &swiParams, NULL);
        if (SpeedSwi == NULL) { System_abort("SWI Failure: HALLS");
        }

        /*Power ADC swi*/
        CurrSwi = Swi_create(PowerBuffer, &swiParams, NULL);
        if (CurrSwi == NULL) { System_abort("SWI Failure: ADC");
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
        Clock_construct(&clkRPMStruct, (Clock_FuncPtr)rpmFxn, 10, &clkParams);
        clkSpeed            = Clock_handle(&clkSpeedStruct);

        /* Current Clock (DRV8323) - 150hz */
        int Power_Hz              = 150;          //Define the frequency to Hwi this clock
        clkParams.period    = 1000/Power_Hz;      // 1000 Tick-> 1 second
        Clock_construct(&clkRPMStruct, (Clock_FuncPtr)rpmFxn, 10, &clkParams);
        clkPower            = Clock_handle(&clkPowerStruct);

}

/*
 * Call to read ADC values & return
 * - Function updates ADC values for the motor
 * - Will also Buffer and filter ADC current readings.
 * */
void ReadADC() {
    uint32 CurrentADC;
    // ADC Conversion
    ADCProcessorTrigger(ADC1_BASE, 2);
    while(!ADCIntStatus(ADC1_BASE, 2, false)){}

    //Clear Interrupt
    ADCIntClear( ADC1_BASE, 2);
    // Output Sampled Sequence
    ADCSequenceDataGet(ADC1_BASE, 2, CurrentADC);
    return CurrentADC;
}



