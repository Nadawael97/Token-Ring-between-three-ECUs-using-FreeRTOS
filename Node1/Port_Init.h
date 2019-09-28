/*
 * Port_Init.h
 *
 *  Created on: Mar 22, 2019
 *      Author: Wagih
 */

#ifndef PORT_INIT_H_
#define PORT_INIT_H_

#define RED_GPIO_PIN            GPIO_PIN_1
#define BLUE_GPIO_PIN           GPIO_PIN_2
#define GREEN_GPIO_PIN          GPIO_PIN_3
#define WHITE_GPIO_PIN GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3

extern void PORTF_init(void);
extern void GPIOF_Handler(void);


#endif /* PORT_INIT_H_ */
