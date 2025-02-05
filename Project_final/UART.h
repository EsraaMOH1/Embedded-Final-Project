/*
 * UART.h
 *
 *  Created on: Dec 23, 2024
 *      Author: hager sharqawy
 */

#ifndef UART_H_

static void GPIO_SetupUART0Pins(void);
void UART0_Init(void);
void UART0_SendByte(unsigned char data);
void UART0_SendString(unsigned char *string);
unsigned char UART0_ReceiveByte(void);

#define UART_H_





#endif /* UART_H_ */
