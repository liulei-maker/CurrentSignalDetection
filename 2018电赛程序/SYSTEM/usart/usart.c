#define USART_C

#include "sys.h"
#include "usart.h"	
#include "timer.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif


u8 SendBuf[256];
u32 sendPtr=0;
u32 readPtr=0;
u8 Send_flag=0; // ���ͱ�־λ
u32 num=0;
//////////////////////////////////////////////////////////////////////////////////	 
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
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
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 USART_TX_BUF[USART_REC_LEN];     //���ͻ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  

//��ʼ��IO ����1 
//bound:������
void uart1_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART1);  //��λ����1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
	USART_ITConfig(USART1,USART_IT_TXE,ENABLE);//�������ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 
		
	sendPtr=0;
	readPtr=0;

}
//��ʼ��IO ����2 
//bound:������
void uart2_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART2��GPIOAʱ��
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
		USART_DeInit(USART2);  //��λ����2
	 //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3

   //Usart2 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
		//USART ��ʼ������

		USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART2, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
		USART_ITConfig(USART2,USART_IT_TXE,ENABLE);//�������ж�
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
}

//��ʼ��IO ����3 
//bound:������
void uart3_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��USART3��GPIOBʱ��
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
		USART_DeInit(USART3);  //��λ����3
	 //USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��PB.10
   
    //USART3_RX	  PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //��ʼ��PB.11

   //Usart3 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
		//USART ��ʼ������

		USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART3, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�
		USART_ITConfig(USART3,USART_IT_TXE,ENABLE);//�������ж�
    USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 
}

//��ʼ��IO ����4 
//bound:������
// void uart4_init(u32 bound){
//     //GPIO�˿�����
//     GPIO_InitTypeDef GPIO_InitStructure;
// 		USART_InitTypeDef USART_InitStructure;
// 		NVIC_InitTypeDef NVIC_InitStructure;
// 	 
// 		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��UART4��GPIOCʱ��
// 		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
// 		USART_DeInit(UART4);  //��λ����4
// 	 //UART4_TX   PC.10
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
//     GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��PC.10
//    
//     //UART4_RX	  PC.11
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
//     GPIO_Init(GPIOC, &GPIO_InitStructure);  //��ʼ��PC.11

// //    //Uart4 NVIC ����

// //     NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
// // 		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
// // 		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
// // 		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
// // 		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
//   
// 		//USART ��ʼ������

// 		USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
// 		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
// 		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
// 		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
// 		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
// 		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

//     USART_Init(UART4, &USART_InitStructure); //��ʼ������
//     USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�����ж�
// 		USART_ITConfig(UART4,USART_IT_TXE,ENABLE);//�������ж�
//     USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ��� 
// 		
// 		sendPtr4=0;
// 	  readPtr4=0;
// }


// //��ʼ��IO ����5 
// //bound:������
// void uart5_init(u32 bound){
//     //GPIO�˿�����
//     GPIO_InitTypeDef GPIO_InitStructure;
// 		USART_InitTypeDef USART_InitStructure;
// 		NVIC_InitTypeDef NVIC_InitStructure;
// 	 
// 		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	//ʹ��UART5��GPIOC,GPIODʱ��
// 		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
// 		USART_DeInit(UART4);  //��λ����5
// 	 //UART5_TX   PC.12
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PC.12
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
//     GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��PC.12
//    
//     //UART5_RX	  PD.2
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
//     GPIO_Init(GPIOD, &GPIO_InitStructure);  //��ʼ��PD.2

// //    //Uart5 NVIC ����

// //     NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
// // 		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
// // 		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
// // 		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
// // 		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
//   
// 		//USART ��ʼ������

// 		USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
// 		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
// 		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
// 		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
// 		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
// 		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

//     USART_Init(UART5, &USART_InitStructure); //��ʼ������
//     USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//�����ж�
// 		//USART_ITConfig(UART5,USART_IT_TXE,ENABLE);//�������ж�
//     USART_Cmd(UART5, ENABLE);                    //ʹ�ܴ��� 
// }


