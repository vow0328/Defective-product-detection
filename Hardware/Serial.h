#ifndef __SERIAL_H
#define __SERIAL_H

#define SERIAL_PACKET_QUEUE_SIZE 10
#define SERIAL_PACKET_MAX_LEN 64

typedef struct
{
  uint8_t data[SERIAL_PACKET_MAX_LEN];
  uint8_t len;
} SerialPacket;

extern QueueHandle_t serial3PacketQueue; // 串口3的包队列

void Serial_Init(void);
void Serial3_SendByte(uint8_t Byte);
void Serial3_SendArray(uint8_t *Array, uint16_t Length);
void Serial3_SendString(char *String);
void Serial3_SendNumber(uint32_t Number, uint8_t Length);
uint8_t Serial3_GetRxData(void);
void Serial3_Printf(char *format, ...);
void Serial3_GetRxPacket(uint8_t *buf, size_t n);



#endif
