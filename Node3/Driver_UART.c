/*
 * Driver_UART.c
 *
 *  Created on: May 26, 2019
 *      Author: NADA
 */
#include <stdbool.h>
#include <stdint.h>
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\inc/hw_can.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\inc/hw_ints.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\inc/hw_memmap.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\driverlib/can.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\driverlib/gpio.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\driverlib/interrupt.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\driverlib/pin_map.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\driverlib/sysctl.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\driverlib/uart.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\utils/uartstdio.h"
#include "tm4c123gh6pm.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\inc/hw_types.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\inc/hw_memmap.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\inc/hw_gpio.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\driverlib/sysctl.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\driverlib/rom.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\driverlib/rom_map.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\driverlib/pin_map.h"
#include "E:\merged_partition_content\MCT\Spring 2019 (FINALLY!!)\Automotive_Embedded\final\driverlib/gpio.h"
#include "led_task.h"
#include "Driver_can.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "Driver_UART.h"

TaskHandle_t xGetDistanceHandle = 4;

xSemaphoreHandle UART_RXSem;

uint32_t Distance = 0;



void UARTInterruptHandler(void)
{
    uint32_t ui32Status;
    BaseType_t xHigherPriorityTaskWoken;

    //
    // Get the interrrupt status.
    //
    ui32Status = ROM_UARTIntStatus(UART0_BASE, true);

    if (ui32Status == UART_INT_RX)
    {
        while(UARTCharsAvail(UART0_BASE) != 1)
        {
        }

        xSemaphoreGiveFromISR(UART_RXSem, &xHigherPriorityTaskWoken);
    }

    /*else if (ui32Status == UART_INT_TX)
    {
        //xSemaphoreGiveFromISR(UART_RXSem, &xHigherPriorityTaskWoken);
        //print the value that is transmitted
    }*/

    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART0_BASE, ui32Status);

    UARTprintf("UART ISR in.\n");
    /* The xHigherPriorityTaskWoken parameter must be initialized to pdFALSE as
    it will get set to pdTRUE inside the interrupt safe API function if a
    context switch is required. */
    xHigherPriorityTaskWoken = pdFALSE;


    //xQueueSendToFrontFromISR( xButtonState, &ButtonPressed, &xHigherPriorityTaskWoken );


    UARTprintf("UART interrupt cleared.\n");


    /* Pass the xHigherPriorityTaskWoken value into portYIELD_FROM_ISR().  If
    xHigherPriorityTaskWoken was set to pdTRUE inside xSemaphoreGiveFromISR()
    then calling portYIELD_FROM_ISR() will request a context switch.  If
    xHigherPriorityTaskWoken is still pdFALSE then calling
    portYIELD_FROM_ISR() will have no effect.  The implementation of
    portYIELD_FROM_ISR() used by the Windows port includes a return statement,
    which is why this function does not explicitly return a value. */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


void InitConsole(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    //
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);


    //
    // Enable UART0 so that we can configure the clock.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    ROM_IntMasterEnable();

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Select the alternate (UART) function for these pins.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Enable the UART interrupt.
    //
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_TX );

    UARTFIFOEnable(UART0_BASE);

    UARTIntRegister(UART0_BASE, UARTInterruptHandler);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}



//*****************************************************************************
//
// This function prints some information about the CAN message to the
// serial port for information purposes only.
//
//*****************************************************************************
void PrintCANMessageInfo(tCANMsgObject *psCANMsg, uint32_t ui32MsgObj)
{
    /*Used to print a UART msg when it recievce/sends*/

    unsigned int uIdx;

    UARTprintf("Sending msg: obj=%d ID=0x%04X msg=0x", ui32MsgObj,
               psCANMsg->ui32MsgID);
    for(uIdx = 0; uIdx < psCANMsg->ui32MsgLen; uIdx++)
    {
        UARTprintf("%02X ", psCANMsg->pui8MsgData[uIdx]);
    }
    UARTprintf("\n");
}

static void
GetDistanceTask(void *pvParameters)
{
    while(1)
    {
        xSemaphoreTake(UART_RXSem, 0);
        Distance = UARTCharGet(UART0_BASE);
        UARTprintf("distance uart %d",Distance);
        SendDistance(Distance);
        //send distance to the API Function
    }

}

uint32_t UART_Init(void)
{
    InitConsole();

    if(xTaskCreate(GetDistanceTask, (const portCHAR *)"Token", 128, NULL,
                      5, &xGetDistanceHandle) != pdTRUE)
    {
        return(1);
    }

    return (0);
}


