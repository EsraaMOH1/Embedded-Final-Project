/*
 * bonus_throttle.h
 *
 *  Created on: Dec 24, 2024
 *      Author: hager sharqawy
 */

#ifndef BONUS_THROTTLE_H_
#define BONUS_THROTTLE_H_

void GPIO_SetupPE4AnalogPin(void);
void ADC0thrott_Init(void);
unsigned long ADC0_Read_PE4(void);
float Get_ThrottlePosition(unsigned long adcValue );
//void Monitor_Throttle(void);



#endif /* BONUS_THROTTLE_H_ */
