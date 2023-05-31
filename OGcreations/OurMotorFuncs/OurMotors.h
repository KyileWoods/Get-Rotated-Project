/*
 * OurMotors.h
 *
 *  Created on: 13 May 2023
 *      Author: Delluser
 */

#ifndef OGCREATIONS_OURMOTORFUNCS_OURMOTORS_H_
#define OGCREATIONS_OURMOTORFUNCS_OURMOTORS_H_

#include <xdc/std.h>
#include <stdbool.h>
#include <ti/sysbios/knl/Task.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include <ti/drivers/UART.h>  // For UART print


// typedef struct MotorsComms{
//     /* QUEUE's*/
//     Queue_Struct HallISR_Queue;
//     Queue_Handle HallISR_QueueHandle;
//     Queue_Params HallISR_QueueParams;
// } MotorComms_t;

// SETUP
void ConfigureMotorPins(void);
void ConfigureMotors(void);

// Button to trigger any interrupt function you want
void SW1_ISR(void);
void SW2_ISR(void);


// ISR prototypes
void HallSensorA_isr(void);
void HallSensorB_isr(void);
void HallSensorC_isr(void);

// OPERATIONS
void MotorsPrelude(UArg arg0);
void CalcMotorSpeed(UArg arg0);
void DrivingModeTask(UArg arg0, UArg arg1);
void kick_motors_on(void);
void kill_motors(void);
void MotorMonitorTask(UArg arg0, UArg arg1);
void MotorTask(UArg arg0, UArg arg1);
void initUart(UART_Handle *uart);

#endif /* OGCREATIONS_OURMOTORFUNCS_OURMOTORS_H_ */
