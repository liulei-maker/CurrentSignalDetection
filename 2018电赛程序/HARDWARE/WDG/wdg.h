#ifndef __WDG_H
#define __WDG_H
#include "sys.h"

#define EWDG PAout(2) 

#if defined WDG_C
u8 EWDG_Flag;

void IWDG_Init(u8 prer,u16 rlr);
void IWDG_Feed(void);

void ESWDG_Init(void); //�ⲿ���Ź���ʼ��
void EWDG_Feed(void); //ι��
#else
extern u8 EWDG_Flag;

extern void IWDG_Init(u8 prer,u16 rlr);
extern void IWDG_Feed(void);

extern void ESWDG_Init(void); //�ⲿ���Ź���ʼ��
extern void EWDG_Feed(void); //ι��
#endif
#endif
