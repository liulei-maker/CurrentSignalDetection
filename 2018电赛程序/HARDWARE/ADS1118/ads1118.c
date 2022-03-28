#include "ads1118.h"
#include "delay.h"
#define BIT(n) (1<<(n))

#define ADS_CS2_H	GPIOE->ODR |= BIT(11)
#define ADS_CS2_L	GPIOE->ODR &= (~BIT(11))

#define ADS_CS1_H	GPIOE->ODR |= BIT(12)
#define ADS_CS1_L	GPIOE->ODR &= (~BIT(12))

#define ADS_CLK_H	GPIOE->ODR |= BIT(13)
#define ADS_CLK_L	GPIOE->ODR &= (~BIT(13))

#define ADS_DIN_H	GPIOE->ODR |= BIT(14)
#define ADS_DIN_L	GPIOE->ODR &= (~BIT(14))

#define ADS_DOUT	(GPIOE->IDR & BIT(15))


#define ADSCH1_STARTONCE 0x8f8B  //MUX[2:0]000---AIN P为AIN0且AIN N 为AIN1启动单次转换
#define ADSCH2_STARTONCE 0xB58B  //MUX[2:0]001 = AIN P为AIN2 AIN N 为AIN3+0.256V
#define ADSTMP_STARTONCE 0x859B  //启动温度单次转换+2.048V

#define ADSIN0_VALUE     0xc38B
#define ADSIN1_VALUE     0xd38B
#define ADSIN2_VALUE     0xe38B
#define ADSIN3_VALUE     0xf38B 

//#define ADSCH1_STARTONCE 0x0e8B  //AIN P为AIN0 AIN N 为AIN1启动单次转换，连续转换
//#define ADSCH2_STARTONCE 0x3e8B  //011 = AIN P为AIN2 AIN N 为AIN3+0.256V
//#define ADSTMP_STARTONCE 0x859B  //启动温度单次转换+2.048V


unsigned int Tc_Value0 = 0;
unsigned int Tc_Value1 = 0;
unsigned int Tc_Value2 = 0;
unsigned int Tc_Value3 = 0;

unsigned int Cj_Value0 = 0;
unsigned int Cj_Value1 = 0;
unsigned int Cj_Value2 = 0;
unsigned int Cj_Value3 = 0;

unsigned int Voltage_Value = 0;


void ADSPort_Init(void)
{
	ADS_CS1_H;
	ADS_CS2_H;
	ADS_DIN_L;
	ADS_CLK_L;
}



unsigned char ADS1118_Config(unsigned char CS,unsigned int cmd)
{
	unsigned char n;
	unsigned int dat = 0;
	unsigned int temp = 0;
	ADS_CLK_L;
	ADS_DIN_L;
	if(CS)
	{
		ADS_CS1_L;
	}
	else
	{
		ADS_CS2_L;
	}
	delay_us(1);
	
	for(n=0;n<16;n++)
	{
		ADS_CLK_H;
		if(cmd & BIT(15-n))
		{
			ADS_DIN_H;
		}
		else
		{
			ADS_DIN_L;
		}

		delay_us(1);
		ADS_CLK_L;
	}
	delay_us(1);
	ADS_DIN_L;
	for(n=0;n<16;n++)
	{
		ADS_CLK_H;
		delay_us(1);
		if(ADS_DOUT)
		{
			dat |= BIT(15-n);
		}
		ADS_CLK_L;
	}
	
	ADS_DIN_L;
	
	if(CS)
	{
		ADS_CS1_H;
	}
	else
	{
		ADS_CS2_H;
	}
	
	if(dat == cmd)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

unsigned int ADS1118_ReadDat(unsigned char CS,unsigned int cmd)
{
	unsigned char n;
	unsigned int dat = 0;
	ADS_CLK_L;
	ADS_DIN_L;
	if(CS)
	{
		ADS_CS1_L;
	}
	else
	{
		ADS_CS2_L;
	}
	delay_us(1);
	while(ADS_DOUT);
	
	for(n=0;n<16;n++)
	{
		ADS_CLK_H;
		if(cmd & BIT(15-n))
		{
			ADS_DIN_H;
		}
		else
		{
			ADS_DIN_L;
		}
		
		if(ADS_DOUT)
		{
			dat |= BIT(15-n);
		}
		
		ADS_CLK_L;
	}
	ADS_DIN_L;
	
	ADS_CS1_H;
	ADS_CS2_H;

	return dat;
}



void ADS1118_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14; // 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	ADSPort_Init();
	//ADS1118_Config(1,ADSCH1_STARTONCE);
	
//	ADS1118_Config(0,ADSCH2_STARTONCE);
//	ADS1118_Config(0,ADSCH1_STARTONCE);
	
	ADS1118_Config(0,ADSCH1_STARTONCE);		
	ADS1118_Config(0,ADSIN1_VALUE);	
	ADS1118_Config(0,ADSIN3_VALUE);	
}

void ADS1118_TestOnce(void)
{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
	unsigned int temp;

	
//	Tc_Value3 = ADS1118_ReadDat(0,ADSTMP_STARTONCE);
//	Cj_Value1 = ADS1118_ReadDat(0,ADSCH1_STARTONCE);
//	Cj_Value2 = ADS1118_ReadDat(0,ADSCH2_STARTONCE);
	Cj_Value3 = ADS1118_ReadDat(0,ADSCH1_STARTONCE);
	delay_ms(10);		
	Voltage_Value = ADS1118_ReadDat(0,ADSIN2_VALUE);	
	delay_ms(10);	
	Cj_Value2 = ADS1118_ReadDat(0,ADSIN3_VALUE);	

	
}


