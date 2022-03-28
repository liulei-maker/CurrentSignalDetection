#ifndef __FM25V02_H
#define __FM25V02_H
#include "sys.h"

#define SPI2_FM25V02_CS PBout(12) //NSS���
#define FM25V02_W PGout(6) //W��� д�������� ��д������ ��д�����ر�

void SPI2_FM25V02_Init(void);//��ʼ��FM25V02
void SPI2_FM25V02_Write(u16 address,char data);//д����
char SPI2_FM25V02_Read(u16 address);//������
void SPI2_FM25V02_nWrite(u16 address,char *data,u16 len);//д�������
void SPI2_FM25V02_nRead(u16 address,char *data,u16 len);//���������
#endif
