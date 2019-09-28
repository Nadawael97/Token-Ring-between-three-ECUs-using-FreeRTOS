//*****************************************************************************
//
// led_task.c - A simple flashing LED task.
//
// Copyright (c) 2012-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "drivers/rgb.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"
#include "led_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
//#include "Port_Init.h"
#include "Driver_can.h"

//*****************************************************************************
//
// The stack size for the LED toggle task.
//
//*****************************************************************************
#define LEDTASKSTACKSIZE        128         // Stack size in words

//*****************************************************************************
//
// The item size and queue size for the LED message queue.
//
//*****************************************************************************
#define LED_ITEM_SIZE           sizeof(uint8_t)
#define LED_QUEUE_SIZE          5

//*****************************************************************************
//
// Default LED toggle delay value. LED toggling frequency is twice this number.
//
//*****************************************************************************
#define LED_TOGGLE_DELAY        500

//*****************************************************************************
//
// The queue that holds messages sent to the LED task.
//
//*****************************************************************************
xQueueHandle g_pLEDQueue;
xQueueHandle xRxQueue;


TaskHandle_t xNormalHandle = 2;
extern TaskHandle_t xStartupHandle = 1;

//
// [G, R, B] range is 0 to 0xFFFF per color.
//
static uint32_t g_pui32Colors[3] = { 0x0000, 0x0000, 0x0000 };
static uint8_t g_ui8ColorsIndx;

extern xSemaphoreHandle g_pUARTSemaphore;

//*****************************************************************************
//
// This task toggles the user selected LED at a user selected frequency. User
// can make the selections by pressing the left and right buttons.
//
//*****************************************************************************
static void
LEDTaskStartup(void *pvParameters)
{
    UARTprintf("task1");
    while(1)
    {
        //turn the blue led on for 1 sec
        UARTprintf("led on task1");

        GPIOPinWrite(GPIO_PORTF_BASE, BLUE_GPIO_PIN, BLUE_GPIO_PIN);
        //SimpleDelay();

        if(CANTokenTaskInit() != 0)
            {

            UARTprintf("can task");

                while(1)
                {
                }
            }
        UARTprintf("can task init");

    }
}

static void
LEDTaskNormal(void *pvParameters)
{

   portTickType ui32WakeTime;
   uint32_t ui32LEDToggleDelay;
   ui32LEDToggleDelay = 1000;
   uint8_t RxBuffer[1];
   if (xQueueReceive(xRxQueue, &RxBuffer, portMAX_DELAY))
   {
   while(1){

       GPIOPinWrite(GPIO_PORTF_BASE, BLUE_GPIO_PIN, 0x00);
       GPIOPinWrite(GPIO_PORTF_BASE, GREEN_GPIO_PIN, GREEN_GPIO_PIN);
       ui32WakeTime = xTaskGetTickCount();
       vTaskDelayUntil(&ui32WakeTime, ui32LEDToggleDelay / portTICK_RATE_MS);
       GPIOPinWrite(GPIO_PORTF_BASE, GREEN_GPIO_PIN, 0x00);
       ui32WakeTime = xTaskGetTickCount();
       vTaskDelayUntil(&ui32WakeTime, ui32LEDToggleDelay / portTICK_RATE_MS);

   }
   }

}

//*****************************************************************************
//
// Initializes the LED task.
//
//*****************************************************************************
uint32_t
LEDTaskInit(void)
{
    PORTF_init();
    UARTprintf("initialized!!.\n");


    //
    // Create the LED tasks.
    //
    if(xTaskCreate(LEDTaskStartup, (const portCHAR *)"Blue", LEDTASKSTACKSIZE, NULL,
                  2, &xStartupHandle) != pdTRUE)
    {
        return(1);
    }
    if(xTaskCreate(LEDTaskNormal, (const portCHAR *)"Green", LEDTASKSTACKSIZE, NULL,
                      4, &xNormalHandle) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}
