#include <stdbool.h>
#include <stdio.h>

#include <ti/drivers/GPIO.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Task.h> // For Task_exit()
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Clock.h>

#include <xdc/runtime/System.h> //For printf'ing
#include <ti/drivers/UART.h>  // For UART print

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"



//#include "Board.h"
#include "motorlib.h"
#include "OGcreations/OurMotorFuncs/OurMotors.h"
#include "OGcreations/OurMessagingFuncs/OurMessaging.h"
// THIS WAS INTENDED TO ONLY BE FOR MAIN.C, but is has a PRIORITY ENUM struct
#include "OGcreations/MainInclude.h" 
#include "shared_mem.h"
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/gates/GateHwi.h>
#include <ti/sysbios/BIOS.h>


/* HANDLES */
UART_Handle uart;
char    uartbuff[30];

/* EVENTS*/
extern Event_Struct evtStruct;
extern Event_Handle MotorEventHandle;
#define MonitorMailboxEventID Event_Id_03

/* TASKS  */
#define MOTORMONITORTASKSTACKSIZE   512
#define MAXMONITORMESSAGES 15
Task_Struct MotorMonitorTask_Struct;
Char MotorMonitorTask_Stack[MOTORMONITORTASKSTACKSIZE];

/* QUEUE's*/
Queue_Struct HallISR_Queue;
Queue_Handle HallISR_QueueHandle;
Queue_Params HallISR_QueueParams;

/* MAILBOXES */

/* Highly Muteable*/
Task_Params taskParams;
Event_Params eventParams;
Clock_Params clkParams;
Error_Block ErrorBlock;
extern char    uartbuff[30];



uint_fast16_t SpeedCounter;
uint_fast16_t SpeedValue;

GateHwi_Handle gateHwi;

/* GLOBALS*/
bool Hall_a=0;
bool Hall_b=0;
bool Hall_c=0;

enum MOTOR_STATE{
    DRIVE_IDLE,
    DRIVE_TRANSITION_STARTING,
    DRIVE_RUNNING,
    DRIVE_TRANSITION_STOPPING,
    DRIVE_E_STOP,
    DRIVE_EXPLOSION,


    
    DRIVE_N_STATES //Equal to the value of valid motor states
};
enum MOTOR_STATE DRIVE_RULESET = DRIVE_IDLE;

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
    GPIO_enableInt(0); // Switch-1
    GPIO_enableInt(1); // Switch-2


    GPIO_enableInt(4);
    GPIO_enableInt(5);
    GPIO_enableInt(6);
}


void ConfigureMotors(void) {

    ConfigureMotorPins(); //Set pWM, GPIO, input/output
    Queue_Params_init(&HallISR_QueueParams); // Initialize with default parameters
    Queue_construct(&HallISR_Queue, &HallISR_QueueParams);
    HallISR_QueueHandle = Queue_handle(&HallISR_Queue);
    
    //Set an initial motor state.
    DRIVE_RULESET = DRIVE_IDLE;
    SpeedCounter = 0;

    
}


// KEEP a generic function on these buttons, so we can easily,
// quickly drop a new prototyping function onto this interrupt (Otherwise you have to change configs in board.h)
void SW1_ISR(void){kick_motors_on();}
void kick_motors_on(void){
    GPIOIntClear(GPIO_PORTJ_BASE, GPIO_INT_PIN_0); // MUST CLEAR THE INTERRUPT
    DRIVE_RULESET = DRIVE_TRANSITION_STARTING;
}

void SW2_ISR(void){kill_motors();}
void kill_motors(void){
    GPIOIntClear(GPIO_PORTJ_BASE, GPIO_INT_PIN_1); // MUST CLEAR THE INTERRUPT
    UInt gateKey = GateHwi_enter(gateHwi); // PROTECT THE OPERATIONS
    disableMotor();
    GateHwi_leave(gateHwi, gateKey); // Release the operation
    DRIVE_RULESET = DRIVE_IDLE;
}

