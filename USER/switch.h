#ifndef _SWITCH_H
#define _SWITCH_H

#include "sys.h"










//#define SWITCH_GPIO_RCC RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB
#define SWITCH_POWER_GPIOA GPIOA
#define SWITCH_POWER_GPIOB GPIOB

//PB
#define SWITCH_MODE_PIN GPIO_PIN_1			//模式
#define SWITCH_SUB_PIN GPIO_PIN_15			//减
#define SWITCH_ADD_PIN GPIO_PIN_10			//加

//#define SWITCH_POWER_PIN GPIO_Pin_7			//阻力
//#define SWITCH_POWER_PIN GPIO_Pin_8			//时间
//#define SWITCH_POWER_PIN GPIO_Pin_11		
//#define SWITCH_POWER_PIN GPIO_Pin_12			//启动


//#define SWITCH_POWER_PIN GPIO_Pin_0			//速度
//#define SWITCH_POWER_PIN GPIO_Pin_1			//功率
//#define SWITCH_POWER_PIN GPIO_Pin_2			//正/反
//#define SWITCH_POWER_PIN GPIO_Pin_12
//#define SWITCH_POWER_PIN GPIO_Pin_13			//循环





void Switch_Init(void);
void Switch_DeleteInit(void);
void Switch_Start(void);
void Switch_Loop(void);
void Switch_Mode(void);
void Switch_Speed(void);
void Switch_Direction(void);
void Switch_Resistance(void);
void Switch_Time(void);
void Switch_Power(void);
void Switch_Add(void);
void Switch_Sub(void);
void Power_ON(void);






#endif
