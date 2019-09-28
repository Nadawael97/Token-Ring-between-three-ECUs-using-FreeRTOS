#ifndef DRIVER_UART_H_
#define DRIVER_UART_H_
//*****************************************************************************
//
// This function sets up UART0 to be used for a console to display information
// as the example is running.
//
//*****************************************************************************
#define RESET 0U


#define num_of_IDs 2U

extern xSemaphoreHandle UART_RXSem;
extern uint32_t Distance;


extern void InitConsole(void);
extern void PrintCANMessageInfo(tCANMsgObject *psCANMsg, uint32_t ui32MsgObj);
extern uint32_t UART_Init(void);

#endif
