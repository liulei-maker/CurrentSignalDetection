
/*************************************************************************************************
                                   ���ݲɼ�����ģ��  v1.0
*************************************************************************************************/
#ifndef __DATA_SAMPLING_H
#define __DATA_SAMPLING_H
#include "sys.h"



#if defined DATA_SAMPLING_C
u8 SSend_flag=1;


short count_avg(short a[],unsigned short n);//����ƽ��ֵ

int CalcSum(char chars[], unsigned short offset, unsigned short length);  //��У��

u8 XOR(u8 data[],u32 len);//���У��



void SendSS(void);


#else
extern u8 SSend_flag;


extern short count_avg(short a[],unsigned short n);//����ƽ��ֵ

extern int CalcSum(char chars[], unsigned short offset, unsigned short length);  //��У��

extern u8 XOR(u8 data[],u32 len);//���У��


extern void SendSS(void);



#endif
#endif





     

