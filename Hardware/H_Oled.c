#include "gpio.h"
#include "H_Oled_Font.h"
#include "tim.h"

/*
	普通定时器实现us延时
*/
void user_delaynus_tim(uint32_t nus)
{

	uint16_t differ = 0xffff - nus - 5;
	// 设置定时器2的技术初始值
	__HAL_TIM_SetCounter(&htim13, differ);
	// 开启定时器
	HAL_TIM_Base_Start(&htim13);

	while (differ < 0xffff - 5)
	{
		differ = __HAL_TIM_GetCounter(&htim13);
	};
	// 关闭定时器
	HAL_TIM_Base_Stop(&htim13);
}

/*��������*/

#define OLED_W_SCL(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, (GPIO_PinState)(x))
#define OLED_W_SDA(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, (GPIO_PinState)(x))

/*���ų�ʼ��*/
void OLED_I2C_Init(void)
{
	OLED_W_SCL(1);
	user_delaynus_tim(2);
	OLED_W_SDA(1);
	user_delaynus_tim(2);
}

/**
 * @brief  I2C��ʼ
 * @param  ��
 * @retval ��
 */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	user_delaynus_tim(2);
	OLED_W_SCL(1);
	user_delaynus_tim(2);
	OLED_W_SDA(0);
	user_delaynus_tim(2);
	OLED_W_SCL(0);
	user_delaynus_tim(2);
}

/**
 * @brief  I2Cֹͣ
 * @param  ��
 * @retval ��
 */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	user_delaynus_tim(2);
	OLED_W_SCL(1);
	user_delaynus_tim(2);
	OLED_W_SDA(1);
	user_delaynus_tim(2);
}

/**
 * @brief  I2C����һ���ֽ�
 * @param  Byte Ҫ���͵�һ���ֽ�
 * @retval ��
 */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(Byte & (0x80 >> i));
		OLED_W_SCL(1);
		user_delaynus_tim(2);
		OLED_W_SCL(0);
		user_delaynus_tim(2);
	}
	OLED_W_SCL(1); // �����һ��ʱ�ӣ�������Ӧ���ź�
	user_delaynus_tim(2);
	OLED_W_SCL(0);
	user_delaynus_tim(2);
}

/**
 * @brief  OLEDд����
 * @param  Command Ҫд�������
 * @retval ��
 */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78); // �ӻ���ַ
	OLED_I2C_SendByte(0x00); // д����
	OLED_I2C_SendByte(Command);
	OLED_I2C_Stop();
}

/**
 * @brief  OLEDд����
 * @param  Data Ҫд�������
 * @retval ��
 */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78); // �ӻ���ַ
	OLED_I2C_SendByte(0x40); // д����
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
 * @brief  OLED���ù��λ��
 * @param  Y �����Ͻ�Ϊԭ�㣬���·�������꣬��Χ��0~7
 * @param  X �����Ͻ�Ϊԭ�㣬���ҷ�������꣬��Χ��0~127
 * @retval ��
 */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);				 // ����Yλ��
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4)); // ����Xλ�ø�4λ
	OLED_WriteCommand(0x00 | (X & 0x0F));		 // ����Xλ�õ�4λ
}

/**
 * @brief  OLED����
 * @param  ��
 * @retval ��
 */
void OLED_Clear(void)
{
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for (i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
 * @brief  OLED��ʾһ���ַ�
 * @param  Line ��λ�ã���Χ��1~4
 * @param  Column ��λ�ã���Χ��1~16
 * @param  Char Ҫ��ʾ��һ���ַ�����Χ��ASCII�ɼ��ַ�
 * @retval ��
 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8); // ���ù��λ�����ϰ벿��
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]); // ��ʾ�ϰ벿������
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8); // ���ù��λ�����°벿��
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]); // ��ʾ�°벿������
	}
}

