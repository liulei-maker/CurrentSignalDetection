#include "FM25V02.h"
#include "spi.h"
#include "usart.h"


#define WREN    0x06
#define WRSR    0x01
#define READ    0x03
#define WRITE   0x02
#define RDSR    0x05 

void SPI2_FM25V02_Init(void)//��ʼ��FM25V02
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOG, ENABLE );//PORTBʱ��ʹ�� 

	//SPI2 NSS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  // PB12����   
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_12);
	
	//FM25V02 W
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  // PG6����   
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
 	GPIO_Init(GPIOG, &GPIO_InitStructure);
 	GPIO_ResetBits(GPIOG,GPIO_Pin_6);


	SPI2_Init();		   	//��ʼ��SPI2
	SPI2_SetSpeed(SPI_BaudRatePrescaler_2);//����18Mʱ�ӣ�����ģʽ
	
	//����FM25V02�ɶ���д
	SPI2_FM25V02_CS=0;
	SPI2_ReadWriteByte(WREN);
	SPI2_FM25V02_CS=1;
	
	SPI2_FM25V02_CS=0;
	SPI2_ReadWriteByte(WRSR);
	SPI2_FM25V02_CS=1;
	
}

void SPI2_FM25V02_Write(u16 address,char data)//д����
{
	//����д
	SPI2_FM25V02_CS=0;
	SPI2_ReadWriteByte(WREN);
	SPI2_FM25V02_CS= 1;
	
	//д��
	SPI2_FM25V02_CS=0;
	SPI2_ReadWriteByte(WRITE);
	SPI2_ReadWriteByte(address>>8);  //��ַ��λ
	SPI2_ReadWriteByte(address>>0);  //��ַ��λ
	SPI2_ReadWriteByte(data);
	SPI2_FM25V02_CS=1;
}


char SPI2_FM25V02_Read(u16 address)//������
{
	char data;
	SPI2_FM25V02_CS=0;
	SPI2_ReadWriteByte(READ);
	SPI2_ReadWriteByte(address>>8);  //��ַ��λ
	SPI2_ReadWriteByte(address>>0);  //��ַ��λ
	data = SPI2_ReadWriteByte(0xff);
	SPI2_FM25V02_CS=1;
	return data;
}
void SPI2_FM25V02_nWrite(u16 address,char *data,u16 len)//д�������
{
	u16 i;
	FM25V02_W=1;//д������
	if(FM25V02_W !=1)
	{
		printf("�ر�д����ʧ�ܣ�\r\n");
		return;
  }
	for(i=0;i<len;i++)
	{
		SPI2_FM25V02_Write(address+i,data[i]);
	}
	FM25V02_W=0;//д������
}
void SPI2_FM25V02_nRead(u16 address,char *data,u16 len)//���������
{
	u16 i;
	for(i=0;i<len;i++)
	{
		data[i] = SPI2_FM25V02_Read(address+i);
	}
}


