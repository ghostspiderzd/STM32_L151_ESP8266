#ifndef __UART_DRIVER_H
#define __UART_DRIVER_H

#include "sys.h"
#include "stm32l1xx_hal_uart.h"
#include "string.h"

#define DEFAULT_LOG_LEVEL 2
#define LOG_LEVEL_DEBUG 0
#define INFO_LEVEL_DEBUG 1
#define HEX_LEVEL_DEBUG 2

#define PRINTF_HEX_MAXLEN 200

extern UART_HandleTypeDef huart2;
#define DEBUG_LOG(string)	\
	if(DEFAULT_LOG_LEVEL >= LOG_LEVEL_DEBUG) \
		{	\
			 UART2_SendData(#string##"\r\n",strlen(#string)+2); \
		}

#define DEBUG_INFO(string,len) 	\
	if(DEFAULT_LOG_LEVEL >= INFO_LEVEL_DEBUG) \
		{	\
			 UART2_SendData(string,len); \
		}

#define DEBUG_HEX(hexstr,len) \
	if(DEFAULT_LOG_LEVEL >= HEX_LEVEL_DEBUG)	\
		{	\
			PrintfHexChar(hexstr,len);	\
		}

#define UART_BUFF_MAX 100

#define UART_ReceiveReday 0
#define UART_Receivestart 1




	






typedef struct
	{
		unsigned char ReceiveBuff[UART_BUFF_MAX];
		unsigned char ReceiveLen;
		unsigned char ReceiveFinishFlag;
		unsigned char ReceiveState;		//接收状态
	}UART_STRUCT;
extern UART_STRUCT gUART_Struct;








void UART2_Init(void);
void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_NVIC_Init(void);
void _Error_Handler(char *file, int line);
void Uart2_Handler(void);
void UART2_SendData(unsigned char *data,unsigned char len);
void UART2_Test(void);
void PrintfHexChar(unsigned char *HexStr,unsigned char len);


#endif /* __UART_DRIVER_H */

