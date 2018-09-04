//*	@Right_Lever  SW->B11		X->B1(ADC_IN9)		Y->B0(ADC_IN8)
//*	@Left_Lever		SW->A15		X->A1(ADC_IN1)		Y->A0(ADC_IN0)
//* @SW �� Key.c ������

#include "Lever.h"

u16 AD_Value[4];		//�������ADCת�������Ҳ��DMA��Ŀ���ַ
extern u8 ASCII[10];//={'0','1','2','3','4','5','6','7','8','9'};			//������ת��ΪASCII��
extern u8 AD_Flag[4];//={'a','b','c','d'};					//ҡ�˵������룬����ʶ���շ������ĸ�ҡ�˵�ADֵ

void Control_Lever_Config(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	ADC_InitTypeDef 	ADC_InitStructure;
	DMA_InitTypeDef   DMA_InitStructure;
	//=========================================================================================================
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);		//72/6=12,ADC���ת��ʱ�䲻����14MHz
	//=========================================================================================================
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | \
												 RCC_APB2Periph_AFIO | RCC_APB2Periph_ADC1,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	//X & Y ->ADC
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AIN;						//X/Yģ������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 |GPIO_Pin_1;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AIN;						//X/Yģ������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 |GPIO_Pin_1;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//ADC1_Config
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;  //��������ģʽ
  ADC_InitStructure.ADC_ScanConvMode=ENABLE;       	//ģ��ת��������ɨ��ģʽ
  ADC_InitStructure.ADC_ContinuousConvMode=ENABLE; 	//ģ��ת��������ת��ģʽ
  ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;    //��ʹ�ܴ���
  ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;                 //���ݸ�ʽΪ�Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel=4;  						//4��ͨ��ת��
	ADC_Init(ADC1,&ADC_InitStructure);
	
	/*	ADC1 regular channel11 Config	*/
	/*		����ָ��ADC�Ĺ���ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
				ADC1��ADCͨ��x,�������˳��ֵΪy������ʱ��Ϊ239.5��		*/
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_8,3,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_9,4,ADC_SampleTime_239Cycles5);
	
	//DMA1_Config
	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&ADC1->DR; 	//DMA����ADC����ַ
  DMA_InitStructure.DMA_MemoryBaseAddr=(u32)AD_Value;    		//DMA�ڴ����ַ
  DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;        	//�ڴ���Ϊ���ݴ����Ŀ�ĵ�
  DMA_InitStructure.DMA_BufferSize=4;         							//DMAͨ��DMA�������ݴ�С
  DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable; 		//�����ַ�Ĵ�������
  DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;   	//DMA�ڴ��ַ�Ĵ�������
  DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord; //���ݿ��16bit
  DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;					//���ݿ��16bit
  DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;         		//������ѭ������ģʽ
  DMA_InitStructure.DMA_Priority=DMA_Priority_High;     		//DMAͨ��ӵ�и����ȼ�
  DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;                //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��

	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
	
	ADC_Cmd(ADC1, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	//����ADC��DMA֧��
	ADC_DMACmd(ADC1,ENABLE);
	
	ADC_Cmd(ADC1, ENABLE);
	//��λADCУ׼�Ĵ���
	ADC_ResetCalibration(ADC1);		//��λ�ƶ�ADC1��У׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	//У׼ADC
	ADC_StartCalibration(ADC1);		//��ʼУ׼ADC1
	while(ADC_GetCalibrationStatus(ADC1));
}

//void AD_Num(void)
//{
//	u8 i,temp[4];
//	for(i=0;i<4;i++)
//	{
//		temp[i]=AD_Value[i]/410;
//		printf("%6d      ",temp[i]);
//		/*	���ݰ����������룬���ڴӻ�ʶ����յ����ĸ�ҡ�˵�ADֵ	*/
//		USART_SendData(USART2,(u8)ASCII[AD_Flag[i]]<<8 | (u8)ASCII[temp[i]]);
//		Delay_ms(10);				//ÿ��10ms����һ�Σ����������շ��쳣
//	}
//	printf("\r\n");
//}


