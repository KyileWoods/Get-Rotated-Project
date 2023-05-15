/*
 * OurMotors.h
 *
 *  Created on: 13 May 2023
 *      Author: Delluser
 */

#ifndef OGCREATIONS_OURMOTORFUNCS_OURMOTORS_H_
#define OGCREATIONS_OURMOTORFUNCS_OURMOTORS_H_

void ConfigureMotorPins(void);
void ConfigureMotors(void);
void kill_motors(bool be_gentle);
void HallSensorA_isr(void);

void HallSensorB_isr(void);
void HallSensorC_isr(void);

#endif /* OGCREATIONS_OURMOTORFUNCS_OURMOTORS_H_ */
