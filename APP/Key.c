//*	@Key				Key1->C15		Key2->C14		Key3->C13
//*	@Right_Lever  SW->B11		
//*	@Left_Lever		SW->A15		

#include "Key.h"

void Key_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);
	// Key1 / Key2 / Key3 -> IPU
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;	
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	// SW1 -> IPU
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU;		//SW������������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	// SW2 -> IPU
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU;		//SW������������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

/**
	*	@brief	just judge key press down
	*	@param  None
	*	@retval Key_Value
	*/
u8 Key_Scan(void)
{
	u8 i,temp;
	for(i=0;i<5;i++)													//	for(i=1;i<6;i++)
	{																					//	{
		temp=KEY_VAL>>i;												//		temp=KEY_VAL<<i;
		if(0x1f!=KEY_VAL && 0==(temp & 0x01))		//		if(0x1f!=KEY_VAL && 0==(temp & 0x20))		//�м�����
			return i+1;						//���ؼ�ֵ			//			return i;	
	}																					//	}
	return 0;																	//	return 0;
}

/**
	*	@brief	��ȡ��ֵ
	*	@param  None
	*	@retval Key_Value
	*/
u8 Get_Key(void)
{
	static u8 n,status=0,last_val;
	u8 key_val;
	key_val=Key_Scan();									//��ֵ��ֵ��key_val
	if(0!=key_val && key_val==last_val)	//����ڿ���ģʽ���м�����
	{
		if(0==status)	status=1;						//�������±�ʶ
		else 	n++;
	}
	if(1==status && key_val==0)					//�������±�ʶʹ���Ұ����Ѿ��ͷ�
	{
		status=0;													//�����ͷű�ʶ
		if(n>2 && n<25)	//40ms ~ 500ms		//�̰�����ʱ����ƣ����ݿ�������ʱ����ƴ���
		{
			n=0;														//�������
			return last_val;								//���ؼ�ֵ
		}
		else if(n>=25)		//���� 500ms			//����ʱ�����
		{
			n=0;														//�������
			return last_val+10;							//������ֵ = (�̰���ֵ+10)
		}
		else 															//����ʱ����̱���Ϊ�ǰ�����������״̬
		{
			n=0;														//�������
			return 20;											//������״̬
		}
	}
	last_val=key_val;										//������һ�εļ�ֵ
	return 0;
}





