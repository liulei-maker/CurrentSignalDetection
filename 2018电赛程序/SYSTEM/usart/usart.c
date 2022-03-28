#define USART_C

#include "sys.h"
#include "usart.h"	
#include "timer.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif


u8 SendBuf[256];
u32 sendPtr=0;
u32 readPtr=0;
u8 Send_flag=0; // 发送标志位
u32 num=0;
//////////////////////////////////////////////////////////////////////////////////	 
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART_TX_BUF[USART_REC_LEN];     //发送缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  

//初始化IO 串口1 
//bound:波特率
void uart1_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART1);  //复位串口1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

   //Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
	USART_ITConfig(USART1,USART_IT_TXE,ENABLE);//开启发中断
	USART_Cmd(USART1, ENABLE);                    //使能串口 
		
	sendPtr=0;
	readPtr=0;

}
//初始化IO 串口2 
//bound:波特率
void uart2_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART2，GPIOA时钟
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
		USART_DeInit(USART2);  //复位串口2
	 //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3

   //Usart2 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
		//USART 初始化设置

		USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART2, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
		USART_ITConfig(USART2,USART_IT_TXE,ENABLE);//开启发中断
    USART_Cmd(USART2, ENABLE);                    //使能串口 
}

//初始化IO 串口3 
//bound:波特率
void uart3_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能USART3，GPIOB时钟
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
		USART_DeInit(USART3);  //复位串口3
	 //USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PB.10
   
    //USART3_RX	  PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PB.11

   //Usart3 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
		//USART 初始化设置

		USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART3, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
		USART_ITConfig(USART3,USART_IT_TXE,ENABLE);//开启发中断
    USART_Cmd(USART3, ENABLE);                    //使能串口 
}

//初始化IO 串口4 
//bound:波特率
// void uart4_init(u32 bound){
//     //GPIO端口设置
//     GPIO_InitTypeDef GPIO_InitStructure;
// 		USART_InitTypeDef USART_InitStructure;
// 		NVIC_InitTypeDef NVIC_InitStructure;
// 	 
// 		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能UART4，GPIOC时钟
// 		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
// 		USART_DeInit(UART4);  //复位串口4
// 	 //UART4_TX   PC.10
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
//     GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PC.10
//    
//     //UART4_RX	  PC.11
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
//     GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PC.11

// //    //Uart4 NVIC 配置

// //     NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
// // 		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
// // 		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
// // 		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
// // 		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
//   
// 		//USART 初始化设置

// 		USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
// 		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
// 		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
// 		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
// 		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
// 		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

//     USART_Init(UART4, &USART_InitStructure); //初始化串口
//     USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断
// 		USART_ITConfig(UART4,USART_IT_TXE,ENABLE);//开启发中断
//     USART_Cmd(UART4, ENABLE);                    //使能串口 
// 		
// 		sendPtr4=0;
// 	  readPtr4=0;
// }


// //初始化IO 串口5 
// //bound:波特率
// void uart5_init(u32 bound){
//     //GPIO端口设置
//     GPIO_InitTypeDef GPIO_InitStructure;
// 		USART_InitTypeDef USART_InitStructure;
// 		NVIC_InitTypeDef NVIC_InitStructure;
// 	 
// 		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	//使能UART5，GPIOC,GPIOD时钟
// 		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
// 		USART_DeInit(UART4);  //复位串口5
// 	 //UART5_TX   PC.12
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PC.12
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
//     GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PC.12
//    
//     //UART5_RX	  PD.2
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
//     GPIO_Init(GPIOD, &GPIO_InitStructure);  //初始化PD.2

// //    //Uart5 NVIC 配置

// //     NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
// // 		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
// // 		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
// // 		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
// // 		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
//   
// 		//USART 初始化设置

// 		USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
// 		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
// 		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
// 		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
// 		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
// 		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

//     USART_Init(UART5, &USART_InitStructure); //初始化串口
//     USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启中断
// 		//USART_ITConfig(UART5,USART_IT_TXE,ENABLE);//开启发中断
//     USART_Cmd(UART5, ENABLE);                    //使能串口 
// }


void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();    
#endif
		if(USART_GetFlagStatus(USART1,USART_FLAG_ORE)==SET)
		{
			USART_ReceiveData(USART1);
			USART_ClearFlag(USART1,USART_FLAG_ORE);        //??ORE
		}
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
		{
			Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
			usart1_timer=1;
			usart1_count=0;
			USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
			USART1_RX_STA++; 		
			if(USART1_RX_STA>=USART_REC_LEN)	USART1_RX_STA=0;
     } 
		 if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
		 {
			 USART_ClearITPendingBit(USART1,USART_IT_TXE);
			 USART_SendData(USART1,SendBuf[sendPtr]);
			 sendPtr++;
			 if(sendPtr>=readPtr)
			 {
				 USART_ITConfig(USART1,USART_IT_TXE,DISABLE);//关闭发送中断
 				 sendPtr=readPtr=0;
				 Send_flag=0;
			 }
		 }
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
}

