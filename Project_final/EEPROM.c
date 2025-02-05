#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "EEPROM.h"
#include "UART.h"

#include "tm4c123gh6pm_registers.h"
#include "common_macros.h"

#define EEPROM_EESUPP_REG           (*((volatile unsigned long *)0x400AF01C))
#define EEDONE_REG                  (*((volatile unsigned long *)0x400AF018))
#define EEPROM_EEBLOCK_REG          (*((volatile unsigned long *)0x400AF004))
#define EEPROM_EEOFFSET_REG         (*((volatile unsigned long *)0x400AF008))
#define EEPROM_EERDWR_REG           (*((volatile unsigned long *)0x400AF010))

#define MAX_OFFSET 15
#define MAX_BLOCK 31



void EEPROM_init(void){
    //enabling the EEPROM clock and waiting for to stabilize
    SYSCTL_RCGCEEPROM_REG |= 0x01;
    while(GET_BIT(SYSCTL_PREEPROM_REG,0)==0);

    // Reseting the EEPROM and waiting for it to stabilize
    SYSCTL_SREEPROM_REG |= 0x01;
    SYSCTL_SREEPROM_REG &= ~0x01;
    while ((SYSCTL_PREEPROM_REG & 0x01) == 0);

    //waiting for the working bit is set to check if EEPROM is working properly
    while(GET_BIT(EEDONE_REG,0)==1);

    // Checking for errors in the EESUPP register (PRETRY and ERETRY bits)
    if (EEPROM_EESUPP_REG & (0x08 | 0x10)) {
        UART0_SendString("EEPROM Initialization Failed: Error in EESUPP\n");
        while(1); //stop the system
    }

    //to check for errors in EEDONE register
    if(EEDONE_REG &(0x08| 0x10)){
        UART0_SendString("EEPROM Initialization Failed\n");
        while(1); //stop the system
    }
    UART0_SendString("EEPROM Initialization Successful\n");

}

unsigned int EEPROM_readInt(uint32_t block,uint32_t offset)
{
    EEPROM_EEBLOCK_REG = block; //select block from EEBLOCK REG
    EEPROM_EEOFFSET_REG = offset; //offset to select word to read
    return (EEPROM_EERDWR_REG); //return 32bit value
}

void EEPROM_WriteInt (uint32_t block, uint32_t offset, unsigned int error){

    while((EEDONE_REG &0x20)!=0);
    EEPROM_EEBLOCK_REG = block; //select block from EEBLOCK REG
    EEPROM_EEOFFSET_REG = offset; //offset to select word to read
    EEPROM_EERDWR_REG =error;
}

unsigned char EEPROM_ReadByte(uint32_t block, uint32_t offset, uint32_t byte){
    EEPROM_EEBLOCK_REG = block; //select block from EEBLOCK REG
    EEPROM_EEOFFSET_REG = offset; //offset to select word to read
    return (EEPROM_EERDWR_REG >> (byte * 8)) & 0xFF; //shift and mask the byte from 32bit word
}

void EEPROM_WriteByte(uint32_t block, uint32_t offset, uint32_t byte, unsigned char error){
    while((EEDONE_REG &0x20)!=0);
    EEPROM_EEBLOCK_REG = block; //select block from EEBLOCK REG
    EEPROM_EEOFFSET_REG = offset; //offset to select word to read
    // Write only the specified byte, preserving other bytes in the word
    EEPROM_EERDWR_REG= (EEPROM_EERDWR_REG  &~(0xFF << (byte * 8))) | (error<< (byte*8));
}

void EEPROM_WriteString(uint32_t block, uint32_t offset, unsigned char* error){
    int i;
    for(i=0; i<4;i++){
        EEPROM_WriteByte(block,offset,i,error[i] );
    }
}

void EEPROM_ReadString (uint32_t block, uint32_t offset, unsigned char* error){
    int i=0;
       for(i=0; i<4;i++){
         error[i] = EEPROM_ReadByte(block,offset,i);
       }
}


void ERROR_LOG (unsigned char* ERROR_CODE){
    UART0_SendString("\r\n");
    UART0_SendString("Logging Error: ");
    UART0_SendString(ERROR_CODE);
    if (ERROR_CODE[3] == '1')
        UART0_SendString("DTC_ACCIDENT_MIGHT_HAPPENED");
    else if (ERROR_CODE[3] == '2')
        UART0_SendString("DTC_ENGINE_HIGH_TEMP");
        UART0_SendString("\r\n");


    unsigned int count = EEPROM_readInt(0, 0);
    count++;
    EEPROM_WriteInt(0,0,count);

    uint32_t ERROR_CODE_block = 0;
    uint32_t ERROR_CODE_offset = 0;

    while (count > MAX_OFFSET) {
        count -= MAX_OFFSET;
        ERROR_CODE_block++;
    }
    ERROR_CODE_offset = count;

    EEPROM_WriteString(ERROR_CODE_block,ERROR_CODE_offset,ERROR_CODE);
    UART0_SendString("EEPROM Write Successful\n");

}

void EEPROM_Retrieve (void){
    unsigned int count = EEPROM_readInt(0, 0);
    UART0_SendString("\r\n");
    UART0_SendString("Retrieving Logged Errors:\n");

    uint32_t ERROR_CODE_block = 0;
    uint32_t ERROR_CODE_offset = 0;

    unsigned int i=0;

    for (i = 0; i < count; i++) {
        int temp=i;
        ERROR_CODE_offset++;
        while(ERROR_CODE_offset >MAX_OFFSET){
            temp-=(MAX_OFFSET);
            ERROR_CODE_block++;
            ERROR_CODE_offset=0;
        }

        unsigned char ERROR_CODE[4];
        EEPROM_ReadString(ERROR_CODE_block,ERROR_CODE_offset,ERROR_CODE);
        UART0_SendString("\r\n");
        UART0_SendString(ERROR_CODE);

        if (ERROR_CODE[3] == '1'){
            UART0_SendString(" Accident Happened\n");
        }
        else if (ERROR_CODE[3] == '2') {
            UART0_SendString(" Engine High Temperature detected\n");
        }
        UART0_SendString("\r\n");
    }

}
