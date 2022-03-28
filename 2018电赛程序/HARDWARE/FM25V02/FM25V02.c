#include "FM25V02.h"
#include "spi.h"
#include "usart.h"


#define WREN    0x06
#define WRSR    0x01
#define READ    0x03
#define WRITE   0x02
#define RDSR    0x05 

void SPI2_FM25V02_Init(void)//初始化FM25V02
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOG, ENABLE );//PORTB时钟使能 

	//SPI2 NSS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  // PB12推挽   
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_12);
	
	//FM25V02 W
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  // PG6推挽   
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
 	GPIO_Init(GPIOG, &GPIO_InitStructure);
 	GPIO_ResetBits(GPIOG,GPIO_Pin_6);


	SPI2_Init();		   	//初始化SPI2
	SPI2_SetSpeed(SPI_BaudRatePrescaler_2);//设置18M时钟，高速模式
	
	//设置FM25V02可读可写
	SPI2_FM25V02_CS=0;
	SPI2_ReadWriteByte(WREN);
	SPI2_FM25V02_CS=1;
	
	SPI2_FM25V02_CS=0;
	SPI2_ReadWriteByte(WRSR);
	SPI2_FM25V02_CS=1;
	
}

void SPI2_FM25V02_Write(u16 address,char data)//写数据
{
	//允许写
	SPI2_FM25V02_CS=0;
	SPI2_ReadWriteByte(WREN);
	SPI2_FM25V02_CS= 1;
	
	//写入
	SPI2_FM25V02_CS=0;
	SPI2_ReadWriteByte(WRITE);
	SPI2_ReadWriteByte(address>>8);  //地址高位
	SPI2_ReadWriteByte(address>>0);  //地址地位
	SPI2_ReadWriteByte(data);
	SPI2_FM25V02_CS=1;
}


char SPI2_FM25V02_Read(u16 address)//读数据
{
	char data;
	SPI2_FM25V02_CS=0;
	SPI2_ReadWriteByte(READ);
	SPI2_ReadWriteByte(address>>8);  //地址高位
	SPI2_ReadWriteByte(address>>0);  //地址低位
	data = SPI2_ReadWriteByte(0xff);
	SPI2_FM25V02_CS=1;
	return data;
}
void SPI2_FM25V02_nWrite(u16 address,char *data,u16 len)//写多个数据
{
	u16 i;
	FM25V02_W=1;//写保护关
	if(FM25V02_W !=1)
	{
		printf("关闭写保护失败！\r\n");
		return;
  }
	for(i=0;i<len;i++)
	{
		SPI2_FM25V02_Write(address+i,data[i]);
	}
	FM25V02_W=0;//写保护开
}
void SPI2_FM25V02_nRead(u16 address,char *data,u16 len)//读多个数据
{
	u16 i;
	for(i=0;i<len;i++)
	{
		data[i] = SPI2_FM25V02_Read(address+i);
	}
}


