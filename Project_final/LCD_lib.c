/*
 * LCD_lib.c
 *
 *  Created on: 13 Dec 2024
 *      Author: abutalb
 *
 */
#include "stdio.h"
#include "common_macros.h"
#include "LCD_lib.h"
#include "delay.h"
#include "tm4c123gh6pm_registers.h"


static const int command = 0;
static const int data = 1;

static void lcd_send(int value, int type){

    GPIO_PORTB_DATA_REG = value;


    if (type == command){
        CLEAR_BIT(GPIO_PORTA_DATA_REG,2);
    }
    else{
        SET_BIT(GPIO_PORTA_DATA_REG,2);
    }
    Delay_MS(1);

    SET_BIT(GPIO_PORTA_DATA_REG,3);
    Delay_MS(1);
    CLEAR_BIT(GPIO_PORTA_DATA_REG,3);


}


static void set_address(int row, int col) {
    int address = 0;

        if (row == 1){
            address = 0x80 + (col - 1);
        }
        else if (row == 2){ // Second row
            address = 0xC0 + (col -1 );
        }
        else if (row == 3){// Third row
            address = 0x90 + (col - 1);
        }
        else {
            address = 0xD0 + (col - 1);
        }

    lcd_send(address, command);
    Delay_MS(2);
}



static void pins_config(){


    /* Disable Analog */
    GPIO_PORTB_AMSEL_REG = 0x00;
    CLEAR_BIT(GPIO_PORTA_AMSEL_REG,2);
    CLEAR_BIT(GPIO_PORTA_AMSEL_REG,3);

    /*Set pins to GPIO */
    GPIO_PORTB_AFSEL_REG = 0X00;
    CLEAR_BIT(GPIO_PORTA_AFSEL_REG,2);
    CLEAR_BIT(GPIO_PORTA_AFSEL_REG,3);

    /* Clear PMCx bits to use it as GPIO pin */
    GPIO_PORTB_PCTL_REG  = 0x00000000;
    GPIO_PORTA_PCTL_REG &= 0xFFFF00FF;

    /* Set pins to output */
    GPIO_PORTB_DIR_REG = 0xFF;
    SET_BIT(GPIO_PORTA_DIR_REG,2);
    SET_BIT(GPIO_PORTA_DIR_REG,3);

    /*Enable digital*/
    GPIO_PORTB_DEN_REG = 0xFF;
    SET_BIT(GPIO_PORTA_DEN_REG,2);
    SET_BIT(GPIO_PORTA_DEN_REG,3);

    /*Clear Data bits*/
    GPIO_PORTB_DATA_REG = 0x00;
    CLEAR_BIT(GPIO_PORTA_DATA_REG,2);
    CLEAR_BIT(GPIO_PORTA_DATA_REG,3);
}


void lcd_init(void){    //lcd init

    pins_config();

    Delay_MS(20);

    lcd_send(0x30, command);
    Delay_MS(2);

    lcd_send(0x30, command);
    Delay_MS(2);

    lcd_send(0x30, command);
    Delay_MS(2);

    lcd_send(0x3C, command);
    Delay_MS(2);

    lcd_send(0x00, command);
    Delay_MS(5);

    lcd_send(0x01, command);
    Delay_MS(5);

    lcd_send(0x06, command);
    Delay_MS(5);

}

void lcd_cmd(unsigned int cmd){

    int value = 0;

    switch (cmd){

    case LCD_CLEAR:
            lcd_send(0x01, command);
            Delay_MS(3);
        break;

    case LCD_CURSOR_OFF:
        value = 0x0c;
        break;

    case LCD_UNDERLINE_ON:
        value = 0x0e;
        break;


    default:
        value = 0xc0;
    }

    if (LCD_CLEAR != cmd){
        lcd_send(value, command);
        Delay_MS(2);
    }

}

void lcd_char_cp(char out_char){
    lcd_send(out_char, data);
    Delay_MS(10);
}
void lcd_char(int row, int col, char out_char){
    set_address(row,col);
    lcd_char_cp(out_char);
}

void lcd_text_cp(char *text){

    while (*text != '\0'){
        lcd_char_cp(*text);
        text++;
    }
}

void lcd_text(int row, int col, char *text){
    set_address(row,col);
    lcd_text_cp(text);
}



void ftoa(float num, char *str, int precision) {
    int intPart = (int)num;  // Extract integer part
    float fractionalPart = num - intPart;  // Extract fractional part
    char *ptr = str;  // Pointer to the beginning of the string

    // Convert integer part to string
    while (intPart > 0) {
        *ptr++ = '0' + (intPart % 10);  // Store the last digit and move the pointer
        intPart /= 10;  // Remove the last digit
    }

    // Reverse the string for the integer part
    char *start = str;
    char *end = ptr - 1;
    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }

    // Add decimal point if precision is greater than 0
    if (precision > 0) {
        *ptr++ = '.';  // Add decimal point
        fractionalPart *= 10;  // Multiply fractional part by 10 for each decimal place

        // Convert fractional part to string
        int i = 0;
        for (i = 0; i < precision; i++) {
            int digit = (int)fractionalPart;
            *ptr++ = '0' + digit;  // Convert digit to character
            fractionalPart -= digit;  // Remove the integer part
            fractionalPart *= 10;  // Move to the next decimal place
        }
    }

    *ptr = '\0';  // Null-terminate the string
}



