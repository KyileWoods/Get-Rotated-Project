/*
 * Useful links...
 *
 * TI-RTOS Example???: https://github.com/alexeicolin/tirtos/blob/master/packages/examples/i2ceeprom.c
 * TI-RTOS i2C Example???:https://github.com/alexeicolin/tirtos/blob/master/packages/examples/i2ctmp006.c
 *
 * TI-RTOS I2C Example (Best one????): https://github.com/alexeicolin/tirtos/blob/master/packages/examples/i2crf430cl330_load.c
 *
 * https://e2e.ti.com/support/microcontrollers/arm-based-microcontrollers-group/arm-based-microcontrollers/f/arm-based-microcontrollers-forum/450247/how-to-use-ti-rtos-i2c
 */



/*
 *  ======== empty_min.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
// #include <ti/drivers/EMAC.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
// #include <ti/drivers/SDSPI.h>
// #include <ti/drivers/SPI.h>
// #include <ti/drivers/UART.h>
// #include <ti/drivers/USBMSCHFatFs.h>
// #include <ti/drivers/Watchdog.h>
// #include <ti/drivers/WiFi.h>

/* Board Header file */
#include "Board.h"

#include "drivers/opt3001.h"
#include <stdio.h>


#define TASKSTACKSIZE   1024

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

I2C_Transaction             i2cTransaction;
uint8_t                     writeBuffer[3];
uint8_t                     readBuffer[2];
I2C_Handle                  i2c;
bool                        transferOK;
I2C_Params i2cParams;





/*
 *  ======== Opt3001 sensor Fxn ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
Void I2C_Opto3001Fxn(UArg arg0, UArg arg1)
{
    //    while (1) {
    //        Task_sleep((unsigned int)arg0);
    //        GPIO_toggle(Board_LED0);
    //        System_flush();
    //    }

        bool      worked, success;
        uint16_t  rawData = 0;
        float     convertedLux = 0;
        char      tempStr[20];

        // Test that sensor is set up correctly
        System_printf("Testing OPT3001 Sensor:\n");
        worked = sensorOpt3001Test();

//        while (!worked) {
//    //        SysCtlDelay(g_ui32SysClock);
//            Task_sleep(100);
//            System_printf("\nTest Failed, Trying again\n");
//            worked = sensorOpt3001Test();
//        }

        System_printf("All Tests Passed!\n\n");

        // Initialize opt3001 sensor
        sensorOpt3001Init();
        sensorOpt3001Enable(true);

        // Loop Forever
        while(1)
        {
    //        SysCtlDelay(g_ui32SysClock/100);
            Task_sleep(100);

            //Read and convert OPT values
            success = sensorOpt3001Read(&rawData);

            if (success) {
                sensorOpt3001Convert(rawData, &convertedLux);

                // Construct Text
                sprintf(tempStr, "Lux: %5.2f\n", convertedLux);
                System_printf("%s\n", tempStr);
            }
            System_flush();
        }
}

