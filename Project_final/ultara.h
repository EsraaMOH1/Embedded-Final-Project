#ifndef ULTARA_H_
#define ULTARA_H_

/* Pin Configurations */
#define TRIGGER_PIN 5  // Trigger pin is Pin E5
#define ECHO_PIN 1     // Echo pin is Pin E1

/* Conversion Constants */
#define SOUND_SPEED 343.0            // Speed of sound in m/s
#define CLOCK_FREQUENCY 16000000     // Clock frequency in Hz

/* Function Prototypes */
void SysTick_Init(void);           // Initialize SysTick Timer
void HCSR04_Init(void);            // Initialize ultrasonic sensor pins
void HCSR04_TriggerPulse(void);    // Send trigger pulse
float HCSR04_GetDistance(void);    // Get distance in centimeters
void GPIOPortE_Handler(void);      // Echo pin (E1) Interrupt handler
void Delay_US(int time);

extern volatile unsigned long pulseTicks;  // Stores pulse ticks
extern volatile float distance;             // Stores measured distance

#endif /* ULTARA_H_ */
