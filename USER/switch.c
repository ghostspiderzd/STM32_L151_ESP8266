#include "switch.h"
#include "sys.h"

#include "delay.h"

void Switch_Init()
{

   GPIO_InitTypeDef GPIO_Initure;
	  /* GPIOD Periph clock enable */
   __HAL_RCC_GPIOA_CLK_ENABLE();
   __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure PD0 and PD1 or PD3 and PD7 in output pushpull mode */
    GPIO_Initure.Pin = SWITCH_MODE_PIN | SWITCH_SUB_PIN | SWITCH_ADD_PIN | GPIO_PIN_9;
  GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Initure.Speed = GPIO_SPEED_HIGH;
  GPIO_Initure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SWITCH_POWER_GPIOB, &GPIO_Initure);
  Switch_DeleteInit();
  Switch_Mode();
}

void Switch_DeleteInit()
{
	HAL_GPIO_WritePin(SWITCH_POWER_GPIOB, SWITCH_MODE_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SWITCH_POWER_GPIOB, SWITCH_SUB_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SWITCH_POWER_GPIOB, SWITCH_ADD_PIN, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(SWITCH_POWER_GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
}

void Switch_Start()
{
	
}

void Switch_Loop()
{
	
}

void Switch_Mode()
{
	HAL_GPIO_WritePin(SWITCH_POWER_GPIOB, SWITCH_MODE_PIN, GPIO_PIN_SET);
	delay_ms(500);
	HAL_GPIO_WritePin(SWITCH_POWER_GPIOB, SWITCH_MODE_PIN, GPIO_PIN_RESET);
} 

void Switch_Speed()
{
	
}

void Switch_Direction()
{
	
}

void Switch_Resistance()
{
	
}

void Switch_Time()
{
	
}

void Switch_Power()
{
	
}

void Switch_Add()
{
	HAL_GPIO_WritePin(SWITCH_POWER_GPIOB, SWITCH_ADD_PIN, GPIO_PIN_SET);
	delay_ms(500);
	HAL_GPIO_WritePin(SWITCH_POWER_GPIOB, SWITCH_ADD_PIN, GPIO_PIN_RESET);
}

void Switch_Sub()
{
	HAL_GPIO_WritePin(SWITCH_POWER_GPIOB, SWITCH_SUB_PIN, GPIO_PIN_SET);
	delay_ms(500);
	HAL_GPIO_WritePin(SWITCH_POWER_GPIOB, SWITCH_SUB_PIN, GPIO_PIN_RESET);
}


void Power_ON()
{
	HAL_GPIO_WritePin(SWITCH_POWER_GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
}

















