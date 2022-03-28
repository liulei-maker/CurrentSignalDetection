/*************************************************************************************************
                                   数据处理模块  v1.0
*************************************************************************************************/
#define DATA_SAMPLING_C
#include "Data_sampling.h"
#include "usart.h"
#include "FDC2X14.h"	
#include <stdio.h>
#include <math.h>
#include <stdlib.h> 


u32 ReadNum=0;          
u16 ReadByteNum=0;
u8 XORResult=0; //XOR


//计算平均值
short count_avg(short a[],unsigned short n)
{
  unsigned short i;
  short result_avg=0;
  long sum=0;
  for(i=0;i<n;i++)
    sum+=a[i];
  result_avg=(short)(sum/n);
  return result_avg; 
}


//和校验
int CalcSum(char chars[], unsigned short offset, unsigned short length)
{
  int intValue = 0;
	int i=0;
  for (i = offset; i < length; ++i)
  {
    intValue += chars[i];
  }
  return intValue;
}
//异或校验
u8 XOR(u8 data[],u32 len)
{
	u8 results=0;
	u32 i;
	for(i=0;i<=len-1;i++)
	{
		results ^= data[i];
	}	
	return results;
}



//发送实时数据
void SendSS()
{
	int k=0;
	ReadByteNum=0;
	USART1_TX_BUF[ReadByteNum++] = 0x55;
 	USART1_TX_BUF[ReadByteNum++] = 0xAA;
	USART1_TX_BUF[ReadByteNum++] = 0x15;
	USART1_TX_BUF[ReadByteNum++] = 0x01;
	k=ChValue[0];
	USART1_TX_BUF[ReadByteNum++] = k>>24;
	USART1_TX_BUF[ReadByteNum++] = k>>16;
	USART1_TX_BUF[ReadByteNum++] = k>>8;
	USART1_TX_BUF[ReadByteNum++] = k>>0;
	k=ChValue[1];
	USART1_TX_BUF[ReadByteNum++] = k>>24;
	USART1_TX_BUF[ReadByteNum++] = k>>16;
	USART1_TX_BUF[ReadByteNum++] = k>>8;
	USART1_TX_BUF[ReadByteNum++] = k>>0;
	k=ChValue[2];
	USART1_TX_BUF[ReadByteNum++] = k>>24;
	USART1_TX_BUF[ReadByteNum++] = k>>16;
	USART1_TX_BUF[ReadByteNum++] = k>>8;
	USART1_TX_BUF[ReadByteNum++] = k>>0;
	k=ChValue[3];
	USART1_TX_BUF[ReadByteNum++] = k>>24;
	USART1_TX_BUF[ReadByteNum++] = k>>16;
	USART1_TX_BUF[ReadByteNum++] = k>>8;
	USART1_TX_BUF[ReadByteNum++] = k>>0;
	USART1_TX_BUF[ReadByteNum]	= XOR(USART1_TX_BUF,ReadByteNum);
	Usart1_Sendbuf(SendBuf,USART1_TX_BUF,ReadByteNum+1);
}






