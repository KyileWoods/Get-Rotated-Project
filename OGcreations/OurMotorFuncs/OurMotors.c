#include <stdbool.h>
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Task.h> // For Task_exit()
#include <xdc/runtime/System.h> //For printf'ing

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"


//#include "Board.h"
#include "motorlib.h"
#include "OGcreations/OurMotorFuncs/OurMotors.h"
#include "OGcreations/OurMessagingFuncs/OurMessaging.h"
#include "shared_mem.h"
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/gates/GateHwi.h>

extern Queue_Struct HallISR_Queue;
extern Queue_Handle HallISR_QueueHandle;
extern Queue_Params HallISR_QueueParams;
extern bool Hall_a;
extern bool Hall_b;
extern bool Hall_c;

GateHwi_Handle gateHwi;

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
- HALLC -> Port N, Pin 2
- HALLD -> Port H, Pin 3
*/

void ConfigureMotorPins(void){
    /*
    CHRIS has said that the initilization function of motorlib.h already sets up all of the OUTPUT pins,
    We need to set up the HALL pins ourselves.

    Hall setup is being done at line...basically line 277 onwards
    of the EK_TM4C1294XL.c board configuration function
    */


    /* GPIO configurations */
    /* Set GPIOs as outputs */
    /* Enable GPIO interrupts */
    GPIO_enableInt(4);
    GPIO_enableInt(5);
    GPIO_enableInt(6);
    GPIO_enableInt(0); // kick-switch
    GPIO_enableInt(1); // kill-switch
    

    
}


void ConfigureMotors(void) {

    ConfigureMotorPins(); //Set pWM, GPIO, input/output

    //IS THERE EVEN ANYTHING ELSE TO BE DONE?!?
    
}

void kill_motors(bool stop_hard){
    
    stopMotor(stop_hard);


}

void SW1_kick_ISR(void){
    enableMotor();
    GPIOIntClear(GPIO_PORTJ_BASE, GPIO_INT_PIN_0); // MUST CLEAR THE INTERRUPT
    Hall_a = GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_3);
    Hall_b = GPIOPinRead(GPIO_PORTH_BASE, GPIO_PIN_2);
    Hall_c = GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_2);
    // KICK IT!
    updateMotor(Hall_a, Hall_b, Hall_c);
}

void SW2_kill_ISR(void){
    GPIOIntClear(GPIO_PORTJ_BASE, GPIO_INT_PIN_1); // MUST CLEAR THE INTERRUPT
    UInt gateKey = GateHwi_enter(gateHwi); // PROTECT THE OPERATIONS
    //stopMotor(true);
    disableMotor();
    GateHwi_leave(gateHwi, gateKey); // Release the operation
}

void HallSensorA_isr(void)
{
    GPIOIntClear(GPIO_PORTM_BASE, GPIO_INT_PIN_3); // MUST CLEAR THE INTERRUPT
    UInt gateKey;
    gateKey = GateHwi_enter(gateHwi); // PROTECT THE OPERATIONS
    //System_printf("Triggered\n");
    //System_flush();
    /*
    1) Read all three (oR ONE?) Hall sensors, dump it into a register or structure
    2) Flag a Hall-read event
        2b) We could have this ISR set a specific task-thread to READY,
            if we design that task to switch itself to WAITING, without
            actually asking it to wait upon anything
    3) Increment the speed counter
    4) updateMotor();
    */
   
    // Read the GPIO line of the halls
    Hall_a = !Hall_a;
    
    // Post the relevant event OR set a specific task to READY

    //New motor position registered, means new motor position demanded.
    updateMotor(Hall_a, Hall_b, Hall_c);
    

    // Increment the speed counter. Not actually sure what that means
    //gateKey = GateHwi_enter(gateHwi); // PROTECT THE OPERATIONS
    //Do a thing
    GateHwi_leave(gateHwi, gateKey); // Release the operation
}

void HallSensorB_isr(void)
{
   GPIOIntClear(GPIO_PORTH_BASE, GPIO_INT_PIN_2);//MUST CLEAR THE INTERRUPT
    UInt gateKey;
    gateKey = GateHwi_enter(gateHwi); // PROTECT THE OPERATIONS
    Hall_b = !Hall_b;
    updateMotor(Hall_a, Hall_b, Hall_c);
    GateHwi_leave(gateHwi, gateKey); // Release the operation
}

void HallSensorC_isr(void)
{
    
    GPIOIntClear(GPIO_PORTN_BASE, GPIO_INT_PIN_2); //MUST CLEAR THE INTERRUPT
    UInt gateKey;
    gateKey = GateHwi_enter(gateHwi); // PROTECT THE OPERATIONS
    Hall_c = !Hall_c;
    updateMotor(Hall_a, Hall_b, Hall_c);
    GateHwi_leave(gateHwi, gateKey); // Release the operation
}

void MotorTransitionTask(UArg arg0, UArg arg1){
    MotorArgStruct_t* args = (MotorArgStruct_t*)arg0;
    Mailbox_Handle mbxHandle = args->mbxHandle;
    int mbxMaxMessages = args->max_mailbox_messages;
    MailboxMessage TheMessage;

    int timeout = 50;

    bool quitter = false;
    while(quitter == false){
        if(Mailbox_pend(mbxHandle, &TheMessage, 50)){
            System_printf("Just received id = %d val = '%c' ...\n",
            TheMessage.id, TheMessage.val);
            System_flush();





            if(TheMessage.val == 'Q'){
                System_printf("QUIT SIGNAL RECEIVED");
                System_flush();
                quitter = true;
            }
        }
    }
    while(0){
        //SysCtlDelay(50);

        bool A_read = GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_3);
        bool B_read = GPIOPinRead(GPIO_PORTH_BASE, GPIO_PIN_2);
        bool C_read = GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_2);
        
        //int halls = Hall_a<<2|Hall_b<<1|Hall_c;
        System_printf("Hall: %i%i%i\n", A_read, B_read, C_read);
        System_flush();
    }


    Task_exit();
}


void MotorTask(UArg arg0, UArg arg1){

    MotorArgStruct_t* args = (MotorArgStruct_t*)arg0;
    Mailbox_Handle mbxHandle = args->mbxHandle;
    int mbxMaxMessages = args->max_mailbox_messages;
    MailboxMessage TheMessage;
    int timeout = 50;

    ConfigureMotors();

    //Write a message
    for (int i=0; i < mbxMaxMessages; i++) {
        /* Fill in value */
        TheMessage.id = i;
        TheMessage.val = i + 'a';
        TheMessage.valNumber = 20+i;

        System_printf("Writing message id = %d val = '%i' ...\n",
        TheMessage.id, TheMessage.valNumber);
        System_flush();

        /* Enqueue message */
        Mailbox_post(mbxHandle,&TheMessage,timeout);
        
    }
    
    TheMessage.val = 'Q';
    System_printf("Writing QUIT...\n");
    System_flush();
    Mailbox_post(mbxHandle,&TheMessage,timeout);

    Task_exit();


}


