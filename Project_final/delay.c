/*
 * delay.c
 *
 *  Created on: 12 Dec 2024
 *      Author: abutalb
 */
#include <stdio.h>
#include "delay.h"

#define NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND 364


void Delay_MS(unsigned long long n){

    volatile unsigned long long count = 0;
    while(count++ < (NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND * n) );

}

