/*
 * Driver_UART.h
 *
 *  Created on: May 30, 2019
 *      Author: NADA
 */

#ifndef DRIVER_UART_H_
#define DRIVER_UART_H_

#define RESET 0U

#define num_of_IDs 2U

extern void InitConsole(void);
extern void PrintCANMessageInfo(tCANMsgObject *psCANMsg, uint32_t ui32MsgObj);



#endif /* DRIVER_UART_H_ */
