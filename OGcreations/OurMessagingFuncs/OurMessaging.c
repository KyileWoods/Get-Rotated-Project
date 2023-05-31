#include <stdbool.h>
#include <stdint.h>
#include <ti/sysbios/knl/Mailbox.h>
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
// any other includes your file needs



uint32_t g_ui32SysClock;

void SendMailInteger(Mailbox_Handle MailboxHandle, int speed){
    
}

void SendMailPointer(Mailbox_Handle MailboxHandle, void* PointerAsVoid){

}

void SendMailBool(bool PostBool){

}

void ConfigureUART(void){
    //
    // Enable the GPIO Peripheral used by the UART.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // Enable UART0
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    // Configure GPIO Pins for UART mode.
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, g_ui32SysClock);
}

