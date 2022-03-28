#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
					  
//////////////////////////////////////////////////////////////////////////////////

//IO方向设置
// #define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}
// #define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}
// #define SDA_IN()  {GPIOB->CRH&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;} 
// #define SDA_OUT() {GPIOB->CRH&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}

//IO操作函数	 
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //输入SDA 

#if defined MYIIC_C
int Velocity;                       //倾角输出*1000
u32 tempVel;
//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号
void ADS_delay(void);
void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	 
//I2C读数据寄存器
unsigned int ReadDataCfgReg(u8 index,u8 firstAddress,u8 SecondAddress);
#else
extern int Velocity;                       //倾角输出*1000
extern u32 tempVel;
//IIC所有操作函数
extern void IIC_Init(void);                //初始化IIC的IO口				 
extern void IIC_Start(void);				//发送IIC开始信号
extern void IIC_Stop(void);	  			//发送IIC停止信号
extern void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
extern u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
extern u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
extern void IIC_Ack(void);					//IIC发送ACK信号
extern void IIC_NAck(void);				//IIC不发送ACK信号
extern void ADS_delay(void) ;
extern void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
extern u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	 
//I2C读数据寄存器  
extern unsigned int ReadDataCfgReg(u8 index,u8 firstAddress,u8 SecondAddress);
#endif
#endif
















