#include "sys.h"
#include "time.h"
#include "speed.h"
#include "stm32l1xx_hal.h"
#include "uartdriver.h"


 TIM_HandleTypeDef ghtim2;
 SPEED_STRUCT gSpeed_Struct;


 void MX_TIM2_Init(void)
{
  
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  ghtim2.Instance = TIM2;
  ghtim2.Init.Prescaler = 0;
  ghtim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  ghtim2.Init.Period = 0xffff;
  ghtim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&ghtim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
  sClockSourceConfig.ClockPolarity = TIM_CLOCKPOLARITY_NONINVERTED;
  sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
  sClockSourceConfig.ClockFilter = 0;
  if (HAL_TIM_ConfigClockSource(&ghtim2, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&ghtim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

	HAL_TIM_Base_Start(&ghtim2);
}

 void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
 {
 
   GPIO_InitTypeDef GPIO_InitStruct;
   if(htim_base->Instance==TIM2)
   {
   /* USER CODE BEGIN TIM2_MspInit 0 */
 
   /* USER CODE END TIM2_MspInit 0 */
	 /* Peripheral clock enable */
	 __HAL_RCC_TIM2_CLK_ENABLE();
   
	 /**TIM2 GPIO Configuration    
	 PA0-WKUP1	   ------> TIM2_ETR 
	 */
	 GPIO_InitStruct.Pin = GPIO_PIN_0;
	 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	 GPIO_InitStruct.Pull = GPIO_NOPULL;
	 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	 GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 
   /* USER CODE BEGIN TIM2_MspInit 1 */
 
   /* USER CODE END TIM2_MspInit 1 */
   }
 
 }



void Speed_calculate()
{
	 volatile SPEED_STRUCT *pSpeed_Struct = &gSpeed_Struct;
	 pSpeed_Struct->nowtime = SysTick_Get();	//获取当前时间
	 pSpeed_Struct->nowcnt = ghtim2.Instance->CNT;
	 if(pSpeed_Struct->lasttime != 0)
	 	{
			//pSpeed_Struct->nowspeed = (pSpeed_Struct->nowcnt-pSpeed_Struct->lastcnt) * 10 * 94 / 72 * 100 / (pSpeed_Struct->nowtime - pSpeed_Struct->lasttime);
			pSpeed_Struct->nowspeed =(((pSpeed_Struct->nowcnt-pSpeed_Struct->lastcnt)*630*100/72)/(pSpeed_Struct->nowtime - pSpeed_Struct->lasttime));	//速度放大100倍,周长0.63
	 	}
	 pSpeed_Struct->lasttime = pSpeed_Struct->nowtime;
	 //pSpeed_Struct->lastcnt = ghtim2.Instance->CNT;
	 pSpeed_Struct->lastcnt = pSpeed_Struct->nowcnt;
}

