#ifndef __WDG_H
#define __WDG_H
#include "sys.h"

#define EWDG PAout(2) 

#if defined WDG_C
u8 EWDG_Flag;

void IWDG_Init(u8 prer,u16 rlr);
void IWDG_Feed(void);

void ESWDG_Init(void); //外部看门狗初始化
void EWDG_Feed(void); //喂狗
#else
extern u8 EWDG_Flag;

extern void IWDG_Init(u8 prer,u16 rlr);
extern void IWDG_Feed(void);

extern void ESWDG_Init(void); //外部看门狗初始化
extern void EWDG_Feed(void); //喂狗
#endif
#endif
