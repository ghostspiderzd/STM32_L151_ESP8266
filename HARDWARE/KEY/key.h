#ifndef _KEY_H
#define _KEY_H
#include "sys.h"

#define KEY0_PRES 	1
#define KEY1_PRES	  2
#define KEY2_PRES	  3
#define WKUP_PRES   4

void KEY_Init(void);
u8 KEY_Scan(u8 mode);

#endif
