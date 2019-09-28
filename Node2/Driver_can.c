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
TaskHandle_t xReceiveDistHandle = 5;

//TaskHandle_t xReceiveTokenHandle = 4;
xSemaphoreHandle can_SendSem;
xSemaphoreHandle can_ReceiveSem;
//xSemaphoreHandle can_RxDistSem;

xQueueHandle xRxQueue;
xQueueHandle can_RxDistQueue;

uint8_t g_pui8Msg1[8];
uint8_t g_pui8Msg2[8];
uint8_t g_pui8Msg3[8];
uint8_t g_pui8Msg4[8];
uint8_t g_pui8Msg5[8];




tCANMsgObject g_sCANMsgObject1;
tCANMsgObject g_sCANMsgObject2;
tCANMsgObject g_sCANMsgObject3;
tCANMsgObject g_sCANMsgObject4;
tCANMsgObject g_sCANMsgObject5;

tCANMsgObject sCANMessage;

volatile bool g_bErrFlag =0;

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
    else if(ui32Status == 3)
    {
        CANIntClear(CAN0_BASE, 3);
        UARTprintf("interrupt clearedDistance.\n");

        /* 'Give' the semaphore to unblock the task. */
        xQueueGiveFromISR(can_RxDistQueue, &xHigherPriorityTaskWoken);
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
        g_sCANMsgObject1.ui32MsgID = 0x2001;
        g_sCANMsgObject1.ui32MsgIDMask = 0xfffff;
        g_sCANMsgObject1.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
        g_sCANMsgObject1.ui32MsgLen = sizeof(g_pui8Msg1);
        g_sCANMsgObject1.pui8MsgData = g_pui8Msg1;


        g_sCANMsgObject2.ui32MsgID = 0x1001;
        g_sCANMsgObject2.ui32MsgIDMask = 0xfffff;
        g_sCANMsgObject2.ui32Flags = MSG_OBJ_RX_INT_ENABLE;
        g_sCANMsgObject2.ui32MsgLen = 8;


        g_sCANMsgObject3.ui32MsgID = 0x3001;
        g_sCANMsgObject3.ui32MsgIDMask = 0xfffff;
        g_sCANMsgObject3.ui32Flags = MSG_OBJ_RX_INT_ENABLE;
        g_sCANMsgObject3.ui32MsgLen = 8;

        g_sCANMsgObject4.ui32MsgID = 0x1001;
        g_sCANMsgObject4.ui32MsgIDMask = 0xfffff;
        g_sCANMsgObject4.ui32Flags = MSG_OBJ_RX_INT_ENABLE;
        g_sCANMsgObject4.ui32MsgLen = 8;

        g_sCANMsgObject5.ui32MsgID = 0x1001;
        g_sCANMsgObject5.ui32MsgIDMask = 0xfffff;
        g_sCANMsgObject5.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
        g_sCANMsgObject5.ui32MsgLen = 8;


        CANMessageSet(CAN0_BASE, 2, &g_sCANMsgObject2, MSG_OBJ_TYPE_RX);
        CANMessageSet(CAN0_BASE, 3, &g_sCANMsgObject3, MSG_OBJ_TYPE_RX);
        CANMessageSet(CAN0_BASE, 4, &g_sCANMsgObject4, MSG_OBJ_TYPE_RX);
        CANMessageSet(CAN0_BASE, 5, &g_sCANMsgObject5, MSG_OBJ_TYPE_TX);
        CANMessageSet(CAN0_BASE, 1, &g_sCANMsgObject1, MSG_OBJ_TYPE_TX);

}

/*static void
ReceiveTokenTask(void *pvParameters)
{
    xSemaphoreTake(can_ReceiveSem, 0);
    UARTprintf("semaphore is empty.\n");



}*/

static void
SendTokenTask(void *pvParameters)
{
    /*portTickType ui32WakeTime;
       uint32_t ui32LEDToggleDelay;
       ui32LEDToggleDelay = 2000;
       ui32WakeTime = xTaskGetTickCount();*/
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

static void
ReceiveDistanceTask(void *pvParameters)
{
    //xSemaphoreTake(can_RxDistSem,0);
    uint8_t RxBuffer[8];
    if (xQueueReceive(can_RxDistQueue, &RxBuffer, portMAX_DELAY))
    {
    while(1){

        CANMessageGet(CAN0_BASE, 3, &g_sCANMsgObject3, 0);
        UARTprintf("Dist received %d", g_sCANMsgObject3.pui8MsgData[0]);
        //send the data to a receive indication function

    }
    }
}

uint32_t
CANReceiveInit(void)
{
    CanInt();
    if(xTaskCreate(ReceiveDistanceTask, (const portCHAR *)"Token", 128, NULL,
                  5, &xReceiveDistHandle) != pdTRUE)
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
