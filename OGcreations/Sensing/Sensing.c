//#include <stdio.h>
//#include <stdint.h>
//#include <stdbool.h>
//#include <math.h>
//
//#include "driverlib/adc.h"
//#include "driverlib/gpio.h"
//#include "driverlib/sysctl.h"
//#include "inc/hw_memmap.h"
//#include "driverlib/pin_map.h"
//
//#include <ti/drivers/GPIO.h>
////#include "drivers/motor_current.h"
//
///* HANDELS */
//Swi_Handle          CurrSwi, SpeedSwi;
//
//Clock_Handle        clkSpeed, clkPower;
//Clock_Struct        clkSpeedStruct, clkPowerStruct;
//
//Event_Struct        evtStruct;
//Event_Handle        evtHandle;
//
//
///* ------------------ Setup Sensor Clks ------------------ */
//
//void CurrentFxn(UArg arg0){
//    // Call Software Interupt to get Power from ADCs
//    Swi_post{CurrSwi}
//}
//
//void SpeedFxn(UArg arg0){
//    // Call Software Interupt to get RPM from Halls
//    //swi_post{SpeedSwi}
//}
//
////void AccelerometerFxn(UArg arg0){
////    // Call Software Interupt to get RPM from Halls
////    Event_post{AccelSwi}
////    Event_post(evtHandle, Accel_Event);
////}
////
////void DistanceFxn(UArg arg0){
////    // Call Software Interupt to get RPM from Halls
////    Swi_post{DistSwi}
////    Event_post(evtHandle, Dist_Event);
////}
///* ------------------                   ------------------ */
//
///*
// * To be called before attempting to read from sensors:
// * This function will intialise the ADC GPIO for Trigger input
// */
//
//void initADC (void){
//        SysCtlPeripheralEnable( SYSCTL_PERIPH_ADC0 );
//
//        /* CONFIGURE BOTH ADC GPIOS FOR ANALOG INPUT */
//        //Makes GPIO an INPUT and sets them to be ANALOG
//        SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOE );
//        GPIOPinTypeADC( GPIO_PORTE_BASE, GPIO_PIN_3 );
//
//        SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOE );
//        GPIOPinTypeADC( GPIO_PORTE_BASE, GPIO_PIN_3 );
//
//        /* CONFIGURE BOTH ADC GPIOS FOR ANALOG INPUT */
//        /*
//         * uint32_t ui32Base,
//         * uint32_t ui32SequenceNum,
//         * uint32_t ui32Trigger,
//         * uint32_t,
//         * ui32Priority
//         */
//
//        #define ADC_SEQ 2;
//        #define ADC_STEP 0;
////        ADCSequenceConfigure( ADC0_BASE, ADC_SEQ , ADC_TRIGGER_PROCESSOR, 0 );
////
////        ADCSequenceStepConfigure( ADC0_BASE, ADC_SEQ , 0 , ADC_CTL_IE | ADC_CTL_CH0 | ADC_CTL_END );
////        ADCSequenceStepConfigure( ADC0_BASE, ADC_SEQ , 1 , ADC_CTL_IE | ADC_CTL_CH0 | ADC_CTL_END );
////
////        ADCSequenceEnable( ADC0_BASE, ADC_SEQ );
////        ADCIntClear( ADC0_BASE, ADC_SEQ );
////
//}
//
//
//
//void MainSense{
////        /*------------------------ SWIS --------------------------*/
////
////        Event_construct(&evtStruct, &evtParams); //Construct an event according to params, populate a struct with the event's unique information
////        evtHandle = Event_handle(&evtStruct);// Pull the event handle out from the structure, for easy reference
////
////        /*------------------------ SWIS --------------------------*/
////
////        Swi_Params_init(&swiParams);
////
////        /*Speed swi for converting revolution count to RPM*/
////        SpeedSwi = Swi_create(SpeedBuffer, &swiParams, NULL);
////        if (SpeedSwi == NULL) { System_abort("SWI Failure: HALLS");
////        }
////        /*Power ADC swi*/
////        CurrSwi = Swi_create(PowerBuffer, &swiParams, NULL);
////        if (CurrSwi == NULL) { System_abort("SWI Failure: ADC");
////        }
////
////        /*----------------------- CLOCKS --------------------------*/
////
////        /* Defualt off; Wait until motor start is called */
////        Clock_Params_init(&clkParams);
////        clkParams.startFlag = FALSE;
////
////        /*
////         * Period = (1/Hz) * 1000us per clk
////         */
////
////        /*  Speed Clock (Hall Sensors) - 100hz */
////        clkParams.period = 10;
////        Clock_construct(&clkRPMStruct, (Clock_FuncPtr)rpmFxn, 10, &clkParams);
////        clkSpeed = Clock_handle(&clkSpeedStruct);
////
////        /* Current Clock (DRV8323) - 150hz */
////        clkParams.period = 15;
////        Clock_construct(&clkRPMStruct, (Clock_FuncPtr)rpmFxn, 10, &clkParams);
////        clkPower = Clock_handle(&clkPowerStruct);
//
//}
//
//void ReadADC() {
//
//
//
//}
//
//
//
