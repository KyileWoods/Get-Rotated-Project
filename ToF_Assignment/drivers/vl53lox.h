/*
 * vl53l0x.h
 *
 *  Created on: 9 May 2023
 *      Author: Sam
 */

#ifndef VL53LOX_H
#define VL53LOX_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * FUNCTIONS
 */
extern bool sensorVL53L0XInit(void);
extern void sensorVL53L0XEnable(bool enable);
extern bool sensorVL53L0XRead(uint16_t *rawData);
extern void sensorVL53L0XConvert(uint16_t rawData, float *convertedDistance);
extern bool sensorVL53L0XTest(void);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_VL53LOX_H_ */
