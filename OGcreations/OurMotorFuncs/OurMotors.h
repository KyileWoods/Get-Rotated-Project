/*
 * OurMotors.h
 *
 *  Created on: 13 May 2023
 *      Author: Delluser
 */

#ifndef OGCREATIONS_OURMOTORFUNCS_OURMOTORS_H_
#define OGCREATIONS_OURMOTORFUNCS_OURMOTORS_H_


// SETUP
void ConfigureMotorPins(void);
void ConfigureMotors(void);

// ISR prototypes
void HallSensorA_isr(void);
void HallSensorB_isr(void);
void HallSensorC_isr(void);

// OPERATIONS
void kill_motors(bool be_gentle);
void MotorTransitionTask(UArg arg0, UArg arg1);
void MotorTask(UArg arg0, UArg arg1);

#endif /* OGCREATIONS_OURMOTORFUNCS_OURMOTORS_H_ */
