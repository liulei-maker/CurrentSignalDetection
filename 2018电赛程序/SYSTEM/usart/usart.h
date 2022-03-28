#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

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
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			256  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#if defined USART_C
u8 SendBuf[256];
u32 sendPtr;
u32 readPtr;	
u32 sendPtr4;
u32 readPtr4;	
u8 Send_flag;
u8  USART1_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
u8  USART1_TX_BUF[USART_REC_LEN]; //���ͻ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
u16 USART1_RX_STA;         		//����״̬���	
u8  USART2_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
u8  USART2_TX_BUF[USART_REC_LEN]; //���ͻ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
u16 USART2_RX_STA;         		//����״̬���	
u8  USART3_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
u8  USART3_TX_BUF[USART_REC_LEN]; //���ͻ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
u16 USART3_RX_STA;         		//����״̬���	
u8  USART4_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
u8  USART4_TX_BUF[USART_REC_LEN]; //���ͻ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
u16 USART4_RX_STA;         		//����״̬���	
u8  USART5_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
u8  USART5_TX_BUF[USART_REC_LEN]; //���ͻ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
u16 USART5_RX_STA;         		//����״̬���	

void uart1_init(u32 bound); //����1��ʼ��
void uart2_init(u32 bound); //����2��ʼ��
void uart3_init(u32 bound); //����3��ʼ��
void uart4_init(u32 bound); //����4��ʼ��
void uart5_init(u32 bound); //����5��ʼ��
void Usart1_Send(u8 data[],u8 len);//����1����
void Usart2_Send(u8 data[],u8 len);//����2����
void Usart3_Send(u8 data[],u8 len);//����3����
void Usart4_Send(u8 data[],u8 len);//����4����
void Usart5_Send(u8 data[],u8 len);//����5����
void Usart1_Sendbuf(u8 data[],u8 data2[],u16 count);
void Usart4_Sendbuf(u8 data[],u8 data2[],u16 count);
#else

extern u8 SendBuf[256];
extern u32 sendPtr;
extern u32 readPtr;	
extern u32 sendPtr4;
extern u32 readPtr4;
extern u8 Send_flag;
extern u8  USART1_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8  USART1_TX_BUF[USART_REC_LEN]; //���ͻ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART1_RX_STA;         		//����״̬���	
extern u8  USART2_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8  USART2_TX_BUF[USART_REC_LEN]; //���ͻ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART2_RX_STA;         		//����״̬���	
extern u8  USART3_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8  USART3_TX_BUF[USART_REC_LEN]; //���ͻ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART3_RX_STA;         		//����״̬���	
extern u8  USART4_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8  USART4_TX_BUF[USART_REC_LEN]; //���ͻ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART4_RX_STA;         		//����״̬���	
extern u8  USART5_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8  USART5_TX_BUF[USART_REC_LEN]; //���ͻ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART5_RX_STA;         		//����״̬���	

extern void uart1_init(u32 bound); //����1��ʼ��
extern void uart2_init(u32 bound); //����2��ʼ��
extern void uart3_init(u32 bound); //����3��ʼ��
extern void uart4_init(u32 bound); //����4��ʼ��
extern void uart5_init(u32 bound); //����5��ʼ��
extern void Usart1_Send(u8 data[],u8 len);//����1����
extern void Usart2_Send(u8 data[],u8 len);//����2����
extern void Usart3_Send(u8 data[],u8 len);//����3����
extern void Usart4_Send(u8 data[],u8 len);//����4����
extern void Usart5_Send(u8 data[],u8 len);//����5����
extern void Usart1_Sendbuf(u8 data[],u8 data2[],u16 count);
extern void Usart4_Sendbuf(u8 data[],u8 data2[],u16 count);

#endif
#endif


