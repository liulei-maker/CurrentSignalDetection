
/*************************************************************************************************
                                   数据采集计算模块  v1.0
*************************************************************************************************/
#ifndef __DATA_SAMPLING_H
#define __DATA_SAMPLING_H
#include "sys.h"



#if defined DATA_SAMPLING_C
u8 SSend_flag=1;


short count_avg(short a[],unsigned short n);//计算平均值

int CalcSum(char chars[], unsigned short offset, unsigned short length);  //和校验

u8 XOR(u8 data[],u32 len);//异或校验



void SendSS(void);


#else
extern u8 SSend_flag;


extern short count_avg(short a[],unsigned short n);//计算平均值

extern int CalcSum(char chars[], unsigned short offset, unsigned short length);  //和校验

extern u8 XOR(u8 data[],u32 len);//异或校验


extern void SendSS(void);



#endif
#endif





     

