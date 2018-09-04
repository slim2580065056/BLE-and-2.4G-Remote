//	@NRF24L01		CSN->B12	SCK->B13	MISO->B14		MOSI->B15		IRQ->A8		CE->A11
//	ȫ˫������/��ģʽ

#include "NRF.h"

const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ

/**
	*	@brief 	��ʼ��SPI
	*	@param	None
	*	@retval	Nome
	*/
void SPI2_NRF_CONFIG(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	
	//SCK->B13 & MISO->B14 & MOSI->B15    --> AF_PP
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;		//���츴�����
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	GPIO_SetBits(GPIOB,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);	//�ø�
	//IRQ->A8 -->IPU     	CE->A11 -->Out_PP
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8;
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;		//�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//CSN->B12-->Out_PP
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;		//�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz ;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;				//ȫ˫��ģʽ
  SPI_InitStructure.SPI_Mode=SPI_Mode_Master;    					//������SPI
  SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;        	//�շ�����Ϊ8bit
  SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low ; 							//ʱ�����յ�
  SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge; 							//���ݲ������һ��ʱ����
  SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;                 //���ʹ��Ƭѡ
  SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_16;   //������Ԥ��ƵֵΪ16
  SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;        //��λ��ǰ
	SPI_InitStructure.SPI_CRCPolynomial = 7;								//CRCУ��	
	SPI_Init(SPI2,&SPI_InitStructure);
	
	SPI_Cmd(SPI2,ENABLE);				//ʹ��SPI
	
	NRF_CE=0;								//ʹ��24L01
	NRF_CSN=1;							//SPIƬѡȡ��
}

/**
	*	@brief 	����SPI�Ĵ�������
	*	@param	��Ƶֵ
	*	@retval	None
	*/
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1&=0XFFC7;
	SPI2->CR1|=SPI_BaudRatePrescaler; //����SPI2�ٶ�
	SPI_Cmd(SPI2,ENABLE);
}
/**
	*	@brief 	SPIд�ֽ�
	*	@param	д�������
	*	@retval	��ȡ���ֽ�
	*/
u8 SPI2_WRByte(u8 TxData)
{
	u8 retry=0;
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET)	//�ȴ��������
	{
		retry++; 
		if(retry>200) return 0;
	}
	SPI_I2S_SendData(SPI2,TxData);//ͨ��SPI2����һ���ֽ� 
	retry=0;
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==RESET)	//�ȴ��������
	{
		retry++;
		if(retry>200) return 0;
	}
	return SPI_I2S_ReceiveData(SPI2);//����SPI2���յ�������
}

/**
	*	@brief 	���NRF24L01�Ƿ����
	*	@param	None
	*	@retval	0������		1��������
	*/
u8 NRF_Check(void)
{
	u8 buf[5]={0xA5,0xA5,0xA5,0xA5,0xA5};
	u8 i;
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4);	//36/4=9MHz 
	NRF_WBuf(NRF_WRITE_REG+TX_ADDR,buf,5);		//д��5���ֽڵĵ�ַ
	NRF_RBuf(TX_ADDR,buf,5);		//����д��ĵ�ַ
	for(i=0;i<5;i++)
	{
		if(buf[i]!=0xA5)
			break;
	}
	if(5!=i)	return 1;	//forѭ����;�˳������NRF24L01�쳣
	return 0;						//��⵽NRF24L01����
}

/**
	*	@brief 	SPIд�Ĵ���
	*	@param	�Ĵ�����ַ��д�������
	*	@retval	None
	*/
u8 NRF_WReg(u8 reg,u8 data)
{
	u8 status;
	NRF_CSN=0;									//ʹ��SPI����
	status = SPI2_WRByte(reg);	//���ͼĴ�����
	SPI2_WRByte(data);					//д��Ĵ�����ֵ
	NRF_CSN=1;									//��ֹSPI����
	return status;
}

/**
	*	@brief 	SPI��ȡ�Ĵ���ֵ
	*	@param	Ҫ��ȡ�ļĴ�����ַ
	*	@retval ��ȡ��ֵ
	*/
u8 NRF_RReg(u8 reg)
{
	u8 data;
	NRF_CSN=0;									//ʹ��SPI����
	SPI2_WRByte(reg);						//�Ĵ�����ַ
	data=SPI2_WRByte(0xff);			//��ȡ�Ĵ�����
	NRF_CSN=1;									//��ֹSPI����
	return data;
}

/**
	*	@brief 	���ƶ��ļĴ�����ַ�ж���ָ�����ȵ���ֵ
	*	@param	�Ĵ�����ַ�����ݳ��ȣ������׵�ַָ��
	*	@retval	״ֵ̬
	*/
u8 NRF_RBuf(u8 reg,u8 *pBuf,u8 len)
{
	u8 status,i;
	NRF_CSN=0;
	status=SPI2_WRByte(reg);
	for(i=0;i<len;i++)
		pBuf[i]=SPI2_WRByte(0xff);
	NRF_CSN=1;
	return status;
}

