/*
 * Driver_can.c
 *
 *  Created on: Mar 22, 2019
 *      Author: Mark
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

TaskHandle_t xSendTokenHandle = 3;

//TaskHandle_t xReceiveTokenHandle = 4;
xSemaphoreHandle can_SendSem;
xSemaphoreHandle can_ReceiveSem;
xQueueHandle xRxQueue;


uint8_t g_pui8Msg1[8];
uint8_t g_pui8Msg2[8];

tCANMsgObject g_sCANMsgObject1;
tCANMsgObject g_sCANMsgObject2;
tCANMsgObject g_sCANMsgObject3;
tCANMsgObject sCANMessage;

volatile uint32_t g_ui32IntCount = 0;

volatile uint32_t g_ui32Msg1Count = 0;
volatile uint32_t g_ui32Msg2Count = 0;
volatile uint32_t g_ui32Msg3Count = 0;

volatile bool g_bMsgObj3Sent = 0;

volatile bool g_bErrFlag =0;


uint8_t g_pui8Msg3[6] = { 3, 3, 3, 3, 3, 3 };
uint8_t g_pui8Msg4[8] = { 4, 4, 4, 4, 5, 5, 5, 5 };

uint8_t Int_flag=0;


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


void SimpleDelay(void)
{
    /*System clock delay settings for 1 second*/

    SysCtlDelay(16000000 / 3);
}


static uint32_t
CANInterruptHandler( void )
{
    uint32_t ui32Status;
    BaseType_t xHigherPriorityTaskWoken;


    ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);


    if(ui32Status == CAN_INT_INTID_STATUS)
    {

        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);


        g_bErrFlag = 1;


        UARTprintf("error");
    }


    else if(ui32Status == 1)
    {
        CANIntClear(CAN0_BASE, 1);
        UARTprintf("reception.\n");
        xQueueSendFromISR(xRxQueue, 1, &xHigherPriorityTaskWoken);
    }
    else if(ui32Status == 2)
    {
        CANIntClear(CAN0_BASE, 2);
        UARTprintf("interrupt clearedTransmission.\n");

        /* 'Give' the semaphore to unblock the task. */
        xSemaphoreGiveFromISR(can_SendSem, &xHigherPriorityTaskWoken);
    }
    else
    {

    }


    UARTprintf("ISR in.\n");
    /* The xHigherPriorityTaskWoken parameter must be initialized to pdFALSE as
    it will get set to pdTRUE inside the interrupt safe API function if a
    context switch is required. */
    xHigherPriorityTaskWoken = pdFALSE;


    //xQueueSendToFrontFromISR( xButtonState, &ButtonPressed, &xHigherPriorityTaskWoken );


    UARTprintf("interrupt cleared.\n");


    /* Pass the xHigherPriorityTaskWoken value into portYIELD_FROM_ISR().  If
    xHigherPriorityTaskWoken was set to pdTRUE inside xSemaphoreGiveFromISR()
    then calling portYIELD_FROM_ISR() will request a context switch.  If
    xHigherPriorityTaskWoken is still pdFALSE then calling
    portYIELD_FROM_ISR() will have no effect.  The implementation of
    portYIELD_FROM_ISR() used by the Windows port includes a return statement,
    which is why this function does not explicitly return a value. */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


void CanInt(void)
{
    /*Intiliazation of the port clock and the IDs of the Objects that will be recieved and send */

           SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

           GPIOPinConfigure(GPIO_PB4_CAN0RX);
           GPIOPinConfigure(GPIO_PB5_CAN0TX);
           GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);

           SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

           /*Enable the CAN for operation.*/
           CANInit(CAN0_BASE);
           CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

           /*Enable the CAN interrupt on the processor (NVIC).*/
           IntEnable(INT_CAN0);

           CANIntRegister(CAN0_BASE,CANInterruptHandler);
           CANEnable(CAN0_BASE);

           g_pui8Msg1[0U]=0;
           g_sCANMsgObject1.ui32MsgID = 0x1001;
           g_sCANMsgObject1.ui32MsgIDMask = 0xfffff;
           g_sCANMsgObject1.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
           g_sCANMsgObject1.ui32MsgLen = sizeof(g_pui8Msg1);
           g_sCANMsgObject1.pui8MsgData = g_pui8Msg1;


           g_sCANMsgObject2.ui32MsgID = 0x3001;
           g_sCANMsgObject2.ui32MsgIDMask = 0xfffff;
           g_sCANMsgObject2.ui32Flags = MSG_OBJ_RX_INT_ENABLE;
           g_sCANMsgObject2.ui32MsgLen = 8;

           CANMessageSet(CAN0_BASE, 2, &g_sCANMsgObject2, MSG_OBJ_TYPE_RX);
           CANMessageSet(CAN0_BASE, 1, &g_sCANMsgObject1, MSG_OBJ_TYPE_TX);

}

/*static void
ReceiveTokenTask(void *pvParameters)
{
    xSemaphoreTake(can_ReceiveSem, 0);
    UARTprintf("semaphore is empty.\n");   khfytcfygf


}*/

static void
SendTokenTask(void *pvParameters)
{
    /*portTickType ui32WakeTime;
       uint32_t ui32LEDToggleDelay;
       ui32LEDToggleDelay = 3000;*/
       /*ui32WakeTime = xTaskGetTickCount();*/
       while(1)
       {

           /*vTaskDelayUntil(&ui32WakeTime, ui32LEDToggleDelay / portTICK_RATE_MS);*/
           CANMessageSet(CAN0_BASE, 1, &g_sCANMsgObject1, MSG_OBJ_TYPE_TX);
           xSemaphoreTake(can_SendSem, 0);
           UARTprintf("semaphore is empty.\n");
           //vTaskDelete(&xStartupHandle);
           //vTaskDelete(&xSendTokenHandle);
       }
}


uint32_t
CANTokenTaskInit(void)
{
    CanInt();
    if(xTaskCreate(SendTokenTask, (const portCHAR *)"Token", 128, NULL,
                      3, &xSendTokenHandle) != pdTRUE)
    {
        return(1);
    }
    /*if(xTaskCreate(ReceiveTokenTask, (const portCHAR *)"Token", 128, NULL,
                      2, &xReceiveTokenHandle) != pdTRUE)
    {
        return(1);
    }*/

    return (0);

}
