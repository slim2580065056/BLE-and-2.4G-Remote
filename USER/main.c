/***************************@PinMap*****************************************												
*	@NRF24L01		CSN->B12	SCK->B13	MISO->B14		MOSI->B15		IRQ->A8		CE->A11
*	@MPU6050		SCL->B8		SDA->B9		INT->B10
*	@OLED				SCL->B6		SDA->B7
*	@Right_Lever  SW->B11		X->B1(ADC_IN9)		Y->B0(ADC_IN8)
*	@Left_Lever		SW->A12		X->A1(ADC_IN1)		Y->A0(ADC_IN0)
*	@Toggle_Switch	1->B3			2->B4  		3->B5
*	@Key				Key1->C15		Key2->C14		Key3->C13
*	@HC-05			Rx->A2		Tx->A3
*	@USART1			Tx->A9		Rx->A10
*	@Beep				beep->A12
*	@LED				A4  A5  A6
*	@Voltage		ADCx
*	@Control_Pro  5ms_Interrupt to control Program
***************************************************************************/

#include "stm32f10x.h"
#include "sys.h"

extern u16 AD_Value[4];
extern u8 Key_Val;
extern u16 MPU_Value[3];
extern Bit_Flag Flag;

u8 temp[]={0x5a};
//u8 ASCII2[]={" !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_'abcdefghijklmnopqrstuvwxyz"};

int main(void)
{
//	u8 i,temp_buf[15]={'L','S','H','@','$',};
	RCC_Config();										//ϵͳʱ�ӳ�ʼ��
	Delay_Config();									//��ʱ������ʼ��
	USART1_Config(115200);					//����1��ʼ����������115200bps
	BLE_Config(9600);								//��������ͨ�ų�ʼ��		/////������ʱ��ɨ�跽ʽ�շ�������֮���Ϊ�жϷ�ʽ�����ʵʱ��
		USART_SendData(USART2,'1');		//�����������ͣ����ͳɹ�������λ����'1'
	LED_Config();										//LED���ã���ʼ������
	Beep_Config();									//Beep��������ʼ��
	Switch_Config();								//���뿪�س�ʼ��
	Key_Config();										//�������س�ʼ��
	I2C_GPIO_Config();							//��ʹ��IIC��IO���ṩMPU6050ͨ��ʹ��
	MPU6050_Init();									//MPU6050��ʼ��	
	SPI2_NRF_CONFIG();							//����SPI2������NRF24L01ͨ��
	(0==NRF_Check())?LED1_Set(0):LED3_Set(0);
	Control_Lever_Config();					//ҡ��SW���ؼ�ADC����
	OLED_Init();										//OLED��ʼ��	
		Start_Pic();									//OLED��������
	TIM2_Config();									//TIM2ʱ������
	printf("test\r\n");	
	while(1)
	{
		Show_Screen();
		if(Flag.LEV_BLE_Flag==1)			LED1_Set(2),LED2_Set(0),LED3_Set(0),LEV_BLE();
		else if(Flag.MPU_BLE_Flag==1)	LED1_Set(2),LED2_Set(2),LED3_Set(0),MPU_BLE();
		else if(Flag.LEV_NRF_Flag==1)	LED1_Set(0),LED2_Set(0),LEV_NRF();
		else if(Flag.MPU_NRF_Flag==1)	LED1_Set(0),LED2_Set(2),MPU_NRF();
		else if(Flag.NRF_BLE_Flag==1)	LED1_Set(2),LED2_Set(0),LED3_Set(2),NRF_BLE();
		else if(Flag.BLE_NRF_Flag==1)	LED1_Set(2),LED2_Set(0),BLE_NRF();
		if(Flag.SET_Flag==0 && Key_Val==1)	OLED_Clear(),OLED_Init();
		//add you code in here
	}
}





