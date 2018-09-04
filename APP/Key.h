#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x.h"
#include "sys.h"

/*	������ת��Ϊ��ֵ	*/
#define KEY_VAL		(PCin(15) | PCin(14)<<1 | PCin(13)<<2 | PAin(15)<<3 | PBin(11)<<4)





void Key_Config(void);
u8 Key_Scan(void);
u8 Get_Key(void);


#endif


