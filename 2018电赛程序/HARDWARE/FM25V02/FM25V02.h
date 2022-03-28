#ifndef __FM25V02_H
#define __FM25V02_H
#include "sys.h"

#define SPI2_FM25V02_CS PBout(12) //NSS针脚
#define FM25V02_W PGout(6) //W针脚 写保护开关 低写保护开 高写保护关闭

void SPI2_FM25V02_Init(void);//初始化FM25V02
void SPI2_FM25V02_Write(u16 address,char data);//写数据
char SPI2_FM25V02_Read(u16 address);//读数据
void SPI2_FM25V02_nWrite(u16 address,char *data,u16 len);//写多个数据
void SPI2_FM25V02_nRead(u16 address,char *data,u16 len);//读多个数据
#endif
