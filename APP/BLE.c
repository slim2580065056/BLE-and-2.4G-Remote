//*	@HC-05->USART2		Tx->A2		Rx->A3

#include "BLE.h"

/**
	*	@brief  Configured Bluetooth of GPIO and USART2
	*	@param  BaudRate
	* @retval None
	*/
void BLE_Config(u32 BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	//Tx->A2		AF_PP
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;		//�����������	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//Rx->A3   Floating
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=BaudRate;
	USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//ȫ˫��
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength =USART_WordLength_8b;
	
	USART_Init(USART2,&USART_InitStructure);
	
	NVIC_Config(2,1,USART2_IRQn,2);
	
		/*	enable USART2	*/
	USART_Cmd(USART2,ENABLE);
	USART_ITConfig(USART2,USART_IT_RXNE, ENABLE);
}

/**
	*	@brief  Bluetooth to send data
	*	@param  lenth of data;data Pointer
	*	@retval None
	*/
void BLE_Tx(u16 data)
{
	if(USART_GetFlagStatus(USART2,USART_FLAG_TXE)!=RESET)
	{
		USART_ClearFlag(USART2,USART_FLAG_TXE);
		USART_SendData(USART2,data);
	}
}

/**
	*	@brief  Bluetooth to received data
	*	@param  None
	*	@retval received data
	*/
u16 BLE_Rx(void)
{
	u16 temp;
	if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE)!=RESET)
	{
		USART_ClearFlag(USART2,USART_FLAG_RXNE);
		temp=USART_ReceiveData(USART2);
	}
	return temp;
}

/**
	*	@brief	usart2�жϺ���
	*	@param	���ͺ����еķ�������
	*	@retval	None
	*/
void USART2_IRQHandler(u16 data)
{
	static u8 temp;
	void (*f1)(u16);		//���庯��ָ�룬�����ж�ʱ��ɨ����պͷ��ͺ���
	u16 (*f2)(void);
	f1=BLE_Tx;					//���庯��ָ��ָ���ͺ���
	f2=BLE_Rx;					//���庯��ָ��ָ����պ���
	temp=(*f2)();				//���պ����յ������ݸ�ֵ��temp
	(*f1)(temp);				//�����յ�������ͨ�����ͺ������ͳ�ȥ
	printf("%d\r\n",temp);	//��ӡ���պ����յ�������
}


