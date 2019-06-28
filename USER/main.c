#include "stm32l1xx.h"
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "brd_cfg.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"
#include "time.h"
#include "uartdriver.h"
#include "wifidriver.h"
#include "protocol.h"
#include "switch.h"
#include "delay.h"
#include "speed.h"

void M8266WIFI_Test(void);
int main(void)
{
	u8 success=0;
	WIFI_SEND_RECEIVE_STRUCT *pWIFI_Send_Receive_Struct = &gWIFI_Send_Receive_Struct;
#ifdef USE_LED_AND_KEY_FOR_TEST	
  u8 i;	
#endif	

  HAL_Init();
	
	//void SystemClock_Config(int pllm,int plln,int pllp,int pllq,int pllr);  APB1_CLK = APB2_CLK = SYSCLK = 4MHz * plln / pllm / pllr
  SystemClock_Config();	// HSI=8MHz, PLLM=12, PLLN=3=> SYSCLK= APB1_CLK = APB2_CLK = SYSCLK = 32MHz
	delay_Init();

	UART2_Init();
	DEBUG_LOG(Init ...);

#ifdef USE_LED_AND_KEY_FOR_TEST			
  LED_Init();					// Initialise LEDs
  KEY_Init();					// Initialise KEYs
#endif
	
  /////////////////////////////////////////////////////////////////////////////////////////////////////
	//1. Call M8266HostIf_Init() to initialize the MCU Host Interface
	//   - including SPI and nREST/nCS GPIO pins
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	M8266HostIf_Init();
	
#ifdef USE_LED_AND_KEY_FOR_TEST		
  //Flash 5 times both of the two LEDs simutaneously indicating that next will initialisation the M8266WIFI module
	for(i=0; i<5; i++)
  {
		LED_set(0, 0); LED_set(1, 0);  M8266WIFI_Module_delay_ms(50);
		LED_set(0, 1); LED_set(1, 1);  M8266WIFI_Module_delay_ms(50);
	}
#endif
	
	
  /////////////////////////////////////////////////////////////////////////////////////////////////////
	//2. Call M8266WIFI_Module_Init_Via_SPI() to initialize the wifi module via SPI Interface
  //   - Including: Module reset, module select, module connecting wifi if required, and etc	
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	success = M8266WIFI_Module_Init_Via_SPI();
  if(success)
	{
#ifdef USE_LED_AND_KEY_FOR_TEST			
	    for(i=0; i<3; i++)  // Flash 3 times the Two LEDs alternatively in the Main Board indicating M8266WIFI initialised successfully
	    {
	       LED_set(0, 1); LED_set(1, 0);  M8266WIFI_Module_delay_ms(100);
		     LED_set(0, 0); LED_set(1, 1);  M8266WIFI_Module_delay_ms(100);
			}
		  LED_set(0, 1); LED_set(1, 1);
#endif
	}	
	else // If M8266WIFI module initialisation failed, two led constantly flash in 2Hz
	{
		  while(1)
			{
#ifdef USE_LED_AND_KEY_FOR_TEST					
	       LED_set(0, 1); LED_set(1, 1);  M8266WIFI_Module_delay_ms(250);
		     LED_set(0, 0); LED_set(1, 0);  M8266WIFI_Module_delay_ms(250);
#endif
			}
	}

  /////////////////////////////////////////////////////////////////////////////////////////////////////
	//3. Call M8266WIFI_Test() to setup socket connection and tansmit/receive data test in a high-speed manner
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//M8266WIFI_Test(); //
	Switch_Init();
	MX_TIM2_Init();
	MX_GPIO_Init();		
	PROTOCOL_StructInit();
	WIFI_Struct_Init();
	softTimer_Init();
	DEBUG_LOG(Init finish);
	softTimer_Create(WIFI_CONFIG_ID,MODE_PERIODIC,WIFI_CONFIG_DELAY,WIFI_ATCmdInit,0,0);
	DEBUG_LOG(Creat tast_WIFI_Init);
	while(1)
	{
		softTimer_Update();
		if(pWIFI_Send_Receive_Struct->tcpReceiveFlag == SET)
		{
			pWIFI_Send_Receive_Struct->tcpReceiveFlag = RESET;
			WIFI_Receive_AND_AnalysisData();
		}
#ifdef USE_LED_AND_KEY_FOR_TEST					
	       LED_set(0, 1); LED_set(1, 1);  M8266WIFI_Module_delay_ms(1000);
		     LED_set(0, 0); LED_set(1, 0);  M8266WIFI_Module_delay_ms(1000);
#endif
	}
} //end of main 

