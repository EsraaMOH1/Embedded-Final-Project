#ifndef TEMPERATURE_H
#define TEMPERATURE_H

void GPIO_SetupPE2AnalogPin(void);

void ADC0_Init(void);

unsigned long ADC0_ReadSeq3(void);

float GetTemperature(unsigned long digitalValue);

#endif // TEMPERATURE_H
