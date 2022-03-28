#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

#if defined TIMER_C
u8 fivemsec_flag;
u8 usart1_flag=0;
u8 usart1_timer=0;
u8 usart1_count=0;
u8 usart4_flag=0;
u8 usart4_timer=0;
u8 usart4_count=0;
u8 timer4_count=0;
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);
#else
extern u8 fivemsec_flag;
extern u8 usart1_flag;
extern u8 usart1_timer;
extern u8 usart1_count;
extern u8 usart4_flag;
extern u8 usart4_timer;
extern u8 usart4_count;
extern u8 timer4_count;
extern void TIM3_Int_Init(u16 arr,u16 psc);
extern void TIM4_Int_Init(u16 arr,u16 psc);
#endif
 
#endif
