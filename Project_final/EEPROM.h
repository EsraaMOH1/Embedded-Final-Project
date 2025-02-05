/*
 * EEPROM.h
 *
 *  Created on: Dec 23, 2024
 *      Author: hager sharqawy
 */

#ifndef EEPROM_H_

void EEPROM_init(void);
unsigned int EEPROM_readInt(uint32_t block,uint32_t offset);
void EEPROM_WriteInt (uint32_t block, uint32_t offset, unsigned int error);
unsigned char EEPROM_ReadByte(uint32_t block, uint32_t offset, uint32_t byte);
void EEPROM_WriteByte(uint32_t block, uint32_t offset, uint32_t byte, unsigned char error);
void EEPROM_WriteString(uint32_t block, uint32_t offset, unsigned char* error);
void EEPROM_ReadString (uint32_t block, uint32_t offset, unsigned char* error);
void ERROR_LOG (unsigned char* ERROR_CODE);
void EEPROM_Retrieve (void);
//void EEPROM_ClearAll(void);

#define EEPROM_H_





#endif /* EEPROM_H_ */
