
#include "sys.h"

#include "uartdriver.h"


#include "stm32l1xx_hal_uart.h"
#include <stdlib.h>
#include "protocol.h"

//uint8_t aRxBuffer2; 

UART_HandleTypeDef huart2;

UART_STRUCT gUART_Struct;


void UART2_Init()
{
	UART_STRUCT *pUART_Struct = &gUART_Struct;
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_NVIC_Init();

	pUART_Struct->ReceiveFinishFlag = 0;
	pUART_Struct->ReceiveLen = 0;
	memset(pUART_Struct->ReceiveBuff,0,UART_BUFF_MAX);

	__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
	//HAL_UART_Receive_IT(&huart2,aRxBuffer2,1); 
	
}


static void MX_USART2_UART_Init(void)
{
 
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}


static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

static void MX_NVIC_Init(void)
{
  /* USART2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}


void UART2_SendData(unsigned char *data,unsigned char len)
{
	HAL_UART_Transmit(&huart2,data,len,100);
}

void UART2_Test(void)
{
	UART2_SendData("test",4);
}

//
void PrintfHexChar(unsigned char *HexStr,unsigned char len)
{
	
	char *Ptr =(char *)malloc(len*sizeof(char)*2);	//16进制数转换成字符串，1个字节变成两个字节，0x16 -> "16"（0x31,0x36）
	if((len != 0) && (len < PRINTF_HEX_MAXLEN))
	{
		if(Ptr != NULL)
		{
			IntToHexChar((unsigned char *)Ptr,HexStr,len);
			UART2_SendData((unsigned char *)Ptr,len*2);
		}
		free(Ptr);	//释放
		Ptr = NULL;
	}
}



void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UART_STRUCT *pUART_Struct = &gUART_Struct;
	 UNUSED(huart);
    if(huart->Instance == USART2)
    {
        //HAL_UART_Transmit(&huart2,aRxBuffer2,1,100);    
       	if(pUART_Struct->ReceiveLen <= UART_BUFF_MAX)
       	{
			pUART_Struct->ReceiveBuff[pUART_Struct->ReceiveLen++] = aRxBuffer2;
		}
		else
		{
			pUART_Struct->ReceiveLen = 0;
		}
        HAL_UART_Receive_IT(&huart2,aRxBuffer2,1);      
    }
}
*/

//自定义
void Uart2_Handler()
{
 	UART_STRUCT *pUART_Struct = &gUART_Struct;
    if((__HAL_UART_GET_FLAG(&huart2,UART_FLAG_RXNE)!=RESET))
     
    {
    		if(pUART_Struct->ReceiveLen <= UART_BUFF_MAX)
    		{	
            	pUART_Struct->ReceiveBuff[pUART_Struct->ReceiveLen++]=(uint8_t)(huart2.Instance->DR&(uint8_t)0x00FF);
			}
			else
			{
				pUART_Struct->ReceiveLen = 0;
			}
    		__HAL_UART_CLEAR_FLAG(&huart2,UART_FLAG_RXNE);
    }
     
    if((__HAL_UART_GET_FLAG(&huart2,UART_FLAG_IDLE)!=RESET)) 
    {
        USART2->SR;
		USART2->DR;
        __HAL_UART_CLEAR_FLAG(&huart2,UART_IT_IDLE);
		pUART_Struct->ReceiveFinishFlag = SET;
    }
}























