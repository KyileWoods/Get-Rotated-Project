/*
 * OurTimers.c
 *
 *  Created on: 13 May 2023
 *      Author: Delluser
 */


/* INCLUDES*/
#include <stdbool.h>
#include <ti/sysbios/knl/Mailbox.h>
#include "OGcreations/OurEventsFuncs/OurEvents.h"
#include <ti/sysbios/knl/Event.h>
#include <xdc/runtime/System.h>
#include "Board.h"
// any other includes your file needs


void clk0_swi_clk_fxn(UArg arg0){
    //Extract the relevant variables from the (void??)pointer "arg0".
    //Including evtHandle
    GPIO_toggle(Board_LED0);
    //Event_post(evtHandle, OUR_EVENT_NAME);
    System_printf("clock-0: bing\n");
    System_flush();
}




