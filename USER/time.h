#ifndef __TIME_H
#define __TIME_H

#include "stm32l1xx.h"


#define TIMER_NUM 10

//配置
#define WIFI_CONFIG_ID 0
#define WIFI_CONFIG_DELAY 100

//注册
#define WIFI_REGIST_ID 0
#define WIFI_REGIST_DELAY 1000

//心跳
#define WIFI_HEART_ID 0
#define WIFI_HEART_DELAY 1000


//实时数据
#define SEND_REAL_DATA 1		
#define SEND_REAL_DATA_DELAY 2000

#define MICE_ID 2
#define MICE_DELAY 10000
	

typedef void callback(void);

typedef struct  
{
	uint8_t state;           //状态
	uint8_t mode;            //模式
	uint32_t match;          //到期时间
	uint32_t period;         //定时周期
	callback *cb;            //回调函数指针
	void *argv;              //参数指针
	uint16_t argc;           //参数个数
}softTimer;

typedef enum tmrState {
	SOFT_TIMER_STOPPED = 0,  //停止
	SOFT_TIMER_RUNNING,      //运行
	SOFT_TIMER_TIMEOUT       //超时
}tmrState;

typedef enum tmrMode {
	MODE_ONE_SHOT = 0,       //单次模式
	MODE_PERIODIC,           //周期模式
}tmrMode;





extern  __IO uint32_t SYS_Timing;







uint32_t SysTick_Get(void);
void softTimer_Init(void);
void softTimer_Create(uint16_t id, tmrMode mode, uint32_t delay, callback *cb, void *argv, uint16_t argc);
void softTimer_Update(void);
uint8_t softTimer_GetState(uint16_t id);

void softTimer_Stop(uint16_t id);







#endif

