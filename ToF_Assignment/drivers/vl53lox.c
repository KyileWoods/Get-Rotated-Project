/*
 * vl53l0x.c
 *
 *  Created on: 10 May 2023
 *      Author: Sam
 */

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "i2cOptDriver.h"
#include "drivers/vl53lox.h"
#include <xdc/runtime/System.h>

/* ------------------------------------------------------------------------------------------------
 *                                           Constants
 * ------------------------------------------------------------------------------------------------
 */

/* Slave address */
#define VL53L0X_I2C_ADDRESS             0x52    // Sensor Address found in: https://files.seeedstudio.com/wiki/Grove-Time_of_Flight_Distance_Sensor-VL53L0X-/res/vl53l0x-datasheet.pdf

/* Register addresses */
#define SYSRANGE_START                  0x00
#define RESULT_INTERRUPT_STATUS         0x13
#define RESULT_RANGE_STATUS             0x14

/* Register values */
#define VL53L0X_STARTSTOP               0x01

/* Register addresses */
#define REG_RESULT                      0x00
#define REG_CONFIGURATION               0x01
#define REG_LOW_LIMIT                   0x02
#define REG_HIGH_LIMIT                  0x03

#define REG_MANUFACTURER_ID             0x7E
#define REG_DEVICE_ID                   0x7F

/* Register values */
#define MANUFACTURER_ID                 0x5449  // TI
#define DEVICE_ID                       0x3001  // Opt 3001

#define CONFIG_RESET                    0xC810
#define CONFIG_TEST                     0xCC10

#define CONFIG_ENABLE                   0x10C4 // equivalent to 0xC410 as upper and lower bytes are received in reverse (100 ms, continuous)
#define CONFIG_DISABLE                  0x10C0 //  equivalent to 0xC010 as upper and lower bytes are received in reverse  (100 ms, shutdown)

/* Bit values */
#define DATA_RDY_BIT                    0x0080  // Data ready

/* Register length */
#define REGISTER_LENGTH                 2

/* Sensor data size */
#define DATA_LENGTH                     2

/* ------------------------------------------------------------------------------------------------
 *                                           Local Functions
 * ------------------------------------------------------------------------------------------------
 */


/* ------------------------------------------------------------------------------------------------
 *                                           Local Variables
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                           Public functions
 * -------------------------------------------------------------------------------------------------
 */


/**************************************************************************************************
 * @fn          sensorVL53L0XInit
 *
 * @brief       Initialize the distance sensor driver
 *
 * @return      none
 **************************************************************************************************/
bool sensorVL53L0XInit(void)
{
    sensorVL53L0XEnable(false);

    return (true);
}


/**************************************************************************************************
 * @fn          sensorVL53L0XEnable
 *
 * @brief       Turn the sensor on/off
 *
 * @return      none
 **************************************************************************************************/
void sensorVL53L0XEnable(bool enable)
{
    uint8_t val;

    if (enable)
    {
        val = VL53L0X_STARTSTOP;
    }
    else
    {
        val = 0x00;
    }

    bool success = writeI2C(VL53L0X_I2C_ADDRESS, SYSRANGE_START, &val);
    if (!success) {
        System_printf("sensorVL53L0X fail!");
    }
}


/**************************************************************************************************
 * @fn          sensorVL53L0XRead
 *
 * @brief       Read the result register
 *
 * @param       Buffer to store data in
 *
 * @return      TRUE if valid data
 **************************************************************************************************/
bool sensorVL53L0XRead(uint16_t *rawData)
{
    bool data_ready;
    uint16_t val;

    //Get status of configuration register
    if (readI2C(VL53L0X_I2C_ADDRESS, REG_CONFIGURATION, (uint8_t *)&val))
    {
        data_ready = ((val>>8 &0xFF) & DATA_RDY_BIT) == DATA_RDY_BIT;
    }
    else
    {
        return false;
    }

    if (data_ready)
    {
        if (readI2C(VL53L0X_I2C_ADDRESS, REG_RESULT, (uint8_t *)&val))
        {
            // Swap bytes
            *rawData = (val << 8) | (val>>8 &0xFF);
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 * @fn          sensorOpt3001Test
 *
 * @brief       Run a sensor self-test
 *
 * @return      TRUE if passed, FALSE if failed
 **************************************************************************************************/
bool sensorVL53L0XTest(void)
{
    uint16_t val;

    // Check manufacturer ID
    readI2C(VL53L0X_I2C_ADDRESS, REG_MANUFACTURER_ID, (uint8_t *)&val);
    val = (val << 8) | (val>>8 &0xFF);

    if (val != MANUFACTURER_ID)
    {
        return (false);
    }

    System_printf("Manufacturer ID Correct: %c%c\n", val & 0x00FF, (val >> 8) & 0x00FF);

    // Check device ID
    readI2C(VL53L0X_I2C_ADDRESS, REG_DEVICE_ID, (uint8_t *)&val);
    val = (val << 8) | (val>>8 &0xFF);

    if (val != DEVICE_ID)
    {
        return (false);
    }

    System_printf("Device ID Correct: %c%c\n", val & 0x00FF, (val >> 8) & 0x00FF);

    return (true);
}

/**************************************************************************************************
 * @fn          sensorVL53L0XConvert
 *
 * @brief       Convert raw data to distance (in millimeters)
 *
 * @param       rawData - raw data from sensor
 *
 * @param       convertedDistance - converted value (distance in millimeters)
 *
 * @return      none
 **************************************************************************************************/
void sensorVL53L0XConvert(uint16_t rawData, float *convertedDistance)
{
*convertedDistance = (float) rawData;
}
