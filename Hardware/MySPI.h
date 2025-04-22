#ifndef __MYSPI_H
#define __MYSPI_H

void MySPI_Init(void);
void MySPI_Start(void);
void MySPI_Stop(void);
uint8_t MySPI_SwapByte(uint8_t ByteSend);
void MySPI_W_SS(GPIO_PinState BitValue);
void MySPI_W_SCK(GPIO_PinState BitValue);
void MySPI_W_MOSI(GPIO_PinState BitValue);
uint8_t MySPI_R_MISO(void);

#endif
