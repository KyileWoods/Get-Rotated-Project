#include <stdbool.h>
#include <ti/drivers/GPIO.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include "Board.h"
#include "OGcreations/OurMotorFuncs/OurMotors.h"
#include "motorlib.h"

// ISR prototypes
void HallSensorA_isr(void);
void HallSensorB_isr(void);
void HallSensorC_isr(void);

void ConfigureMotorPins(void){

/*
This is the conversion from motor-driver board, into the port-pin of launchpad

- INHA -> Port F, Pin 1
- INLA -> Port F, Pin 2
- INHB -> Port F, Pin 3
- INLB -> Port G, Pin 0
- INHC -> Port L, Pin 4
- INLC -> Port L, Pin 5
- HALLA -> Port M, Pin 3
- HALLB -> Port H, Pin 2
- HALLC -> Port H, Pin 3
- HALLD -> Port N, Pin 2
*/

    // Enable the peripherals as GPIO
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    // Wait for the peripherals to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOG));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOH));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));

    // Configure INHA, INLA, INHB, INLB, INHC, INLC as PWM outputs
    GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeTimer(GPIO_PORTG_BASE, GPIO_PIN_0);
    GPIOPinTypeTimer(GPIO_PORTL_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    // Configure Hall Sensors as input...
    GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_3);
    GPIOPinTypeGPIOInput(GPIO_PORTH_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOInput(GPIO_PORTH_BASE, GPIO_PIN_3);
    GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, GPIO_PIN_2);
    //                           ....... with interrupt on both edges
    GPIOIntTypeSet(GPIO_PORTM_BASE, GPIO_PIN_3, GPIO_BOTH_EDGES);
    GPIOIntTypeSet(GPIO_PORTH_BASE, GPIO_PIN_2, GPIO_BOTH_EDGES);
    GPIOIntTypeSet(GPIO_PORTH_BASE, GPIO_PIN_3, GPIO_BOTH_EDGES);
    GPIOIntTypeSet(GPIO_PORTN_BASE, GPIO_PIN_2, GPIO_BOTH_EDGES);
    
    // Register ISRs
    GPIOIntRegister(GPIO_PORTM_BASE, HallSensorA_isr);
    GPIOIntRegister(GPIO_PORTH_BASE, HallSensorB_isr);
    GPIOIntRegister(GPIO_PORTN_BASE, HallSensorC_isr);

    // Enable interrupts
    GPIOIntEnable(GPIO_PORTM_BASE, GPIO_INT_PIN_3);
    GPIOIntEnable(GPIO_PORTH_BASE, GPIO_INT_PIN_2 | GPIO_INT_PIN_3);
    GPIOIntEnable(GPIO_PORTN_BASE, GPIO_INT_PIN_2);
}

void ConfigureMotors(void) {

    ConfigureMotorPins(); //Set pWM, GPIO, input/output

    //IS THERE EVEN ANYTHING ELSE TO BE DONE?!?
    
}

void kill_motors(bool be_gentle){
    
    stopMotor(be_gentle);


}

void HallSensorA_isr(void)
{
    /*
    1) Read all three (oR ONE?) Hall sensors, dump it into a register or structure
    2) Flag a Hall-read event
        2b) We could have this ISR set a specific task-thread to READY,
            if we design that task to switch itself to WAITING, without
            actually asking it to wait upon anything
    3) Increment the speed counter
    4) updateMotor();

    Some of these can be moved out of this function, but this gives a general outline/flow
    */

    // Read the GPIO line of the halls
    bool Hall_a = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1);
    bool Hall_b = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3);
    bool Hall_c = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_4);
    
    // Communicate that with other Tasks
    System_printf("Sensor A:  %i%i%i \n", Hall_a,Hall_b,Hall_c);
    System_flush();

    // Post the relevant event OR set a specific task to READY// 
    // DO THAT HERE//

    // Increment the speed counter. Not actually sure what that means
    uint16_t PowerPercentage = 20;
    setDuty(getMotorPWMPeriod()*PowerPercentage/100);//Move this out into a copmutation-intensive task, or SWI

    //New motor position registered, means new motor position demanded.
    updateMotor(Hall_a, Hall_b, Hall_c);


    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_1);//MUST CLEAR THE INTERRUPT
}

void HallSensorB_isr(void)
{
    
    //System_printf("Sensor B:  %i%i%i \n", 1,2,3);
    //System_flush();



    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_3);//MUST CLEAR THE INTERRUPT
}

void HallSensorC_isr(void)
{

    
    //System_printf("Sensor C:  %i%i%i \n", 1,2,3);
    //System_flush();


    GPIOIntClear(GPIO_PORTL_BASE, GPIO_INT_PIN_4); //MUST CLEAR THE INTERRUPT
}




