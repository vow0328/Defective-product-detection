#include "main.h" // Device header
#include "usart.h"
#include <stdio.h>
#include <stdarg.h>

uint8_t Serial3_RxData;                                  // 定义串口接收的数据变量
uint8_t Serial3_RxPacket[10];

/**
 * 函数：USART初始化函数
 * 参 数：无
 * 返回 值：无
 */
void Serial_Init(void)
{
  // 启用接收中断
  HAL_UART_Receive_IT(&huart3, &Serial3_RxData, 1);
}

/**
 * 函数：串口发送一个字节
 * 参 数：Byte 要发送的一个字节
 * 返回 值：无
 */
void Serial_SendByte(uint8_t Byte)
{
  HAL_UART_Transmit_DMA(&huart3, &Byte, 1); // 使用HAL库发送一个字节
}

/**
 * 函数：串口发送一个数组
 * 参 数：Array 要发送数组的首地址
 * 参 数：Length 要发送数组的长度
 * 返回 值：无
 */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
  HAL_UART_Transmit_DMA(&huart3, Array, Length); // 使用HAL库发送整个数组
}

/**
 * 函数：串口发送一个字符串
 * 参 数：String 要发送字符串的首地址
 * 返回 值：无
 */
void Serial_SendString(char *String)
{
  HAL_UART_Transmit_DMA(&huart3, (uint8_t *)String, strlen(String)); // 使用HAL库发送字符串
}

/**
 * 函数：次方函数（内部使用）
 * 返回 值：返回值等于X的Y次方
 */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
  uint32_t Result = 1; // 设置结果初值为1
  while (Y--)          // 执行Y次
  {
    Result *= X; // 将X累乘到结果
  }
  return Result;
}

/**
 * 函数：串口发送数字
 * 参 数：Number 要发送的数字，范围：0~4294967295
 * 参 数：Length 要发送数字的长度，范围：0~10
 * 返回 值：无
 */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
  uint8_t i;
  for (i = 0; i < Length; i++) // 根据数字长度遍历数字的每一位
  {
    Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0'); // 依次调用Serial_SendByte发送每位数字
  }
}

/**
 * 函数：使用printf需要重定向的底层函数
 * 参 数：保持原始格式即可，无需变动
 * 返回 值：保持原始格式即可，无需变动
 */
int fputc(int ch, FILE *f)
{
  Serial_SendByte(ch); // 将printf的底层重定向到自己的发送字节函数
  return ch;
}

/**
 * 函数：自己封装的printf函数
 * 参 数：format 格式化字符串
 * 参 数：... 可变的参数列表
 * 返回 值：无
 */
void Serial_Printf(char *format, ...)
{
  char String[100];              // 定义字符数组
  va_list arg;                   // 定义可变参数列表数据类型的变量arg
  va_start(arg, format);         // 从format开始，接收参数列表到arg变量
  vsprintf(String, format, arg); // 使用vsprintf打印格式化字符串和参数列表到字符数组中
  va_end(arg);                   // 结束变量arg
  Serial_SendString(String);     // 串口发送字符数组（字符串）
}



/**
 * 函数：获取串口接收的数据
 * 参 数：无
 * 返回 值：接收的数据，范围：0~255
 */
uint8_t Serial_GetRxData(void)
{
  return Serial3_RxData; // 返回接收的数据变量
}

/**
 * 函数：USART3接收中断回调函数
 * 参 数：无
 * 返回 值：无
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  static uint8_t RxState = 0;
  static uint8_t pRxPacket = 0;

  if (huart->Instance == USART3) // 确保是USART3的中断
  {
    if (RxState == 1)
    {
      if (Serial3_RxData != 0xFE)
      {
        Serial3_RxPacket[pRxPacket++] = Serial3_RxData;
      }
    }
    if (Serial3_RxData == 0xFF)
    {
      RxState = 1;
    }
    else if (Serial3_RxData == 0xFE)
    {
      RxState = 0;
      HAL_UART_Receive_IT(&huart3, &Serial3_RxData, 1); // 重新启动接收中断
    }
  }
}

