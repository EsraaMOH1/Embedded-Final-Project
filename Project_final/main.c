#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "tm4c123gh6pm_registers.h"
#include "common_macros.h"
#include "delay.h"
#include "pushbutton_lib.h"
#include "LCD_lib.h"
#include "ultara.h"
#include "temperature.h"
#include "EEPROM.h"
#include "UART.h"
//#include "bonus_throttle.h"

#define EEPROM_EESUPP_REG           (*((volatile unsigned long *)0x400AF01C))
#define EEDONE_REG                  (*((volatile unsigned long *)0x400AF018))
#define EEPROM_EEBLOCK_REG          (*((volatile unsigned long *)0x400AF004))
#define EEPROM_EEOFFSET_REG         (*((volatile unsigned long *)0x400AF008))
#define EEPROM_EERDWR_REG           (*((volatile unsigned long *)0x400AF010))

#define MAX_OFFSET 15
#define MAX_BLOCK 31

#define Enable_Exceptions()    __asm(" CPSIE I ")
#define Disable_Exceptions()   __asm(" CPSID I ")
#define Enable_Faults()        __asm(" CPSIE F ")
#define Disable_Faults()       __asm(" CPSID F ")


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

#define DTC_ACCIDENT_MIGHT_HAPPENED  "P001"
#define DTC_ENGINE_HIGH_TEMP         "P002"
#define DTC_Throttle_Stuck_Min       "P003"
#define DTC_Throttle_Stuck_Max       "P004"



int main(void){

    unsigned long adcValue; // Variable to store the ADC result
    unsigned long throttle_value;
    float temperature;            // Variable to store the calculated temperature
    char buffer2[16] ;
    char buffer1[16] ;
    volatile float distance = 0;
    volatile float throttle=0;
    volatile int i;
    volatile int accidentLogged=0;


    //Enabling the system clock
    SYSCTL_RCGCGPIO_REG |= (1<<0);
    while (GET_BIT(SYSCTL_PRGPIO_REG, 0) == 0);

    SYSCTL_RCGCGPIO_REG |= (1<<1);
    while (GET_BIT(SYSCTL_PRGPIO_REG, 1) == 0);

    SYSCTL_RCGCGPIO_REG |= (1<<2);
    while (GET_BIT(SYSCTL_PRGPIO_REG, 2) == 0);

    SYSCTL_RCGCGPIO_REG |= (1<<3);
    while (GET_BIT(SYSCTL_PRGPIO_REG, 3) == 0);

    SYSCTL_RCGCGPIO_REG |= (1<<4);
    while (GET_BIT(SYSCTL_PRGPIO_REG, 4) == 0);

    SYSCTL_RCGCGPIO_REG |= (1<<5);
    while (GET_BIT(SYSCTL_PRGPIO_REG, 5) == 0);

    SysTick_Init();
    Enable_Exceptions();
    Enable_Faults();
    UART0_Init();
    UART0_SendString("UART Initialized Successfully\n");
    EEPROM_init();

    GPIO_SetupPE2AnalogPin();
    ADC0_Init();
    lcd_init();
    pushbutton_init();
    motor_init();
    HCSR04_Init();

//    GPIO_SetupPE4AnalogPin();
//    ADC0thrott_Init();

    SET_BIT(GPIO_PORTC_DATA_REG,4);
    SET_BIT(GPIO_PORTD_DATA_REG,6);
    char command;

    while(1){
        UART0_SendString("\r\n");
        if (!(UART0_FR_REG & UART_FR_RXFE_MASK))
        {
             command = UART0_ReceiveByte();

        }
        switch(command){
           case '1':
               UART0_SendString("Operation cycle started\n");

               lcd_text(1,1,"WINDOW 1:");
               lcd_text(2,1,"WINDOW 2:");
               lcd_text(3,1,"TEMP:");
               lcd_text(4,1,"Distance: ");

               distance = HCSR04_GetDistance();
               adcValue = ADC0_ReadSeq3();
               temperature = GetTemperature(adcValue);
//               throttle_value=ADC0_Read_PE4();
//               throttle=Get_ThrottlePosition(throttle_value);

               ftoa(temperature , buffer2, 1);
               lcd_text(3,11, buffer2);

               ftoa(distance , buffer1, 4);
               lcd_text(4,11, buffer1);


               // Check for faults with the parking distance
               if (distance < 10 && (accidentLogged==0)) {
                      ERROR_LOG(DTC_ACCIDENT_MIGHT_HAPPENED);
                      accidentLogged=1;
               }
               else if(distance>=10){
                   accidentLogged=0;
               }

               // Check for faults with the engine temperature
               if (temperature > 25) {
                   ERROR_LOG(DTC_ENGINE_HIGH_TEMP);
               }

              //  Check for faults with the throttle position
//               if (throttle < 5.0) {
//                   ERROR_LOG(DTC_Throttle_Stuck_Min);
//                   UART0_SendString(" Throttle Stuck at Minimum\n");
//               } else if (throttle > 300.0) {
//                   ERROR_LOG(DTC_Throttle_Stuck_Max);
//                   UART0_SendString(" Throttle Stuck at Maximum\n");
//               }



                if (GET_BIT(GPIO_PORTA_DATA_REG,4) == 0){

                    if (GET_BIT(GPIO_PORTA_DATA_REG,5) == 1){
                    lcd_text(1,10,"CLOSED");

                    CLEAR_BIT(GPIO_PORTD_DATA_REG,2);
                    SET_BIT(GPIO_PORTD_DATA_REG,3);

                    }
                }
                else if (GET_BIT(GPIO_PORTA_DATA_REG,5) == 0) {
                    if (GET_BIT(GPIO_PORTA_DATA_REG,4) == 1){
                    lcd_text(1,10,"OPENED");

                    CLEAR_BIT(GPIO_PORTD_DATA_REG,3);
                    SET_BIT(GPIO_PORTD_DATA_REG,2);
                    }
                }
                else{
                    CLEAR_BIT(GPIO_PORTD_DATA_REG,3);
                    CLEAR_BIT(GPIO_PORTD_DATA_REG,2);

                }

                if (GET_BIT(GPIO_PORTA_DATA_REG,6) == 0){

                    if (GET_BIT(GPIO_PORTA_DATA_REG,7) == 1){
                    lcd_text(2,10,"OPENED");

                    CLEAR_BIT(GPIO_PORTC_DATA_REG,7);
                    SET_BIT(GPIO_PORTC_DATA_REG,6);

                    }
                }
                else if (GET_BIT(GPIO_PORTA_DATA_REG,7) == 0) {
                    if (GET_BIT(GPIO_PORTA_DATA_REG,6) == 1){
                    lcd_text(2,10,"CLOSED");

                    CLEAR_BIT(GPIO_PORTC_DATA_REG,6);
                    SET_BIT(GPIO_PORTC_DATA_REG,7);
                    }
                }
                else{
                    CLEAR_BIT(GPIO_PORTC_DATA_REG,7);
                    CLEAR_BIT(GPIO_PORTC_DATA_REG,6);

                }

               break;
           case'2':
               UART0_SendString("Retrieving and displaying logged errors\n");
               EEPROM_Retrieve();
               command=1;
               break;
           case '3':
               UART0_SendString("Closing the operation cycle\n");
               return;
           default:
               UART0_SendString("Invalid command\n");
               break;
           }
       }


}

