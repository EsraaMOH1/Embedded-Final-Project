#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "UART.h"

#include "tm4c123gh6pm_registers.h"
#include "common_macros.h"

#define UART_DATA_5BITS  0x0
#define UART_DATA_6BITS  0x1
#define UART_DATA_7BITS  0x2
#define UART_DATA_8BITS  0x3
#define UART_LCRH_WLEN_BITS_POS 5
#define UART_CTL_UARTEN_MASK  0x00000001
#define UART_CTL_TXE_MASK     0x00000100
#define UART_CTL_RXE_MASK     0x00000200
#define UART_FR_TXFE_MASK    0x00000080
#define UART_FR_RXFE_MASK    0x00000010



static void GPIO_SetupUART0Pins(void){
    SYSCTL_RCGCGPIO_REG |=0x01;
    while(GET_BIT(SYSCTL_RCGCGPIO_REG,0)==0);

    GPIO_PORTA_AMSEL_REG &=~ 0x03;
    GPIO_PORTA_DIR_REG &=0xFE;
    GPIO_PORTA_DIR_REG |=0x02;
    GPIO_PORTA_AFSEL_REG |=0x03;
    GPIO_PORTA_PCTL_REG =(GPIO_PORTA_PCTL_REG & 0xFFFFFF00) | 0x00000011;
    GPIO_PORTA_DEN_REG |=0x03;
}

void UART0_Init(void){
    GPIO_SetupUART0Pins();

    SYSCTL_RCGCUART_REG |=0x01;
    while (!(SYSCTL_PPUART_REG & 0x01));

    UART0_CTL_REG=0;

    UART0_CC_REG=0x0;

    UART0_IBRD_REG=104;
    UART0_FBRD_REG=11;
    UART0_IM_REG&= ~(0x10); //disabling RX interrupt

    UART0_LCRH_REG=(UART_DATA_8BITS<< UART_LCRH_WLEN_BITS_POS);

    UART0_CTL_REG =UART_CTL_UARTEN_MASK |UART_CTL_TXE_MASK |UART_CTL_RXE_MASK;

}


void UART0_SendByte(unsigned char data)
{
    while(!(UART0_FR_REG & UART_FR_TXFE_MASK)); /* Wait until the transmit FIFO is empty */
    UART0_DR_REG = data; /* Send the byte */
}

void UART0_SendString(unsigned char *string){
    while(*string){
        UART0_SendByte(*string++);
    }
}
unsigned char UART0_ReceiveByte(void)
{
    while(UART0_FR_REG & UART_FR_RXFE_MASK); /* Wait until the receive FIFO is not empty */
    unsigned char data= (UART0_DR_REG &0xFF); /* Read the byte */
    UART0_SendString("Received: ");
    UART0_SendByte(data); // Echo the received byte back
    UART0_SendString("\n");
    return data;
}




