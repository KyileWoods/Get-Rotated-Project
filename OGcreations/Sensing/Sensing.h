/*
 * Sensing.h
 *
 *  Created on: May 2023
 *      Author: Phoenix Seybold
 */
 /* 
  * Th
  *
  */
/* HANDELS */
Swi_Handle          CurrSwi, SpeedSwi;

Clock_Handle        clkSpeed, clkPower;
Clock_Struct        clkSpeedStruct, clkPowerStruct;

Event_Struct        evtStruct;
Event_Handle        evtHandle;

// globals
#define BUFFER_SIZE 100
#define WINDOW_SIZE 10
