/*
 * OurTimers.c
 *
 *  Created on: 13 May 2023
 *      Author: Delluser
 */


/* INCLUDES*/
#include <stdbool.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Event.h>
#include <xdc/runtime/System.h>
// any other includes your file needs
#include "OGcreations/OurEventsFuncs/OurEvents.h"
#include "Board.h"
#include "drivers/motorlib.h"



void clk0_swi_clk_fxn(UArg arg0){
    //Extract the relevant variables from the (void??)pointer "arg0".
    //Including evtHandle
    int p1,p2,p3; //Three motor phases GPIO high/low status
    

    //Event_post(evtHandle, OUR_EVENT_NAME);
//    System_printf("clock-0:  %i%i%i bing\n", 1,2,3);
//    System_flush();
    GPIO_toggle(Board_LED0);
}




