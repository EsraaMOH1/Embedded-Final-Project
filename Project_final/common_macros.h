/*
 ============================================================================
 Name        : common_macros.h
 Author      : Mohamed Tarek
 Description : Header file for the commonly used Macros
 Date        : 31/3/2019
 ============================================================================
 */

#ifndef COMMON_MACROS
#define COMMON_MACROS

/* Set a specific bit in any register */
#define SET_BIT(REG,BIT) (REG|=(1<<BIT))

/* Clear a specific bit in any register */
#define CLEAR_BIT(REG,BIT) (REG&=(~(1<<BIT)))

/* Toggle a specific bit in any register */
#define TOGGLE_BIT(REG,BIT) (REG^=(1<<BIT))

/* Get a specific bit in Register */
#define GET_BIT(REG,BIT) ( ( REG & (1<<BIT) ) >> BIT )

#endif
