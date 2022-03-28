#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "wdg.h"
#include "spi.h"
#include "Data_sampling.h"
#include "FDC2X14.h"
#include "FM25V02.h"

#include "stdlib.h"
#include "oled.h"
#include "ads1118.h"
#include <math.h>
void LED_Init(void);
 int main(void)
 {	
  float CH0_VOLTAGE_VALUE=0;
  float CH1_VOLTAGE_VALUE=0;	 
  float CH2_VOLTAGE_VALUE=0;	 
  float CH3_VOLTAGE_VALUE=0;	
  float  voltage=0;	
  u8 str_voltage[20];	 
  u8 str0[20];
  u8 str1[20];		
  u8 str2[20];		
  u8 str3[20];		
		
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	delay_init();	    	     //延时函数初始化	  
	uart1_init(9600);	 	     //串口1初始化为115200
	//SPI2_FM25V02_Init();
	FDC2X14_Init();
	LED_Init();
//	ESWDG_Init();

	TIM3_Int_Init(71,499);//500us定时 	 //72M晶振
	OLED_Init();
	ADS1118_Init();
	delay_ms(100);
	OLED_ShowString(0,24,"CH2:");
	OLED_ShowString(0,36,"CH3:");
	OLED_Refresh_Gram();
	while(1)
	{
//**********************ADS模拟量采集*************************//
		ADS1118_TestOnce();
//		voatage=(float)Voltage_Value*(2.048/32768);
		CH0_VOLTAGE_VALUE=(float)Cj_Value0*(4.096/32768);
		CH1_VOLTAGE_VALUE=(float)Cj_Value1*(4.096/32768);
		CH2_VOLTAGE_VALUE=(float)Cj_Value2*(4.096/32768);
		CH3_VOLTAGE_VALUE=(float)Cj_Value3*(4.096/32768);		
		
		sprintf (str_voltage,"%5d",Voltage_Value);		
		sprintf (str2,"%.4f",CH2_VOLTAGE_VALUE);		
		sprintf (str3,"%.4f",CH3_VOLTAGE_VALUE);

		OLED_ShowString(0,0,str_voltage);
//		OLED_ShowString(32,12,str1);
		OLED_ShowString(32,24,str2);
		OLED_ShowString(32,36,str3);
		OLED_Refresh_Gram();
		
//**********************************************************sqrt()//		
//		if(fivemsec_flag==1)
//		{
//			fivemsec_flag=0;
//			EWDG_Feed();
//			ChValue[0] = FDC2X14ReadCH(1);
//			ChValue[1] = FDC2X14ReadCH(2);
// 			ChValue[2] = FDC2X14ReadCH(3);
// 			ChValue[3] = FDC2X14ReadCH(4);
//			printf("ChValue[0]:%d---%x\r\n",ChValue[0],ChValue[0]);
		
		
//			printf("ChValue[1]:%d---%x\r\n",ChValue[1],ChValue[1]);
//			printf("ChValue[2]:%d---%x\r\n",ChValue[2],ChValue[2]);
//			printf("ChValue[3]:%d---%x\r\n",ChValue[3],ChValue[3]);
			//SendSS();
//		}
		
//**********************************************************//
		ChValue[0] = FDC2X14ReadCH(1);
		ChValue[1] = FDC2X14ReadCH(2);
		if(ChValue[0]<14026893)
			GPIO_ResetBits(GPIOE,GPIO_Pin_5 | GPIO_Pin_6);
		if(ChValue[1]<13600715)		
			GPIO_SetBits(GPIOE,GPIO_Pin_5 | GPIO_Pin_6);
 	}
}

//LED IO初始化
void LED_Init(void)
{    	 
 	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6; // 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOE,GPIO_Pin_5 | GPIO_Pin_6);//GPIOF9,F10设置高，灯灭

}

