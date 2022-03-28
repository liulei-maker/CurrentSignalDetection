#ifndef _ADS1118_H_
#define _ADS1118_H_
#include "sys.h"
#include <stdio.h>

extern void Delay_us(unsigned int);
extern void Delay_ms(unsigned int);

extern unsigned int Tc_Value0;
extern unsigned int Tc_Value1;
extern unsigned int Tc_Value2;
extern unsigned int Tc_Value3;

extern unsigned int Cj_Value0;
extern unsigned int Cj_Value1;
extern unsigned int Cj_Value2;
extern unsigned int Cj_Value3;

extern unsigned int Voltage_Value;

void ADS1118_Init(void);
void ADS1118_TestOnce(void);

#endif


