#ifndef __FDC2X14_H
#define __FDC2X14_H			 
#include "sys.h"	 								  

#if defined _FDC2X14_C
int ChValue[4];

///FDC2X14 IO��ʼ��
void FDC2X14_Init(void);
void SetFDC2X14(u8 Address,u8 MSB,u8 LSB);
//���ݶ�ȡ
//indexͨ������
int FDC2X14ReadCH(u8 index);
unsigned int ReadFDC2X14(u8 firstAddress,u8 secondAddress);
#else

extern int ChValue[4];

///FDC2X14 IO��ʼ��
void FDC2X14_Init(void);

extern void SetFDC2X14(u8 Address,u8 MSB,u8 LSB);
//���ݶ�ȡ
//indexͨ������
extern int FDC2X14ReadCH(u8 index);
extern unsigned int ReadFDC2X14(u8 firstAddress,u8 secondAddress);

#endif

#endif	   
















