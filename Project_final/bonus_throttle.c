#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "tm4c123gh6pm_registers.h"
#include "common_macros.h"
#include "bonus_throttle.h"

#define VREF 3.3     /* Reference voltage for ADC (3.3V) */
#define ADC_RESOLUTION 4096.0


void GPIO_SetupPE4AnalogPin(void)
{
    GPIO_PORTE_DIR_REG   &= ~(1<<3);              /* Configure PE4 as input pin */
    GPIO_PORTE_AMSEL_REG |= (1<<3);               /* Enable Analog at PE4 */
    GPIO_PORTE_AFSEL_REG |= (1<<3);               /* Enable alternative function at PE4 */
    GPIO_PORTE_PCTL_REG  &= 0xFFFF0FFF;           /* Set PMCx bits for PE4 with value 0 to use as analog pin */
    GPIO_PORTE_DEN_REG   &= ~(1<<3);              /* Disable Digital at PE4 */
}

void ADC0thrott_Init(void)
{
    SYSCTL_RCGCADC_REG |= (1<<0);                  /* Enable clock for ADC0 */
    while(GET_BIT(SYSCTL_PRADC_REG,0) == 0);       /* Wait until ADC0 clock is activated and it is ready for access*/

    ADC0_ACTSS_REG  &= ~(1<<3);                    /* Disable Sample Sequencer 3 */
    ADC0_EMUX_REG   &= ~0xF000;                    /* Sequencer 3 is triggered by software */
    ADC0_SSMUX3_REG = 0;                    /* Clear MUX0 field */
    ADC0_SSCTL3_REG = 0x06;            /* Set IE0 and END0 bits */
    ADC0_ACTSS_REG  |= (1<<3);                     /* Enable Sample Sequencer 3 */
}

unsigned long ADC0_Read_PE4(void) {
    ADC0_PSSI_REG = 0x08;           // Start a conversion sequence on SS3
    while((ADC0_RIS_REG & 0x08) == 0); // Wait for conversion to complete
    unsigned long result = ADC0_SSFIFO3_REG & 0xFFF; // Read the 12-bit result
    ADC0_ISC_REG |= (1<<3);            // Clear the interrupt flag
    return result;
}

float Get_ThrottlePosition(unsigned long adcValue ) {

    // Convert ADC value (0-4095) to throttle percentage (0-100%)
    float voltage = (adcValue * VREF) / ADC_RESOLUTION;
    return (voltage / VREF) * 100.0;  /* Scale to 0% - 100% */
}


//void Monitor_Throttle(void) {
//    float throttle = Get_ThrottlePosition();
//
//    char buffer[50];
//    snprintf(buffer, sizeof(buffer), "Throttle: %.1f%%\n", throttle);
//    UART0_SendString(buffer);
//
//    if (throttle < 5.0) {
//        ERROR_LOG(DTC_Throttle_Stuck_Min);
//        UART0_SendString(" Throttle Stuck at Minimum\n");
//    } else if (throttle > 95.0) {
//        ERROR_LOG(DTC_Throttle_Stuck_Max);
//        UART0_SendString(" Throttle Stuck at Maximum\n");
//    }
//}



