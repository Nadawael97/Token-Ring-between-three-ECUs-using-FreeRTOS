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
extern tCANMsgObject sCANMessage;

//*****************************************************************************
//
// A counter that keeps track of the number of times the TX interrupt has
// occurred, which should match the number of TX messages that were sent.
//
//*****************************************************************************
extern volatile uint32_t g_ui32IntCount;
//*****************************************************************************
//
// Counters that are used to count the number of messages on each of the
// three message objects that are used in this example.
//
//*****************************************************************************
extern volatile uint32_t g_ui32Msg1Count;
extern volatile uint32_t g_ui32Msg2Count;
extern volatile uint32_t g_ui32Msg3Count;

//*****************************************************************************
//
// A flag to indicate that CAN controller message object 3 has sent a message.
//
//*****************************************************************************
extern volatile bool g_bMsgObj3Sent;


//*****************************************************************************
//
// A flag to indicate that some transmission error occurred.
//
//*****************************************************************************
extern volatile bool g_bErrFlag;

//*****************************************************************************
//
// Message buffers that hold the contents of the 4 different messages that
// are being transmitted.  Each one is a different length.
//
//*****************************************************************************
extern uint8_t g_pui8Msg3[6];
extern uint8_t g_pui8Msg4[8];

//*****************************************************************************
//
// This function sets up UART0 to be used for a console to display information
// as the example is running.
//
//*****************************************************************************
#define RESET 0U
#define SET 1U
#define SWITCHES GPIO_PIN_0|GPIO_PIN_4
#define num_of_IDs 2U


extern uint8_t Int_flag;
extern uint8_t i;


extern TaskHandle_t xSendTokenHandle;

/*Declaration of Functions used */

extern void InitConsole(void);
extern void PrintCANMessageInfo(tCANMsgObject *psCANMsg, uint32_t ui32MsgObj);
extern void SimpleDelay(void);
extern void CANIntHandler(void);
extern void CanInt(void);
extern uint32_t CANTokenTaskInit(void);


#endif /* DRIVER_CAN_H_ */
