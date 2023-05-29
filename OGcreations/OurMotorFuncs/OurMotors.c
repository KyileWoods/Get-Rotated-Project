#include <stdbool.h>
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Task.h> // For Task_exit()
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Clock.h>
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
// THIS WAS INTENDED TO ONLY BE FOR MAIN.C, but is has a PRIORITY ENUM struct
#include "OGcreations/MainInclude.h" 
#include "shared_mem.h"
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/gates/GateHwi.h>



/* EVENTS*/
extern Event_Struct MotorEventStruct;
extern Event_Handle MotorEventHandle;
#define MonitorMailboxEventID Event_Id_03

/* EVENTS*/
extern Event_Struct evtStruct;
extern Event_Handle MotorEventHandle;
#define MonitorMailboxEventID Event_Id_03

/* TASKS  */
#define MOTORMONITORTASKSTACKSIZE   512
#define MAXMONITORMESSAGES 15
#define MOTORMONITORTASKSTACKSIZE   512
#define MAXMONITORMESSAGES 15
Task_Struct MotorMonitorTask_Struct;
Char MotorMonitorTask_Stack[MOTORMONITORTASKSTACKSIZE];
Char MotorMonitorTask_Stack[MOTORMONITORTASKSTACKSIZE];

/* QUEUE's*/
Queue_Struct HallISR_Queue;
Queue_Handle HallISR_QueueHandle;
Queue_Params HallISR_QueueParams;

/* MAILBOXES */
Mailbox_Struct MonitorMailStruct;
Mailbox_Handle MonitorMailHandle;
Mailbox_Params MotorMailParams;

/* Highly Muteable*/
Task_Params taskParams;
Event_Params eventParams;
Clock_Params clkParams;
Error_Block ErrorBlock;


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
    System_printf("QUITted\n");
    System_flush();
}

void CalcMotorSpeed(UArg arg0){
              //(Counts/totalSegments)/frequency;
    SpeedValue = (SpeedCounter/24)    /150;
    //printf("Speed: %i\n", SpeedValue);
}

void MotorMonitorTask(UArg arg0, UArg arg1){
    MotorArgStruct_t* args = (MotorArgStruct_t*)arg0;
    Mailbox_Handle mbxHandle = args->mbxHandle;
    int mbxMaxMessages = args->max_mailbox_messages;
    MailboxMessage TheMessage;

    int timeout = 50;

    bool quitter = false;
    while(quitter == false){
        if( Mailbox_pend(mbxHandle, &TheMessage, 500)){
            System_printf("Just received id = %d val = '%c' ...\n",
            TheMessage.id, TheMessage.val);
            System_flush();
            
            if(TheMessage.val == 'Q'){
                System_printf("QUIT SIGNAL RECEIVED {IGNORING}\n");
                System_flush();
                //quitter = true;
            }
        }
        else{
            System_printf("Speed: %i\n", SpeedValue);
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

void DrivingModeTask(UArg arg0, UArg arg1){
/* A High-priority task used for handling RTOS decisions and
analysis for when the motor is in DRIVING mode
*/

}

void MotorsPrelude(void){
    ConfigureMotors();
    

    Error_init(&ErrorBlock);// Move this somewhere
    bool MotorLibSuccess = initMotorLib(50, &ErrorBlock);
    if (!MotorLibSuccess) {
        Error_print(&ErrorBlock);
        System_abort("Error Initializing Motor\n");
    }
    setDuty(30);

    // Creating the task
    MotorArgStruct_t MotorFxnArgs;
    MotorFxnArgs.mbxHandle = MonitorMailHandle;
    Task_Params_init(&taskParams);
    taskParams.arg0 = (UArg) &MotorFxnArgs;
    taskParams.stackSize = MOTORMONITORTASKSTACKSIZE;
    taskParams.priority = MOTORMONITORTASK_PRIORITY;
    taskParams.stack = &MotorMonitorTask_Stack;
    System_printf("Tasking time\n");
    System_flush();
    System_printf("Tasking time\n");
    System_flush();
    Task_construct(&MotorMonitorTask_Struct, (Task_FuncPtr)MotorMonitorTask, &taskParams, NULL);
}

void MotorTask(UArg arg0, UArg arg1){
    /* A low-priority background system for determining some non-RTOS functionality*/

    System_flush();
    /* P R E L U D E */
    MotorsPrelude();
    MotorsPrelude();
    WriteAMessage(arg0);
    System_printf("Preluded\n");
    System_flush();
    System_printf("Preluded\n");
    System_flush();

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
                // Create the conditions needed to ignite the motor
                enableMotor(); //This is the key.
                Hall_a = GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_3);
                Hall_b = GPIOPinRead(GPIO_PORTH_BASE, GPIO_PIN_2);
                Hall_c = GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_2);
                // KICK IT!
                updateMotor(Hall_a, Hall_b, Hall_c);
                DRIVE_RULESET = DRIVE_RUNNING;
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


