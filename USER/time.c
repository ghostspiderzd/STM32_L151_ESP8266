#include "time.h"
#include "uartdriver.h"
#include "sys.h"
#include "core_cm3.h"


extern __IO uint32_t uwTick;

static softTimer timer[TIMER_NUM]; 	   //软件定时器数组



uint32_t SysTick_Get()
{
	return uwTick;
}

//软定时器初始化
void softTimer_Init(void)
{
	uint16_t i;
	for(i=0; i<TIMER_NUM; i++) {
		timer[i].state = SOFT_TIMER_STOPPED;
		timer[i].mode = MODE_ONE_SHOT;
		timer[i].match = 0;
		timer[i].period = 0;
		timer[i].cb = NULL;
		timer[i].argv = NULL;
		timer[i].argc = 0;
	}
}
/*参数：id:定时器ID
		mode:运行模式
		delay:定时时间
		cb:回掉函数
		argv:回调函数参数
		argc:回掉函数参数数量
*/
void softTimer_Create(uint16_t id, tmrMode mode, uint32_t delay, callback *cb, void *argv, uint16_t argc)
{
	assert_param(id < TIMER_NUM);
	assert_param(mode == MODE_ONE_SHOT || mode == MODE_PERIODIC);
	
	timer[id].match = SysTick_Get() + delay;
	timer[id].period = delay;
	timer[id].state = SOFT_TIMER_RUNNING;
	timer[id].mode = mode;
	timer[id].cb = cb;
	timer[id].argv = argv;
	timer[id].argc = argc;
}

void softTimer_Update(void)
{
	static unsigned char i = 0;

	if(i < TIMER_NUM)
	{
		switch (timer[i].state) {
          case SOFT_TIMER_STOPPED:
			  break;
		
		  case SOFT_TIMER_RUNNING:
			  if(timer[i].match <= SysTick_Get()) {
				  timer[i].state = SOFT_TIMER_TIMEOUT;
				 // timer[i].cb(timer[i].argv, timer[i].argc);       //执行回调函数
				 timer[i].cb();
			  }
			  break;
			
		  case SOFT_TIMER_TIMEOUT:
			  if(timer[i].mode == MODE_ONE_SHOT) {
			      timer[i].state = SOFT_TIMER_STOPPED;
			  } else {
				  timer[i].match = SysTick_Get() + timer[i].period;
			      timer[i].state = SOFT_TIMER_RUNNING;
			  }
			  break;
		  default:
			 // printf("timer[%d] state error!\r\n", i);
			  break;
	  }
		i++;
	}
	else
	{
		i = 0;
	}
}

uint8_t softTimer_GetState(uint16_t id)
{
	return timer[id].state;
}


void softTimer_Stop(uint16_t id)
{
	assert_param(id < TIMER_NUM);
	timer[id].state = SOFT_TIMER_STOPPED;
}

























