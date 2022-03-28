#define MYIIC_C
#include "myiic.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 						  
//////////////////////////////////////////////////////////////////////////////////
 
//初始化IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP ;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_7|GPIO_Pin_6); 	// 输出高
}

void SDA_IN(void)//设置SDA为输入
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //PA15设置成输入，默认上拉 
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB。7
	
}
void SDA_OUT(void)//设置SDA为输出
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP ;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
//I2C延时
void ADS_delay(void) 
{  
  unsigned char i;    
  for(i=0;i<40;i++); 
}

//I2C时钟
void ADS_clock(void)  
{      
  IIC_SCL=1;//SCL=1
  ADS_delay(); 
  IIC_SCL=0;//SCL=0 
  ADS_delay();   
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	ADS_delay(); 
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	ADS_delay();
  ADS_delay(); 
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=1;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	ADS_delay();
// 	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	ADS_delay();							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;ADS_delay();	   
	IIC_SCL=1;ADS_delay();	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
// 	unsigned int i; 
//   SDA_IN();      //SDA设置为输入 
//   ADS_delay(); 
//   IIC_SCL=1; 
//   i=0xF0; 
//   do 
//   { 
//   } 
//   while(--i!=0); 
//   IIC_SCL=0;//时钟输出0 	 
// 	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	ADS_delay(); 
// 	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	ADS_delay(); 
	IIC_SCL=1;
	ADS_delay(); 
	IIC_SCL=0;
	ADS_delay(); 
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80))
			IIC_SDA=1;
		else
			IIC_SDA=0;
		//ADS_delay();    //对TEA5767这三个延时都是必须的
		ADS_clock();
		txd<<=1; 		
    }	 
		//ADS_delay();
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,adsdata;
  SDA_IN();//SDA设置为输入
  adsdata=0; 
	 for(i=0;i<8;i++) 
	 {
		adsdata <<=1; 
		if(READ_SDA==1) 
		 adsdata |= 0x01;
		else
		 adsdata |= 0x00;
		//ADS_delay();
		ADS_clock();
	 }  
  return(adsdata); 
}


//I2C读数据寄存器0x02 两个字节 高字节在前  
//
unsigned int ReadDataCfgReg(u8 index,u8 firstAddress,u8 SecondAddress)  
{ 
  unsigned int temp;
  u8 result[4];
  IIC_Start(); 
  IIC_Send_Byte(0x2A);
  IIC_Wait_Ack();
  ADS_delay();
  result[0]=(IIC_Read_Byte(1)&firstAddress)*0x100;//高字节 高四位清除
  IIC_Ack(); 
  result[1]=(IIC_Read_Byte(1));          //temp=高字节+低字节
	IIC_Ack(); 
	if(index==1)
	{
		result[2]=(IIC_Read_Byte(1)&SecondAddress);//
		IIC_Ack(); 
		result[3]=(IIC_Read_Byte(1));          //temp=高字节+低字节
		IIC_Ack(); 
	}
  
	
  IIC_Stop();
  temp = (int)(((result[0] << 24) | (result[1] << 16) | (result[2] << 8) | (result[3] & 0xff)));  
  return(temp); 
     
}



























