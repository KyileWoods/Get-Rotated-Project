/*
 *  Filename:    Sensing.h
 *
 *  Description: Header file for My Module.
 *
 *  Created on:  [May 2023]
 *  Author:      [Phoenix Seybold, N9379410]
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

/*   ADC current shunt stats   */
#define R_Sense     0.007;        // Current resistance
#define G_csa       10;           // Gain setting
#define V_vref      24;           // Motor Voltage
#define ADC_voltage 3.3;          // Logic voltage range for ADC
#define Sample_Resolution 4096;   // Sampling resolution of ADC



/* Function prototypes */

/**
 * @brief Initialize My Module.
 *
 * @return Status of initialization.
 *         - 0: Success
 *         - Negative value: Error code
 */
//int32_t my_module_init(void);


/**
 * @brief Perform an action in My Module.
 *
 * @param[in] parameter: Action parameter.
 * @return Status of the action.
 *         - 0: Success
 *         - Negative value: Error code
 */
//int32_t my_module_action(uint32_t parameter);



#ifdef __cplusplus
}
#endif

#endif /* SENSING_MODUAL_H */




