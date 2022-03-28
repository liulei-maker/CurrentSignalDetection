#define _FDC2X14_C
#include "sys.h"		    
#include "FDC2X14.h"	
#include "myiic.h"
#include "delay.h"


#define FDC2X14_Address 0x2A
#define FDC2X14_W FDC2X14_Address<<1
#define FDC2X14_R (FDC2X14_Address<<1)+1
#define DATA_CH0 0x00                  //ͨ��0���ݵĸ�12λ
#define DATA_LSB_CH0 0x01              //ͨ��1���ݵĵ�12λ��λ�������ֲ�15ҳ����4��
#define DATA_CH1 0x02
#define DATA_LSB_CH1 0x03
#define DATA_CH2 0x04
#define DATA_LSB_CH2 0x05
#define DATA_CH3 0x06
#define DATA_LSB_CH3 0x07 
#define RCOUNT_CH0 0x08               //ת��ʱ�����üĴ�����
#define RCOUNT_CH1 0x09
#define RCOUNT_CH2 0x0A
#define RCOUNT_CH3 0x0B
#define OFFSET_CH0 0x0C
#define OFFSET_CH1 0x0D
#define OFFSET_CH2 0x0E
#define OFFSET_CH3 0x0F
#define SETTLECOUNT_CH0 0x10
#define SETTLECOUNT_CH1 0x11
#define SETTLECOUNT_CH2 0x12
#define SETTLECOUNT_CH3 0x13
#define CLOCK_DIVIDERS_C_CH0 0x14
#define CLOCK_DIVIDERS_C_CH1 0x15
#define CLOCK_DIVIDERS_C_CH2 0x16
#define CLOCK_DIVIDERS_C_CH3 0x17
#define STATUS 0x18
#define ERROR_CONFIG 0x19
#define CONFIG 0x1A
#define MUX_CONFIG 0x1B
#define RESET_DEV 0x1C
#define DRIVE_CURRENT_CH0 0x1E
#define DRIVE_CURRENT_CH1 0x1F
#define DRIVE_CURRENT_CH2 0x20
#define DRIVE_CURRENT_CH3 0x21
#define MANUFACTURER_ID 0x7E
#define DEVICE_ID 0x7F


///FDC2X14 IO��ʼ��
void FDC2X14_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP ;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_1); 	// �����
	GPIO_ResetBits(GPIOC,GPIO_Pin_2); 	// �����
	GPIO_ResetBits(GPIOC,GPIO_Pin_3); 	// �����
	IIC_Init();//��ʼ��iic
	
	//����SetFDC2X14�Ĵ���
	SetFDC2X14(RCOUNT_CH0,0x30,0xCB);
	SetFDC2X14(RCOUNT_CH1,0x30,0xCB);
	SetFDC2X14(RCOUNT_CH2,0x30,0xCB);
	SetFDC2X14(RCOUNT_CH3,0x30,0xCB);
	
	SetFDC2X14(CLOCK_DIVIDERS_C_CH0,0x20,0x01);
	SetFDC2X14(CLOCK_DIVIDERS_C_CH1,0x20,0x01);	
	SetFDC2X14(CLOCK_DIVIDERS_C_CH2,0x20,0x01);
	SetFDC2X14(CLOCK_DIVIDERS_C_CH3,0x20,0x01);	

	
	SetFDC2X14(SETTLECOUNT_CH0,0x00,0x19);
	SetFDC2X14(SETTLECOUNT_CH1,0x00,0x19);
	SetFDC2X14(SETTLECOUNT_CH2,0x00,0x19);
	SetFDC2X14(SETTLECOUNT_CH3,0x00,0x19);

	
	SetFDC2X14(ERROR_CONFIG,0x00,0x00);
	
//	SetFDC2X14(MUX_CONFIG,0x82,0x0c);//CH0��CH1
  SetFDC2X14(MUX_CONFIG,0xD2,0x0c);//CH0��CH1��CH3��CH4    0xD2:11010010-0x0c:00001100
	SetFDC2X14(DRIVE_CURRENT_CH0,0x50,0x00);
	SetFDC2X14(DRIVE_CURRENT_CH1,0x50,0x00);
	SetFDC2X14(DRIVE_CURRENT_CH2,0x50,0x00);
	SetFDC2X14(DRIVE_CURRENT_CH3,0x50,0x00);
	
	SetFDC2X14(CONFIG,0x16,0x01);
}

//���ݶ�ȡ
//indexͨ������
int FDC2X14ReadCH(u8 index)
{
	unsigned int result;
	switch(index)
	{
		case 0x01:
			result = ReadFDC2X14(DATA_CH0,DATA_LSB_CH0);
			break;
		case 0x02:
			result = ReadFDC2X14(DATA_CH1,DATA_LSB_CH1);
			break;
		case 0x03:
			result = ReadFDC2X14(DATA_CH2,DATA_LSB_CH2);
			break;
		case 0x04:
			result = ReadFDC2X14(DATA_CH3,DATA_LSB_CH3);
			break;
	}
	return result;
}
void SetFDC2X14(u8 Address,u8 MSB,u8 LSB)
{
	IIC_Start(); 
  IIC_Send_Byte(FDC2X14_W);
  IIC_Wait_Ack();
  ADS_delay();
	IIC_Send_Byte(Address);
	IIC_Wait_Ack();
  ADS_delay();
	IIC_Send_Byte(MSB);
	IIC_Wait_Ack();
  ADS_delay();
	IIC_Send_Byte(LSB);
	IIC_Wait_Ack();
  ADS_delay();
	IIC_Stop();
	ADS_delay();

}

unsigned int ReadFDC2X14(u8 firstAddress,u8 secondAddress)
{
	 unsigned int temp;
  u8 result[4];
  IIC_Start(); 
  IIC_Send_Byte(FDC2X14_W);
  IIC_Wait_Ack();
  ADS_delay();
	IIC_Send_Byte(firstAddress);
	IIC_Wait_Ack();
	ADS_delay();
	IIC_Stop();
	ADS_delay();
	
	IIC_Start();
	IIC_Send_Byte(FDC2X14_R);
  IIC_Wait_Ack();
  ADS_delay();
  result[0]=IIC_Read_Byte(1);//���ֽ� ����λ���
	result[0] = result[0]<<4;
	result[0] = result[0]>>4;
  IIC_Ack(); 
	ADS_delay();
  result[1]=(IIC_Read_Byte(1));          //temp=���ֽ�+���ֽ�
	IIC_Ack(); 
	IIC_Stop();
	ADS_delay();
	
	IIC_Start(); 
  IIC_Send_Byte(FDC2X14_W);
  IIC_Wait_Ack();
  ADS_delay();
	IIC_Send_Byte(secondAddress);
	IIC_Wait_Ack();
	ADS_delay();
	IIC_Stop();
	ADS_delay();
	
	IIC_Start();
	IIC_Send_Byte(FDC2X14_R);
  IIC_Wait_Ack();
  ADS_delay();
  result[2]=IIC_Read_Byte(1);   //���ֽ� ����λ���
  IIC_Ack();
	ADS_delay();
  result[3]=(IIC_Read_Byte(1)); //temp=���ֽ�+���ֽ�
	IIC_Ack(); 
	IIC_Stop();
	ADS_delay();
	
  temp = (unsigned int)(((result[0]<< 24) | (result[1] << 16) | (result[2] << 8) | (result[3] & 0xff)));  
  return(temp); 
}
