void HallSensorA_isr(void)
{
    Hall_a = !Hall_a; // Toggle new Hall-state
    updateMotor(Hall_a, Hall_b, Hall_c); //Try keep this temporally close to interrupt event
    GPIOIntClear(GPIO_PORTM_BASE, GPIO_INT_PIN_3); // MUST CLEAR THE INTERRUPT
    
    /*
    1) Read all three (oR ONE?) Hall sensors, dump it into a register or structure
    2) Flag a Hall-read event
        2b) We could have this ISR set a specific task-thread to READY,
            if we design that task to switch itself to WAITING, without
            actually asking it to wait upon anything
    3) Increment the speed counter
    4) updateMotor();
    */
   
   UInt gateKey;
    gateKey = GateHwi_enter(gateHwi); // PROTECT THE OPERATIONS
    SpeedCounter++;
    GateHwi_leave(gateHwi, gateKey); // Release the operation
}

void HallSensorB_isr(void)
{
    Hall_b = !Hall_b;
    updateMotor(Hall_a, Hall_b, Hall_c);
    GPIOIntClear(GPIO_PORTH_BASE, GPIO_INT_PIN_2);//MUST CLEAR THE INTERRUPT
    UInt gateKey;
    gateKey = GateHwi_enter(gateHwi); // PROTECT THE OPERATIONS
    SpeedCounter++;
    GateHwi_leave(gateHwi, gateKey); // Release the operation
}

void HallSensorC_isr(void)
{
    Hall_c = !Hall_c;
    updateMotor(Hall_a, Hall_b, Hall_c);
    GPIOIntClear(GPIO_PORTN_BASE, GPIO_INT_PIN_2); //MUST CLEAR THE INTERRUPT
    UInt gateKey;
    gateKey = GateHwi_enter(gateHwi); // PROTECT THE OPERATIONS
    SpeedCounter++;
    GateHwi_leave(gateHwi, gateKey); // Release the operation
}


void WriteAMessage(UArg arg0){
    /* Mostly at the moment this is a DEBUG function
    MotorMonitorTask is monitoring this MailBox
    This is NOT a task. It is called, and handed the task arg0*/
    MotorArgStruct_t* args = (MotorArgStruct_t*)arg0;
    Mailbox_Handle mbxHandle = args->mbxHandle;
    int mbxMaxMessages = args->max_mailbox_messages;
    MailboxMessage TheMessage;
    int timeout = 50;
    
    // Synchronize with the reader process: Pend for 1 failed wait by the MotorMonitor.
    Mailbox_pend(mbxHandle, &TheMessage, BIOS_WAIT_FOREVER);
    //Write a message 
    char TheText[20] = "The Message";
    TheText[20-1] = NULL;
    System_printf("SendString: %s\n", TheText);
    System_flush();
    int i=0;
    bool MessageComplete = false;
    while(MessageComplete == false){
        MessageComplete = (TheText[i] == NULL);
        /* Fill in value */
        TheMessage.id = 1; //Basically a sender-ID #
        TheMessage.val = TheText[i]; // A single letter at a time into the mailbox
        TheMessage.valNumber = i; //Append the index position

        System_printf("'%c'", TheMessage.val);
        UART_write(uart, uartbuff,2);
        //System_flush();

        /* Enqueue message */
        Mailbox_post(mbxHandle,&TheMessage, BIOS_WAIT_FOREVER);
        i++;

    }
}

void CalcMotorSpeed(UArg arg0){
              //(Counts/totalSegments)/frequency;
    SpeedValue = (1000*SpeedCounter/24)    /150;
    SpeedCounter = 0;
    int uartery = 0;
    if(uartery){
        snprintf(uartbuff, 20, "Speed: %i", SpeedValue);
        if(uart == NULL || UART_ERROR == UART_write(uart, uartbuff, 20)){
            printf("UART ERROR!\n");
            System_flush();
        }
    }
}

void MotorMonitorTask(UArg arg0, UArg arg1){
    MotorArgStruct_t* args = (MotorArgStruct_t*)arg0;
    Mailbox_Handle mbxHandle = args->mbxHandle;
    int mbxMaxMessages = args->max_mailbox_messages;
    MailboxMessage TheMessage;

    int timeout = 50;

    bool MoreMessage = true;
    char TheText[20];
    int i=0;
    int FailedWaits = 0;
    while(MoreMessage == true){
        if(Mailbox_pend(mbxHandle, &TheMessage, 500)){
            if(TheMessage.id!=2){ // Ignore self-reports
                FailedWaits = 0; // Resets this counter
                TheText[TheMessage.valNumber] = TheMessage.val;
                if(TheMessage.val == NULL){
                    System_printf("Got string: %s\n", TheText);
                    System_flush();
                    MoreMessage = false;
                }
            }
        }
        else{
            FailedWaits++;
            System_printf("Timeout %i\n", FailedWaits);
            System_flush();
            /* Fill in value */
            TheMessage.id = 2; //Basically a sender-ID #
            TheMessage.val = 'Q'; // An arbitrary letter to signify a recursive failure
            TheMessage.valNumber = FailedWaits;
            Mailbox_post(mbxHandle,&TheMessage, BIOS_WAIT_FOREVER);  //Could cause a dead-lock when the box is full. Fix with event pending
        }
    }
    System_printf("\nMotor Monitoring Begins NOW\n");
    while(1){
        Task_sleep(1000);
        // System_printf("Speed: %i\n", SpeedValue);
        // System_flush();
    }


    Task_exit();
}

