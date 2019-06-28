#ifndef _SPEED_H
#define _SPEED_H

















typedef struct
{
	uint32_t nowtime;
	uint32_t lasttime;
	uint32_t nowcnt;	//当前计数
	uint32_t lastcnt;
	uint32_t nowspeed;	//当前速度
}SPEED_STRUCT;
extern  SPEED_STRUCT gSpeed_Struct;








void MX_TIM2_Init(void);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base);
void Speed_calculate(void);






#endif