/**
 * @brief  OLED��ʾ�ַ���
 * @param  Line ��ʼ��λ�ã���Χ��1~4
 * @param  Column ��ʼ��λ�ã���Χ��1~16
 * @param  String Ҫ��ʾ���ַ�������Χ��ASCII�ɼ��ַ�
 * @retval ��
 */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
 * @brief  OLED�η�����
 * @retval ����ֵ����X��Y�η�
 */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
 * @brief  OLED��ʾ���֣�ʮ���ƣ�������
 * @param  Line ��ʼ��λ�ã���Χ��1~4
 * @param  Column ��ʼ��λ�ã���Χ��1~16
 * @param  Number Ҫ��ʾ�����֣���Χ��0~4294967295
 * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~10
 * @retval ��
 */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
 * @brief  OLED��ʾ���֣�ʮ���ƣ�����������
 * @param  Line ��ʼ��λ�ã���Χ��1~4
 * @param  Column ��ʼ��λ�ã���Χ��1~16
 * @param  Number Ҫ��ʾ�����֣���Χ��-2147483648~2147483647
 * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~10
 * @retval ��
 */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
 * @brief  OLED��ʾ���֣�ʮ�����ƣ�������
 * @param  Line ��ʼ��λ�ã���Χ��1~4
 * @param  Column ��ʼ��λ�ã���Χ��1~16
 * @param  Number Ҫ��ʾ�����֣���Χ��0~0xFFFFFFFF
 * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~8
 * @retval ��
 */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
 * @brief  OLED��ʾ���֣������ƣ�������
 * @param  Line ��ʼ��λ�ã���Χ��1~4
 * @param  Column ��ʼ��λ�ã���Χ��1~16
 * @param  Number Ҫ��ʾ�����֣���Χ��0~1111 1111 1111 1111
 * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~16
 * @retval ��
 */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
 * @brief  OLED��ʾ������
 * @param  Line ��ʼ��λ�ã���Χ��1~4
 * @param  Column ��ʼ��λ�ã���Χ��1~16
 * @param  Number Ҫ��ʾ�����֣�֧�ָ���
 * @param  IntegerLength �������ֳ��ȣ���Χ��1~10
 * @param  DecimalLength С�����ֳ��ȣ���Χ��1~10
 * @retval ��
 */
void OLED_ShowFloat(uint8_t Line, uint8_t Column, float Number, uint8_t IntegerLength, uint8_t DecimalLength)
{
	int32_t integerPart = (int32_t)Number;	  // ��ȡ��������
	float decimalPart = Number - integerPart; // ��ȡС������
	uint32_t decimalScaled;

	// ����������С�����ַ���
	if (decimalPart < 0)
		decimalPart = -decimalPart;

	// ��ʾ�������֣��Զ��������ţ�
	OLED_ShowSignedNum(Line, Column, integerPart, IntegerLength);

	// ��ʾС����
	OLED_ShowChar(Line, Column + IntegerLength + 1, '.'); // +1Ϊ����λԤ��

	// ���㲢��ʾС������
	decimalScaled = (uint32_t)(decimalPart * OLED_Pow(10, DecimalLength) + 0.5f); // ��������
	OLED_ShowNum(Line, Column + IntegerLength + 2, decimalScaled, DecimalLength); // +2�������ź�С����
}

/**
 * @brief  OLED��ʼ��
 * @param  ��
 * @retval ��
 */
void OLED_Init(void)
{
	uint32_t i, j;

	for (i = 0; i < 1000; i++) // �ϵ���ʱ
	{
		for (j = 0; j < 1000; j++)
			;
	}

	OLED_I2C_Init(); // �˿ڳ�ʼ��

	OLED_WriteCommand(0xAE); // �ر���ʾ

	OLED_WriteCommand(0xD5); // ������ʾʱ�ӷ�Ƶ��/����Ƶ��
	OLED_WriteCommand(0x80);

	OLED_WriteCommand(0xA8); // ���ö�·������
	OLED_WriteCommand(0x3F);

	OLED_WriteCommand(0xD3); // ������ʾƫ��
	OLED_WriteCommand(0x00);

	OLED_WriteCommand(0x40); // ������ʾ��ʼ��

	OLED_WriteCommand(0xA1); // �������ҷ���0xA1���� 0xA0���ҷ���

	OLED_WriteCommand(0xC8); // �������·���0xC8���� 0xC0���·���

	OLED_WriteCommand(0xDA); // ����COM����Ӳ������
	OLED_WriteCommand(0x12);

	OLED_WriteCommand(0x81); // ���öԱȶȿ���
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9); // ����Ԥ�������
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB); // ����VCOMHȡ��ѡ�񼶱�
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4); // ����������ʾ��/�ر�

	OLED_WriteCommand(0xA6); // ��������/��ת��ʾ

	OLED_WriteCommand(0x8D); // ���ó���
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF); // ������ʾ

	OLED_Clear(); // OLED����
}
