#include "ultara.h"
#include "tm4c123gh6pm_registers.h"
#include "common_macros.h"

volatile unsigned long pulseTicks = 0;  // Stores pulse ticks
volatile unsigned char edgeCount = 0;      // Edge counter


void Delay_US(int time)
{
    volatile int i, j;
    for (i = 0; i < time; i++) {
        for (j = 0; j < 16; j++) {  // Assuming each iteration takes 1 cycle
            // Empty loop for delay
        }
    }
}

/* Initialize SysTick Timer */
void SysTick_Init(void)
{
    SYSTICK_CTRL_REG = 0;               // Disable SysTick Timer
    SYSTICK_RELOAD_REG = 0xFFFFFF;      // Set Reload value for 1 second delay
    SYSTICK_CURRENT_REG = 0;            // Clear Current Register
    SYSTICK_CTRL_REG |= 0x04;           // Enable SysTick,but timer still not started
}
// delay fnc us

/* Initialize the Trigger and Echo Pins */
void HCSR04_Init(void)
{


    GPIO_PORTE_DIR_REG |= (1 << TRIGGER_PIN);    // Set Trigger Pin (PE5) as output
    GPIO_PORTE_DEN_REG |= (1 << TRIGGER_PIN);    // Enable digital function for Trigger Pin

    GPIO_PORTE_DIR_REG &= ~(1 << ECHO_PIN);     // Set Echo Pin (PE1) as input
    GPIO_PORTE_DEN_REG |= (1 << ECHO_PIN);      // Enable digital function for Echo Pin

    // Configure the Echo pin for interrupt on both edges
    GPIO_PORTE_IM_REG |= (1 << ECHO_PIN);  // Enable interrupt on ECHO_PIN
    GPIO_PORTE_IS_REG |= (1 << ECHO_PIN);  // set to be edge sensitive
    GPIO_PORTE_IBE_REG &= ~(1 << ECHO_PIN); //clearing the bit to be controlled by IEV
    GPIO_PORTE_IEV_REG |= (1 << ECHO_PIN);  // set to detect Rising edge
    GPIO_PORTE_ICR_REG |= (1 << ECHO_PIN);  // Clear any prior interrupt flag
    NVIC_EN0_REG |= (1 << 4);       // Enable GPIO Port E interrupt in NVIC
}

/* Interrupt handler for GPIO Port E */
void GPIOPortE_Handler(void)
{
   edgeCount++; // incrementing every time the handler was triggered 1 for rising 2 for falling

   if(edgeCount == 1) // rising flag got detected
   {
        GPIO_PORTE_ICR_REG |= (1 << ECHO_PIN);  // Clear interrupt flag
        GPIO_PORTE_IEV_REG &= ~(1 << ECHO_PIN); // Switch to detect falling edge by clearing the bit as it was set to detect rising
        SYSTICK_CTRL_REG |= 0x01;               // Start SysTick Timer
   }
   else if(edgeCount == 2) // falling edge detected
   {
        GPIO_PORTE_ICR_REG |= (1 << ECHO_PIN);  // Clear interrupt flag
        GPIO_PORTE_IEV_REG |= (1 << ECHO_PIN);  // revert back to detecting rising flags as the falling has been already detected
        pulseTicks = 0xFFFFFF - SYSTICK_CURRENT_REG; // getting the time from start to falling edge
        SYSTICK_CTRL_REG &= ~(1 << 0);           // stopping the SysTick timer
   }
   GPIO_PORTE_ICR_REG |= (1 << ECHO_PIN);  // Clear the interrupt flag
}

/* Trigger Pulse Function */
void HCSR04_TriggerPulse(void)


{
    GPIO_PORTE_DATA_REG &= ~(1 << TRIGGER_PIN);  // Set Trigger Pin low
    Delay_US(10);
    GPIO_PORTE_DATA_REG |= (1 << TRIGGER_PIN);  // Set Trigger Pin high
    Delay_US(10);   // Wait for ~10  s
    GPIO_PORTE_DATA_REG &= ~(1 << TRIGGER_PIN);  // Set Trigger Pin low
}


/* Get Distance Function */
float HCSR04_GetDistance(void)
{
    // Trigger pulse for distance measurement
    HCSR04_TriggerPulse();

    // Wait for both rising and falling edges (echo signal)
    if (edgeCount == 2)
    {
        // Calculate no of ticks (from SysTick Timer)
        unsigned long time_in_sys = pulseTicks;  // Get the elapsed time in ticks
        float pulseDuration = (float)time_in_sys / CLOCK_FREQUENCY;  // Convert to seconds
        float distance1 = pulseDuration * SOUND_SPEED * 90 / 2;  // Calculate the distance in cm

        // Reset edge counter for next measurement
        edgeCount = 0;  // Clear edgeCount to be ready for the next measurement
	SYSTICK_CTRL_REG &= ~(1 << 0);
	SYSTICK_CURRENT_REG = 0;

        return distance1;  // Return the calculated distance in centimeters
    }

    return -1.0;  // Return -1 if measurement is not ready (echo not detected)
}
