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

/* QUT driver files*/
//#include "drivers/opt3001.h"
//#include "drivers/i2cOptDriver.h"
#include "drivers/motorlib.h"

/* OG ROTATED CREATIONS */
#include "OGcreations/MainInclude.h"
#include "OGcreations/OurEventsFuncs/OurEvents.h"
#include "OGcreations/OurMessagingFuncs/OurMessaging.h"
#include "OGcreations/OurTimingFuncs/OurTimers.h"
#include "OGcreations/OurMotorFuncs/OurMotors.h"
#include "shared_mem.h"
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

/* TASKS  */
#define MOTORTASKSTACKSIZE   512
Task_Struct MotorTask_Struct;
Char MotorTask_Stack[MOTORTASKSTACKSIZE];
Task_Struct MotorTransitionTask_Struct;
Char MotorTransitionTask_Stack[MOTORTASKSTACKSIZE];


#define TASKSTACKSIZE   512
Task_Struct task1Struct;
Char task1Stack[TASKSTACKSIZE];
Task_Struct task2Struct;
Char task2Stack[TASKSTACKSIZE];
#define STACKSIZE 1024
Char taskStack[STACKSIZE];

/*SEMAPHORES, KEYS, FLAGS*/


/* EVENTS*/
Event_Struct evtStruct;
Event_Handle evtHandle;
#define MotorMailboxEventID Event_Id_02

/* MAILBOXES */
#define MAXMOTORMESSAGES 10
Mailbox_Struct mbxStruct;
Mailbox_Handle mbxHandle;

/* QUEUE's*/
Queue_Struct HallISR_Queue;
Queue_Handle HallISR_QueueHandle;
Queue_Params HallISR_QueueParams;

/* GLOBALS*/
bool Hall_a=0;
bool Hall_b=0;
bool Hall_c=0;


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

void task_function_skeleton(void){

    bool Condition_maybe_forever= true;

    /*Prologue*/
    //Initilizations
    //Declarations

    while(Condition_maybe_forever){
        /*Process*/
        //Maybe runs forever, or has an exit condition but this probably shouldn't be very instantly exitted

    }

    /*Epilogue*/

    //Release your memory
    //Shut down the task
    //Doesn't have to ever be reached
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
    Board_initGPIO();
    Board_initI2C();
    i2c_init();
    // Board_initEMAC();
    // Board_initSDSPI();
    // Board_initSPI();
    // Board_initUART();
    // Board_initUSB(Board_USBDEVICE);
    // Board_initUSBMSCHFatFs();
    // Board_initWatchdog();
    // Board_initWiFi();
    // sensorOpt3001Init();  // Initialize OPT3001 sensor


    //initialise the motor library
        /* ANY CONFUSING CODE BELOW HERE IS LIKELY TO BE DEBUG PURPOSES, FOR A SPECIFIC PROBLEM. DELETE JUDICIOUSLY

        This function requires an *eb errorbreak but it is breaking at this point.
        Probably an error is being thrown, but with NULL in EB it is being thrown to 0xffffff and crashing*/

        setDuty(50); //Debug only



        int enter = 1; //This is an if-guard which shouldn't exist, except this stupid error.
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
        }
        setDuty(30);

    /*===== CREATE EVERYTHING ABOUT MOTOR CONTROLLER FUNCTION=====*/
    /* Construct a Mailbox Instance */
    Mailbox_Params_init(&mbxParams);
    mbxParams.readerEvent = evtHandle;
    mbxParams.readerEventId = MotorMailboxEventID;
    Mailbox_construct(&mbxStruct,sizeof(MsgObj), MAXMOTORMESSAGES, &mbxParams, NULL);
    mbxHandle = Mailbox_handle(&mbxStruct);


    MotorArgStruct_t MotorFxnArgs;
    MotorFxnArgs.mbxHandle = mbxHandle;
    MotorFxnArgs.max_mailbox_messages = MAXMOTORMESSAGES; //For iterating to a limit

    Task_Params_init(&taskParams); //Start a generic task_params
        taskParams.arg0 = (UArg) &MotorFxnArgs;
        taskParams.stackSize = MOTORTASKSTACKSIZE;
        taskParams.priority = MOTORTASK_PRIORITY;
        taskParams.stack = &MotorTask_Stack;
        Task_construct(&MotorTask_Struct, (Task_FuncPtr)MotorTask, &taskParams, NULL);
        taskParams.stack = &MotorTransitionTask_Stack;
        Task_construct(&MotorTransitionTask_Struct, (Task_FuncPtr)MotorTransitionTask, &taskParams, NULL);
    //Generic Task-creation template. Anything assigned a value BEFORE needs to be re-written.
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.priority = SUPERLOW_PRIORITY_TASK;
    taskParams.stack = &task1Stack;
    Task_construct(&task1Struct, (Task_FuncPtr)MotorTransitionTask, &taskParams, NULL);



    Clock_Params_init(&clkParams);
    clkParams.startFlag = TRUE;
    int Hz = 150; //Define the frequency to Hwi this clock
    clkParams.period = 1000/Hz; // 1000 Tick-> 1 second
    Clock_construct(&CalcMotorSpeed_clkStruct, (Clock_FuncPtr)CalcMotorSpeed,
                    50, &clkParams);
    CalcMotorSpeed_clkHandle = Clock_handle(&CalcMotorSpeed_clkStruct);

    Clock_construct(&clk0Struct, (Clock_FuncPtr)clk0_swi_clk_fxn,
                    50, &clkParams);
    clk0Handle = Clock_handle(&clk0Struct);


    

    Event_Params evtParams; //Declare params locally
    Event_Params_init(&evtParams); //Initialise it with default values
    //Event_create is DYNAMIC; even_construct is STATIC memory allocation-- PROTOTYPE: Event_Handle Event_create(Event_Params *__prms, xdc_runtime_Error_Block *__eb );
    Event_construct(&evtStruct, &evtParams); //Construct an event according to params, populate a struct with the event's unique information
    evtHandle = Event_handle(&evtStruct);// Pull the event handle out from the structure, for easy reference
    if (evtHandle == NULL) {
        //System_printf("Event creation failed :( ");
    }

    
    Queue_Params_init(&HallISR_QueueParams); // Initialize with default parameters
    Queue_construct(&HallISR_Queue, &HallISR_QueueParams);
    HallISR_QueueHandle = Queue_handle(&HallISR_Queue);
    

    //GPIO_setCallback(Port M, Pin 3,HallSensorA_isr);


    /* Start BIOS */
    BIOS_start();

    /*
    BIOS_start NEVER EVER returns control to main()
    This is the end of the program
    */
    System_printf("\n...................................Hello?!\n");
    //System_flush(); THIS HALTS THE CPU?!!!!! WARNING ABOUT FLUSH

    return (0);
}
