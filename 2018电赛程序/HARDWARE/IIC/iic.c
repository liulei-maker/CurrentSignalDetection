#define MYIIC_C
#include "iic.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 						  
//////////////////////////////////////////////////////////////////////////////////

#define ADDRESS 0xF0

//初始化IIC
void IIC_Init(void)
{					     
	I2C_InitTypeDef I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);


	//I2C GPIO Config
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


  I2C_InitStructure.I2C_ClockSpeed    =  100000;     //200kHZ  
	I2C_InitStructure.I2C_Mode          =  I2C_Mode_I2C;  
	I2C_InitStructure.I2C_DutyCycle     =  I2C_DutyCycle_2; //根据datasheet，Tlow/Thigh = 2  
	I2C_InitStructure.I2C_OwnAddress1   =  0x00;          
	I2C_InitStructure.I2C_Ack           =  I2C_Ack_Enable;  
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;  
	I2C_Cmd(I2C1,ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
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
  for(i=0;i<60;i++); 
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
  delay_us(1);	
	IIC_SCL=1;
	delay_us(5); 
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(5); 
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
  delay_us(5);
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=1;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(1); 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(5);
 	IIC_SCL=0;
	delay_us(5);	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
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
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0; 
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
		if((txd&0x80)>>7)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		delay_us(5);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(5); 
		IIC_SCL=0;	
		delay_us(5);
    }	
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
		{
        IIC_SCL=0; 
        delay_us(2);
				IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)
				{
					receive |= 0x01;
				}
				else
				{
					receive |= 0x00;
				}
				delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}


//I2C读数据寄存器0x02 两个字节 高字节在前  从地址为0xF0
unsigned int ReadDataCfgReg(void)  
{ 
  unsigned int temp; 
  IIC_Start(); 
  IIC_Send_Byte(0xF1);
  IIC_Wait_Ack();
  ADS_delay();
  temp=(IIC_Read_Byte(1)&0x0F)*0x100;//高字节 高四位清除 
  temp+=(IIC_Read_Byte(1));          //temp=高字节+低字节
  IIC_Stop();  
  return(temp); 
     
}

/*    2.  * 函数名：I2C_EE_PageWrite    
 * 描述  ：在EEPROM的一个写循环中可以写多个字节，但一次写入的字节数   
 *         不能超过EEPROM页的大小。AT24C02每页有8个字节。   
 * 输入  ：-pBuffer 缓冲区指针    
 *         -WriteAddr 接收数据的EEPROM的地址    
 *         -NumByteToWrite 要写入EEPROM的字节数   
 * 输出  ：无   
 * 返回  ：无    
  * 调用  ：外部调用   
  */    
 
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)    
 {     
     while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); // Added by Najoua  27/08/2008    
          
   /* Send START condition */                                                                                                                               
   I2C_GenerateSTART(I2C1, ENABLE);    
        
 
  /* Test on EV5 and clear it */    
   while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));     
        
   /* Send EEPROM address for write */    
   I2C_Send7bitAddress(I2C1, ADDRESS, I2C_Direction_Transmitter) ;    
        
   /* Test on EV6 and clear it */    
   while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));      
     
   /* Send the EEPROM's internal address to write to */        
   I2C_SendData(I2C1, WriteAddr);      
     
   /* Test on EV8 and clear it */    
   while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));    
     
   /* While there is data to be written */   
   while(NumByteToWrite--)      
   {     
     /* Send the current byte */    
     I2C_SendData(I2C1, *pBuffer);     
     
     /* Point to the next byte to be written */   
     pBuffer++;     
        
     /* Test on EV8 and clear it */    
     while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));      
   }     
   /* Send STOP condition */    
   I2C_GenerateSTOP(I2C1, ENABLE);    
 } 
/*    2.  * 函数名：I2C_EE_BufferRead    
  * 描述  ：从EEPROM里面读取一块数据。    
  * 输入  ：-pBuffer 存放从EEPROM读取的数据的缓冲区指针。   
  *         -WriteAddr 接收数据的EEPROM的地址。     
  *         -NumByteToWrite 要从EEPROM读取的字节数。   
  * 输出  ：无   
  * 返回  ：无    
  * 调用  ：外部调用   
.  */    
 void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)    
 {       
   //*((u8 *)0x4001080c) |=0x80;      
//      while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); // Added by Najoua  27/08/2008    
//          
          
   /* Send START condition */    
   I2C_GenerateSTART(I2C1, ENABLE);    
   //*((u8 *)0x4001080c) &=~0x80;    
        
   /* Test on EV5 and clear it */     
   while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));    
     
   /* Send EEPROM address for write */    
   I2C_Send7bitAddress(I2C1, 0xF0, I2C_Direction_Transmitter) ;    
     
   /* Test on EV6 and clear it */    
   while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));    
        
   /* Clear EV6 by setting again the PE bit */   
   I2C_Cmd(I2C1, ENABLE);    
     
   /* Send the EEPROM's internal address to write to */   
   I2C_SendData(I2C1, ReadAddr);      
     
   /* Test on EV8 and clear it */    
   while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));    
        
   /* Send STRAT condition a second time */      
   I2C_GenerateSTART(I2C1, ENABLE);    
        
   /* Test on EV5 and clear it */    
   while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));    
        
   /* Send EEPROM address for read */    
   I2C_Send7bitAddress(I2C1, ADDRESS, I2C_Direction_Receiver);      
        
   /* Test on EV6 and clear it */    
   while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) );    
        
   /* While there is data to be read */   
   while(NumByteToRead)      
   {     
     if(NumByteToRead == 1)    
     {     
       /* Disable Acknowledgement */    
       I2C_AcknowledgeConfig(I2C1, DISABLE);    
            
       /* Send STOP Condition */    
       I2C_GenerateSTOP(I2C1, ENABLE);    
     }    
    
     /* Test on EV7 and clear it */    
     if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))      
     {           
       /* Read a byte from the EEPROM */   
       *pBuffer = I2C_ReceiveData(I2C1);    
     
       /* Point to the next location where the byte read will be saved  */    
       pBuffer++;     
            
       /* Decrement the read bytes counter */   
       NumByteToRead--;            
     }       
   }    
     
   /* Enable Acknowledgement to be ready for another reception */   
   I2C_AcknowledgeConfig(I2C1, ENABLE);    
 } 
























