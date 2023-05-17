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

// SETUP
void ConfigureMotorPins(void);
void ConfigureMotors(void);

// Fake function, called on Button-1 press
void SW1_kick_ISR(void);
void SW2_kill_ISR(void);

// ISR prototypes
void HallSensorA_isr(void);
void HallSensorB_isr(void);
void HallSensorC_isr(void);

// OPERATIONS
void kill_motors(bool stop_hard);
void MotorTransitionTask(UArg arg0, UArg arg1);
void MotorTask(UArg arg0, UArg arg1);

#endif /* OGCREATIONS_OURMOTORFUNCS_OURMOTORS_H_ */
