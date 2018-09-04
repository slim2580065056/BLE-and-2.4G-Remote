//	@Toggle_Switch	1->B3			2->B4  		3->B5
#include "Toggle_Switch.h"

void Switch_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	/*!< JTAG-DP Disabled and SW-DP Enabled */
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//PB3��PB4Ĭ��ΪJlink�˿ڣ�ʹ��IO������ӳ��
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;			//�������룬Ĭ�Ͻӵأ��ӵ�ԴVCCʱΪ�ߵ�ƽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

u8 Switch_Code(void)
{
	static u8 code=0;		//��ʼ��Ϊ0
	code=((PBin(5)&0101)<<2 | (PBin(4)&0x01)<<1 | PBin(3));
	return code;
}