/**
	*	@brief 	��ָ��λ��д��ָ�����ȵ�����
	*	@param	�Ĵ�����ַ������ָ�룬���ݳ���
	*	@retval	״ֵ̬
	*/
u8 NRF_WBuf(u8 reg,u8 *pBuf,u8 len)
{
	u8 status,i;
	NRF_CSN=0;
	status=SPI2_WRByte(reg);
	for(i=0;i<len;i++)
		SPI2_WRByte(*pBuf++);
	NRF_CSN=1;
	return status;
}

/**
	*	@brief 	����NRF24L01����һ������
	*	@param	�����������׵�ַ
	*	@retval ����������
	*/
u8 NRF_TxPacket(u8 *txbuf)
{
	u8 status;
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4);
	NRF_CE=0;
	NRF_WBuf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);	//д���ݵ�TxBuf
	NRF_CE=1;		//��������
	while(NRF_IRQ!=0);		//�ȴ��������
	status=NRF_RReg(STATUS);	//��ȡ״̬�Ĵ���ֵ
	NRF_WReg(NRF_WRITE_REG+STATUS,status);	//���TX����DSMAX_RT�жϱ�ʶ
	if(status&MAX_TX)		//�ﵽ����ط�����
	{
		NRF_WReg(FLUSH_TX,0xff);	//���TX_FIFO�Ĵ���
		return MAX_TX;
	}
	if(status&TX_OK)	//�������
		return TX_OK;
	return 0xff;		//����ʧ��
}


/**
	*	@brief 	����NRF24L01 ����һ������
	*	@param	�����������׵�ַ
	*	@retval 0���������  �������������
	*/
u8 NRF_RxPacket(u8 *rxbuf)
{
	u8 status;
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4);
	status=NRF_RReg(STATUS);		//��ȡ״̬�Ĵ���ֵ
	NRF_WReg(NRF_WRITE_REG+STATUS,status);		//���TX_DSMAX_RT�жϱ�ʶ
	if(status&RX_OK)		//���ܵ�����
	{
		NRF_RBuf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		NRF_WReg(FLUSH_RX,0xff);//���RX_FIFO�Ĵ���
		return 0;
	}
	return 1;   //û�н��յ�����
}

/**
	*	@brief 	��ʼ��NRF24L01ΪRXģʽ
	*	@param  None
	*	@retval None
	*/
void NRF_RX_Mode(void)
{
	NRF_CE=0;
	NRF_WBuf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);
	NRF_WReg(NRF_WRITE_REG+EN_AA,0x01);	//ʹ��ͨ��0���Զ�Ӧ��
	NRF_WReg(NRF_WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ
	NRF_WReg(NRF_WRITE_REG+RF_CH,40);				//����RFͨ��Ƶ��
	NRF_WReg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ������Ч���ݿ��
	NRF_WReg(NRF_WRITE_REG+RF_SETUP,0x0f);	//����TX����0db���棬2Mbps,���������濪��
	NRF_WReg(NRF_WRITE_REG+NRF_CONFIG,0x0f);		//���û�������ģʽ�Ĳ�����PWR_UP,EN_CRC,16BIT_CRC������ģʽ
	NRF_CE=1;		//�������ģʽ
}

/**
	*	@brief 	��ʼ��NRF24L01ΪTXģʽ
	*	@param  None
	*	@retval None
	*/
void NRF_Tx_Mode(void)
{
	NRF_CE=0;
	NRF_WBuf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);
	NRF_WBuf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//����TX�ڵ��ַ����ҪΪ��ʹ��ACK
	NRF_WReg(NRF_WRITE_REG+EN_AA,0x01);	//ʹ��ͨ��0���Զ�Ӧ��
	NRF_WReg(NRF_WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ
	NRF_WReg(NRF_WRITE_REG+SETUP_RETR,0x0a); //�����Զ��ط����ʱ��500us+86us,������ط�����10��
	NRF_WReg(NRF_WRITE_REG+RF_CH,40);				//����RFͨ��40
	NRF_WReg(NRF_WRITE_REG+RF_SETUP,0x0f);	//����TX����0db���棬2Mbps,���������濪��
	NRF_WReg(NRF_WRITE_REG+NRF_CONFIG,0x0e);		//���û�������ģʽ�Ĳ�����PWR_UP,EN_CRC,16BIT_CRC������ģʽ�����������ж�
	NRF_CE=1;		//�������ģʽ��10us����������
}

//test
void NRF_Tx(u8 txbuf)
{
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4);
	NRF_CE=0;
	NRF_CSN=0;
	SPI2_WRByte(WR_TX_PLOAD);
	SPI2_WRByte(txbuf);
	NRF_CSN=1;
	NRF_CE=1;		//��������
}