void USART2_IRQHandler(void)                	//串口2中断服务程序
	{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();    
#endif
		if(USART_GetFlagStatus(USART2,USART_FLAG_ORE)==SET)
		{
			USART_ReceiveData(USART2);
			USART_ClearFlag(USART2,USART_FLAG_ORE);        //??ORE
		}
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
		{
			Res =USART_ReceiveData(USART2);//(USART2->DR);	//读取接收到的数据
			usart1_timer=1;
			usart1_count=0;
			USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
			USART2_RX_STA++; 		 
     } 
		 if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
		 {
			 USART_ClearITPendingBit(USART2,USART_IT_TXE);
			 USART_SendData(USART2,SendBuf[sendPtr]);
			 sendPtr++;
			 if(sendPtr>=readPtr)
			 {
				 USART_ITConfig(USART2,USART_IT_TXE,DISABLE);//关闭发送中断
 				 sendPtr=readPtr=0;
				 Send_flag=0;
			 }
		 }
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
}

void USART3_IRQHandler(void)                	//串口3中断服务程序
	{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();    
#endif
		
		if(USART_GetFlagStatus(USART3,USART_FLAG_ORE)==SET)
		{
			USART_ReceiveData(USART3);
			USART_ClearFlag(USART3,USART_FLAG_ORE);        //??ORE
		}
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
		{
			Res =USART_ReceiveData(USART3);//(USART3->DR);	//读取接收到的数据
			usart1_timer=1;
			USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
			USART3_RX_STA++; 		 
     } 
		 if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
		 {
			 USART_ClearITPendingBit(USART3,USART_IT_TXE);
			 USART_SendData(USART3,SendBuf[sendPtr]);
			 sendPtr++;
			 if(sendPtr>=readPtr)
			 {
				 USART_ITConfig(USART3,USART_IT_TXE,DISABLE);//关闭发送中断
 				 sendPtr=readPtr=0;
				 Send_flag=0;
			 }
		 }
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
}
void UART4_IRQHandler(void)                	//串口4中断服务程序
{
	u8 Res;
	if(USART_GetFlagStatus(UART4,USART_FLAG_ORE)==SET)
		{
			USART_ReceiveData(UART4);
			USART_ClearFlag(UART4,USART_FLAG_ORE);        //??ORE
		}
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断
		{
			Res =USART_ReceiveData(UART4);//(UART4->DR);	//读取接收到的数据
			usart4_timer=1;
			usart4_count=0;
			
			USART4_RX_BUF[USART4_RX_STA&0X3FFF]=Res ;
			USART4_RX_STA++;
      if(USART4_RX_STA>=USART_REC_LEN)	USART4_RX_STA=0;		
     } 
		 if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET)//发送中断
		 {
			 USART_ClearITPendingBit(UART4,USART_IT_TXE);
			 USART_SendData(UART4,SendBuf[sendPtr4++]);
			 if(sendPtr4>=readPtr4)
			 {
				 USART_ITConfig(UART4,USART_IT_TXE,DISABLE);//关闭发送中断
				 sendPtr+=sendPtr4;
				 readPtr += readPtr4;
 				 sendPtr4=readPtr4=0;
				 Send_flag=0;
			 }
		 }
}
void UART5_IRQHandler(void)                	//串口5中断服务程序
{
	u8 Res;
	 if(USART_GetFlagStatus(UART5,USART_FLAG_ORE)==SET)
		{
			USART_ReceiveData(UART5);
			USART_ClearFlag(UART5,USART_FLAG_ORE);        //??ORE
		}
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //接收中断
		{
			Res =USART_ReceiveData(UART5);//(UART5->DR);	//读取接收到的数据
			USART5_RX_BUF[USART5_RX_STA&0X3FFF]=Res ;
			USART5_RX_STA++; 
			if(USART5_RX_STA	>=USART_REC_LEN)
			{
				USART5_RX_STA=0;
			}			
     } 
}
void Usart1_Send(u8 data[],u8 len)
{
	u8 t=0;
	for(t=0;t<len;t++)
	{
		USART_SendData(USART1, data[t]);//向串口1发送数据
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	}
}
void Usart2_Send(u8 data[],u8 len)
{
	u8 t=0;
	for(t=0;t<len;t++)
	{
		USART_SendData(USART2, data[t]);//向串口2发送数据
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
	}
}
void Usart3_Send(u8 data[],u8 len)
{
	u8 t=0;
	for(t=0;t<len;t++)
	{
		USART_SendData(USART3, data[t]);//向串口3发送数据
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
	}
}
void Usart4_Send(u8 data[],u8 len)
{
	u8 t=0;
	for(t=0;t<len;t++)
	{
		USART_SendData(UART4, data[t]);//向串口4发送数据
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);//等待发送结束
	}
}
void Usart5_Send(u8 data[],u8 len)
{
	u8 t=0;
	for(t=0;t<len;t++)
	{
		USART_SendData(UART5, data[t]);//向串口5发送数据
		while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);//等待发送结束
	}
}
void Usart1_Sendbuf(u8 data[],u8 data2[],u16 count)
{
	u16 tt;
	if(readPtr+count<=256)
	{
		for(tt=0;tt<count;tt++)
		{
			SendBuf[readPtr++] =data2[tt];
		}
	}					
	if(sendPtr == 0 && readPtr>0)
	{
		USART_ITConfig(USART1,USART_IT_TXE,ENABLE);
	}
}
void Usart4_Sendbuf(u8 data[],u8 data2[],u16 count)
{
	u16 tt;
	if(readPtr4+count<=256)
	{
		for(tt=0;tt<count;tt++)
		{
			SendBuf[readPtr4++] =data2[tt];
		}
	}					
	if(sendPtr4 == 0 && readPtr4>0)
	{
		USART_ITConfig(UART4,USART_IT_TXE,ENABLE);
	}
}
#endif	

