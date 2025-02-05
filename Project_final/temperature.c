/*
 * tempurature.c
 *
 *  Created on: Dec 22, 2024
 *      Author: ASUS 030T
 */
#include "tm4c123gh6pm_registers.h"
#include "common_macros.h"
#include "temperature.h"

#define VREF 3.3
#define ADC_RESOLUTION 4096.0     /* 12-bit ADC resolution */
#define LM35_SCALE 0.01           /* LM35 conversion scale is 10mV per degree Celsius */
#define SCALE_FACTOR (VREF / (ADC_RESOLUTION * LM35_SCALE)) /* Scale factor for temperature */

void GPIO_SetupPE2AnalogPin(void)
{
    GPIO_PORTE_PCTL_REG  &= 0xFFFFF0FF;           /* Set PMCx bits for PE2 with value 0 to use as analog pin */

    GPIO_PORTE_DIR_REG   &= ~(1<<2);              /* Configure PE2 as input pin */
    GPIO_PORTE_AMSEL_REG |= (1<<2);               /* Enable Analog at PE2 */
    GPIO_PORTE_AFSEL_REG |= (1<<2);               /* Enable alternative function at PE2 */

    GPIO_PORTE_DEN_REG   &= ~(1<<2);              /* Disable Digital at PE2 */
}

void ADC0_Init(void)
{
    SYSCTL_RCGCADC_REG |= (1<<0);                  /* Enable clock for ADC0 */
    while(GET_BIT(SYSCTL_PRADC_REG,0) == 0);       /* Wait until ADC0 clock is activated and it is ready for access*/

    ADC0_SSPRI_REG   = 0x0123;                     /* Set priority for all four Sequencers and Sequencer 3 is the highest priority */
    ADC0_ACTSS_REG  &= ~(1<<3);                    /* Disable Sample Sequencer 3 */
    ADC0_EMUX_REG   &= ~0xF000;                    /* Sequencer 3 is triggered by software */
    ADC0_SSMUX3_REG &= ~0x000F;                    /* Clear MUX0 field */
    ADC0_SSMUX3_REG |= 1;                          /* Set channel number read by Sequencer 3 to Ain1 (PE2) */
    ADC0_SSCTL3_REG |= (1<<1) | (1<<2);            /* Set IE0 and END0 bits */
    ADC0_ACTSS_REG  |= (1<<3);                     /* Enable Sample Sequencer 3 */
}

unsigned long ADC0_ReadSeq3(void)
{
    unsigned long result = 0;
    ADC0_PSSI_REG |= (1<<3);                  /* Initiate SS3 to start the conversion by Sequencer 3 */
    while(GET_BIT(ADC0_RIS_REG,3) == 0){};    /* Wait for conversion in Sequencer 3 to be completed by polling the INR3 bit */
    result = ADC0_SSFIFO3_REG & 0xFFF;        /* Read result from Sequencer 3 FIFO register  */
    ADC0_ISC_REG |= (1<<3);                   /* Clear INR3 flag bit in ADC0_RIS_REG by setting IN3 bit in ADC0_ISC_REG */
    return result;
}

float GetTemperature(unsigned long digitalValue)
{
    return digitalValue * SCALE_FACTOR ;           /* Calculates temperature in Celsius */
}




