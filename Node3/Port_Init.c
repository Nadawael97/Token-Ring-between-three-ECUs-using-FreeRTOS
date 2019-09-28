/*
 * Port_Init.c
 *
 *  Created on: Mar 22, 2019
 *      Author: Wagih
 */


#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "Driver_can.h"
#include "Port_Init.h"

uint8_t Int_flag=0;

void  GPIOF_Handler(void)
{
    /*set of the handler/interrupt and to resest the flag*/

    GPIOIntClear(GPIO_PORTF_BASE,SWITCHES);
    Int_flag = SET;
}
/*
 * function name:
 * Parameters:
 *              Input:
 *              output:
 *              input/output:
 * return:
 * Discretion:
 *
 * Limitation:
 * */
void PORTF_init(void)
{
    /*Initialization of the port of the LEDs(Port F) */

SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
HWREG(GPIO_PORTF_BASE+ GPIO_O_CR) |= 0x01;
HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,SWITCHES);
GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE ,GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
GPIOPadConfigSet(GPIO_PORTF_BASE,SWITCHES,GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
GPIOIntTypeSet(GPIO_PORTF_BASE,SWITCHES,GPIO_FALLING_EDGE) ;
GPIOIntRegister(GPIO_PORTF_BASE,GPIOF_Handler);
GPIOIntEnable(GPIO_PORTF_BASE,SWITCHES);

UARTprintf("initialized in!!.\n");

}
