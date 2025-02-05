/*
 * pushbutton_lib.c
 *
 *  Created on: 14 Dec 2024
 *      Author: abutalb
 */

#include "tm4c123gh6pm_registers.h"
#include "common_macros.h"
#include "pushbutton_lib.h"

void pushbutton_init(void){

    GPIO_PORTA_PCTL_REG &= 0x0000FFFF;
    GPIO_PORTA_AMSEL_REG &= ~0xF0;
    GPIO_PORTA_DIR_REG   &= ~0xF0;
    GPIO_PORTA_AFSEL_REG &= ~0xF0;
    GPIO_PORTA_PUR_REG   |=  0xF0;
    GPIO_PORTA_DEN_REG   |=  0xF0;

}

void motor_init(void){
    /*  congfig MOTOR 1 direction pins   */

    GPIO_PORTD_PCTL_REG  &= 0x00000000;
    GPIO_PORTD_AMSEL_REG &= ~0x4F;
    GPIO_PORTD_DIR_REG   |= 0x4F;
    GPIO_PORTD_AFSEL_REG &= ~0x4F;
    GPIO_PORTD_DEN_REG   |=  0x4F;
    GPIO_PORTD_DATA_REG  &= ~0x4F;

    GPIO_PORTC_PCTL_REG  &= 0xFFF0FFFF;
    CLEAR_BIT(GPIO_PORTC_AMSEL_REG,4);
    SET_BIT(GPIO_PORTC_DIR_REG,4);
    CLEAR_BIT(GPIO_PORTC_AFSEL_REG,4);
    SET_BIT(GPIO_PORTC_DEN_REG,4);
    CLEAR_BIT(GPIO_PORTC_DATA_REG,4);

    GPIO_PORTC_PCTL_REG  &= 0x00FFFFFF;
    GPIO_PORTC_AMSEL_REG &= ~0xC0;
    GPIO_PORTC_DIR_REG   |= 0xC0;
    GPIO_PORTC_AFSEL_REG &= ~0xC0;
    GPIO_PORTC_DEN_REG   |=  0xC0;
    GPIO_PORTC_DATA_REG  &= ~0xC0;

}




