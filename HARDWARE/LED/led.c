#include "led.h"

//On Discovery MB963: LED3->GPIOB7, LED4->GPIOB6

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();		// GPIOB
	
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;   // PB6/7
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull=GPIO_PULLUP;
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
    LED_set(0, 0);
    LED_set(1, 0);
}

void LED_set(u8 led_no, u8 on)
{
	if(led_no!=0)
	{
		if(on) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
		else   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	}
	else
	{
		if(on) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
		else   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	}
}
