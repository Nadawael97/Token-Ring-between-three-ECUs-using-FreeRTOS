/*
 * Driver_can.h
 *
 *  Created on: Mar 22, 2019
 *      Author: Wagih
 */

#ifndef DRIVER_CAN_H_
#define DRIVER_CAN_H_

#include "driverlib/can.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/*Declaration of the msgs and objects */

extern uint8_t g_pui8Msg1[8];
extern uint8_t g_pui8Msg2[8];
extern uint8_t g_pui8Msg3[8];
extern uint8_t g_pui8Msg4[8];
extern uint8_t g_pui8Msg5[8];

//*****************************************************************************
//
// CAN message objects that will hold the separate CAN messages.  These could
// also be allocated on the stack but be careful because these structures
// each take about 20 bytes.
//
//*****************************************************************************
extern tCANMsgObject g_sCANMsgObject1;
extern tCANMsgObject g_sCANMsgObject2;
extern tCANMsgObject g_sCANMsgObject3;
extern tCANMsgObject g_sCANMsgObject4;
extern tCANMsgObject g_sCANMsgObject5;

extern tCANMsgObject sCANMessage;

//*****************************************************************************
//
// A flag to indicate that some transmission error occurred.
//
//*****************************************************************************
extern volatile bool g_bErrFlag;


extern uint8_t i;

extern TaskHandle_t xSendTokenHandle;
extern TaskHandle_t xSendDistanceHandle;


extern xSemaphoreHandle can_SendSem;
extern xSemaphoreHandle can_SendDistSem;
extern xSemaphoreHandle can_ReceiveSem;
extern xQueueHandle xRxQueue;



/*Declaration of Functions used */

extern void SimpleDelay(void);
extern void CANIntHandler(void);
extern void CanInt(void);
extern uint32_t CANTokenTaskInit(void);
extern void SendDistance(uint32_t );


#endif /* DRIVER_CAN_H_ */
