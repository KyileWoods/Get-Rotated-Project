///*
// * OurOpto.c
// *
// *  Created on: 15 May 2023
// *      Author: Delluser
// */
//#include <stdbool.h>
//
//Void opt3001ReadFxn(UArg arg0, UArg arg1) {
//    uint16_t rawData;
//    float lux;
//
//    // Initialize OPT3001 sensor
//    sensorOpt3001Init();
//
//    // Enable the OPT3001 sensor
//    sensorOpt3001Enable(true);
//
//    while (1) {
//        // Read raw data from the OPT3001 sensor
//        if (sensorOpt3001Read(&rawData)) {
//            // Convert raw data to Lux
//            sensorOpt3001Convert(rawData, &lux);
//
//            // Print the Lux value or perform any other required operation
//            // For example: UARTprintf("Lux: %f\n", lux);
//        } else {
//            // Handle the error if necessary, e.g., log an error message
//        }
//
//        // Add a delay to control the frequency of sensor readings
//        // Replace DELAY_VALUE with an appropriate number of system ticks
//        Task_sleep(100);
//    }
//}
