/*
 *  Filename:    Sensing.h
 *
 *  Description: Header file for My Module.
 *
 *  Created on:  [May 2023]
 *  Author:      [Phoenix Seybold]
 */

#ifndef SENSING_MODUAL_H
#define SENSING_MODUAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <stdint.h>


/* Macros and constants */
// globals
#define BUFFER_SIZE 100
#define WINDOW_SIZE 10

// STRUCTS
typedef struct BMI160sensorStruct{
    float Acc_x, Acc_y, Acc_z;
    float Gyr_x, Gyr_y ,Gyr_z;
};


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



#ifdef __cplusplus
}
#endif

#endif /* SENSING_MODUAL_H */




