#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

void Serial_Init(void);
void Serial3_SendByte(uint8_t Byte);
void Serial3_SendArray(uint8_t *Array, uint16_t Length);
void Serial3_SendString(char *String);
void Serial3_SendNumber(uint32_t Number, uint8_t Length);
uint8_t Serial3_GetRxData(void);
void Serial3_Printf(char *format, ...);
void Serial3_GetRxPacket(uint8_t *buf, size_t n);

void Serial2_SendByte(uint8_t Byte);
void Serial2_SendArray(uint8_t *Array, uint16_t Length);
void Serial2_SendString(char *String);
void Serial2_SendNumber(uint32_t Number, uint8_t Length);
uint8_t Serial2_GetRxData(void);
void Serial2_Printf(char *format, ...);



#endif
