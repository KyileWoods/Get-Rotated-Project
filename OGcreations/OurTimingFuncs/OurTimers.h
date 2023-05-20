/*
 * OurTimers.h
 *
 *  Created on: 13 May 2023
 *      Author: Delluser
 */

#ifndef OGCREATIONS_OURTIMINGFUNCS_OURTIMERS_H_
#define OGCREATIONS_OURTIMINGFUNCS_OURTIMERS_H_

#include <ti/sysbios/knl/Clock.h>

/* Here is the template for creating a clock HWI and associated SWI*/
Clock_Struct clk0Struct;
Clock_Handle clk0Handle;

Clock_Struct CalcMotorSpeed_clkStruct;
Clock_Handle CalcMotorSpeed_clkHandle;

Void clk0_swi_clk_fxn(UArg arg0);



#endif /* OGCREATIONS_OURTIMINGFUNCS_OURTIMERS_H_ */