void USART1_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
		if(USART_GetFlagStatus(USART1,USART_FLAG_ORE)==SET)
		{
			USART_ReceiveData(USART1);
			USART_ClearFlag(USART1,USART_FLAG_ORE);        //??ORE
		}
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
		{
			Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
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
				 USART_ITConfig(USART1,USART_IT_TXE,DISABLE);//�رշ����ж�
 				 sendPtr=readPtr=0;
				 Send_flag=0;
			 }
		 }
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
}

void USART2_IRQHandler(void)                	//����2�жϷ������
	{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
		if(USART_GetFlagStatus(USART2,USART_FLAG_ORE)==SET)
		{
			USART_ReceiveData(USART2);
			USART_ClearFlag(USART2,USART_FLAG_ORE);        //??ORE
		}
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
		{
			Res =USART_ReceiveData(USART2);//(USART2->DR);	//��ȡ���յ�������
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
				 USART_ITConfig(USART2,USART_IT_TXE,DISABLE);//�رշ����ж�
 				 sendPtr=readPtr=0;
				 Send_flag=0;
			 }
		 }
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
}

void USART3_IRQHandler(void)                	//����3�жϷ������
	{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
		
		if(USART_GetFlagStatus(USART3,USART_FLAG_ORE)==SET)
		{
			USART_ReceiveData(USART3);
			USART_ClearFlag(USART3,USART_FLAG_ORE);        //??ORE
		}
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�
		{
			Res =USART_ReceiveData(USART3);//(USART3->DR);	//��ȡ���յ�������
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
				 USART_ITConfig(USART3,USART_IT_TXE,DISABLE);//�رշ����ж�
 				 sendPtr=readPtr=0;
				 Send_flag=0;
			 }
		 }
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
}
void UART4_IRQHandler(void)                	//����4�жϷ������
{
	u8 Res;
	if(USART_GetFlagStatus(UART4,USART_FLAG_ORE)==SET)
		{
			USART_ReceiveData(UART4);
			USART_ClearFlag(UART4,USART_FLAG_ORE);        //??ORE
		}
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�
		{
			Res =USART_ReceiveData(UART4);//(UART4->DR);	//��ȡ���յ�������
			usart4_timer=1;
			usart4_count=0;
			
			USART4_RX_BUF[USART4_RX_STA&0X3FFF]=Res ;
			USART4_RX_STA++;
      if(USART4_RX_STA>=USART_REC_LEN)	USART4_RX_STA=0;		
     } 
		 if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET)//�����ж�
		 {
			 USART_ClearITPendingBit(UART4,USART_IT_TXE);
			 USART_SendData(UART4,SendBuf[sendPtr4++]);
			 if(sendPtr4>=readPtr4)
			 {
				 USART_ITConfig(UART4,USART_IT_TXE,DISABLE);//�رշ����ж�
				 sendPtr+=sendPtr4;
				 readPtr += readPtr4;
 				 sendPtr4=readPtr4=0;
				 Send_flag=0;
			 }
		 }
}
void UART5_IRQHandler(void)                	//����5�жϷ������
{
	u8 Res;
	 if(USART_GetFlagStatus(UART5,USART_FLAG_ORE)==SET)
		{
			USART_ReceiveData(UART5);
			USART_ClearFlag(UART5,USART_FLAG_ORE);        //??ORE
		}
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //�����ж�
		{
			Res =USART_ReceiveData(UART5);//(UART5->DR);	//��ȡ���յ�������
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
		USART_SendData(USART1, data[t]);//�򴮿�1��������
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
}
void Usart2_Send(u8 data[],u8 len)
{
	u8 t=0;
	for(t=0;t<len;t++)
	{
		USART_SendData(USART2, data[t]);//�򴮿�2��������
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
}
void Usart3_Send(u8 data[],u8 len)
{
	u8 t=0;
	for(t=0;t<len;t++)
	{
		USART_SendData(USART3, data[t]);//�򴮿�3��������
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
}
void Usart4_Send(u8 data[],u8 len)
{
	u8 t=0;
	for(t=0;t<len;t++)
	{
		USART_SendData(UART4, data[t]);//�򴮿�4��������
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
}
void Usart5_Send(u8 data[],u8 len)
{
	u8 t=0;
	for(t=0;t<len;t++)
	{
		USART_SendData(UART5, data[t]);//�򴮿�5��������
		while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
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

