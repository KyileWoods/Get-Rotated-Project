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

// Button to trigger any interrupt function you want
void SW1_ISR(void);
void SW2_ISR(void);


// ISR prototypes
void HallSensorA_isr(void);
void HallSensorB_isr(void);
void HallSensorC_isr(void);

// OPERATIONS
void CalcMotorSpeed(UArg arg0);
void DrivingModeTask(UArg arg0, UArg arg1);
void kick_motors_on(void);
void kill_motors(void);
void MotorMonitorTask(UArg arg0, UArg arg1);
void MotorTask(UArg arg0, UArg arg1);

#endif /* OGCREATIONS_OURMOTORFUNCS_OURMOTORS_H_ */