void DrivingModeTask(UArg arg0, UArg arg1){
/* A High-priority task used for handling RTOS decisions and
analysis for when the motor is in DRIVING mode
*/

}

void initUart(UART_Handle *uart){
    UART_Params uart_Params;
    /* Create a UART with data processing off. */
    UART_Params_init(&uart_Params);
    uart_Params.readMode = UART_MODE_BLOCKING;        // CALLBACK?
    uart_Params.writeDataMode = UART_DATA_BINARY;
    uart_Params.readDataMode = UART_DATA_BINARY;
    uart_Params.readReturnMode = UART_RETURN_FULL;
    uart_Params.readEcho = UART_ECHO_OFF;
    uart_Params.readTimeout = 10;
    uart_Params.baudRate = 9600;
    *uart = UART_open(0, &uart_Params);
}

void MotorsPrelude(UArg arg0){
    MotorArgStruct_t* FOOBAR = (MotorArgStruct_t*)arg0;
    
    ConfigureMotors();
    initUart(&uart);
    
    Error_init(&ErrorBlock);
    bool MotorLibSuccess = initMotorLib(50, &ErrorBlock);
    if (!MotorLibSuccess) {
        Error_print(&ErrorBlock);
        System_abort("Error Initializing Motor\n");
    }
    setDuty(30);

    // Creating the task
    MotorArgStruct_t MotorMonitorArgs;
    MotorMonitorArgs.mbxHandle = FOOBAR->mbxHandle;
    Task_Params_init(&taskParams);
    taskParams.arg0 = (UArg) &MotorMonitorArgs;
    taskParams.stackSize = MOTORMONITORTASKSTACKSIZE;
    taskParams.priority = MOTORMONITORTASK_PRIORITY;
    taskParams.stack = &MotorMonitorTask_Stack;
    System_printf("Tasking time\n");
    System_flush();
    Task_construct(&MotorMonitorTask_Struct, (Task_FuncPtr)MotorMonitorTask, &taskParams, NULL);
}

void MotorTask(UArg arg0, UArg arg1){
    /* A low-priority background system for determining some non-RTOS functionality*/

    System_flush();
    /* P R E L U D E */
    MotorsPrelude(arg0);
    WriteAMessage(arg0);

    // // DEBUG TEST DEELETE SHORTLY
    // UARTprintf("\033[2J\033[H");
    // UARTprintf("OPT001 Example\n");
    
    /* B O D Y */

    while(DRIVE_RULESET != DRIVE_EXPLOSION){
        // event_pend()
        /*Detect anything important about the motors*/

        
        //Determine whether a ruleset change has been imposed
        switch(DRIVE_RULESET){
            case DRIVE_IDLE:
                // Wait for a trigger to begin moving

            break;
            case DRIVE_TRANSITION_STARTING:
                if(SpeedValue<1){ //Ensure A kick was good before transitioning to drivestate
                    // Create the conditions needed to ignite the motor
                    enableMotor(); //This is the key.
                    Hall_a = GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_3);
                    Hall_b = GPIOPinRead(GPIO_PORTH_BASE, GPIO_PIN_2);
                    Hall_c = GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_2);
                    // KICK IT!
                    updateMotor(Hall_a, Hall_b, Hall_c);
                }
                else{
                    DRIVE_RULESET = DRIVE_RUNNING;
                }

                break;
            case DRIVE_RUNNING:
                //printf("Motor Speed = %i\n", SpeedCounter);
                break;
        }
        

        //Determine whether a ruleset change is required

        // Modify the variables which are important to happen within this ruleset

        //Communicate with other tasks about what has happened

    }


    /* E X I T   C O N D I T I O N S*/
    Task_exit();


}


#include <ti/sysbios/knl/Task.h>
