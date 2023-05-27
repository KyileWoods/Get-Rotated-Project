/*
 *  Filename:    ACCELEROMETER.h
 *
 *  Description: Header file for My Module.
 *
 *  Created on:  [May 2023]
 *  Author:      [Phoenix Seybold, N9379410]
 */

#ifndef BMI160_ACCELEROMETER_H
#define BMI160_ACCELEROMETER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <stdint.h>

/*      ADRESSES    */

// SLAVE
#define



/* Function prototypes */

/**
 * @brief Initialize My Module.
 *
 * @return Status of initialization.
 *         - 0: Success
 *         - Negative value: Error code
 */
int32_t my_module_init(void);

/**
 * @brief Perform an action in My Module.
 *
 * @param[in] parameter: Action parameter.
 * @return Status of the action.
 *         - 0: Success
 *         - Negative value: Error code
 */
int32_t my_module_action(uint32_t parameter);

/* Macros and constants */
//#define MY_MODULE_CONSTANT     (42)

#ifdef __cplusplus
}
#endif

#endif /* BMI160_ACCELEROMETER_H */
