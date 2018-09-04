//	@TIM2_INT Interrupt to Control Program
//	�жϿ�������20ms

#include "Control.h"

typedef struct IMU_DATA
{
	float q0,q1,q2,q3;									//��Ԫ��	
	float ax,ay,az,gx,gy,gz;						//�����Ǻͼ��ٶȼƵ�ģ��������Ӧ��������
	float ex,ey,ez;											//�˲��ں�֮���������
	float PITCH;				//������
	float YAW;					//ƫ����
	float ROLL;					//�����

}IMU_param;
extern IMU_param IMU;
extern u16 AD_Value[4];		//ҡ�˵�AD����ֵ
extern u16 MPU_Value[3];	//MPU6050������ֵ
u8 ASCII[10]={'0','1','2','3','4','5','6','7','8','9'};			//������ת��ΪASCII��
Bit_Flag Flag={1,0,0,0,0,0,0};
u8 Key_Val;
u8 bit_k;

void TIM2_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	/*	Tout=((arr+1)*(psc+1))/Tclk	*/
	TIM_TimeBaseStructure.TIM_Prescaler=719;         				//Ԥ��Ƶ��72 000 000 / (719+1) = 1 000 00 Hz
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //������ʽ�����ϼ���
  TIM_TimeBaseStructure.TIM_Period= 1999;            				//�������ڣ�1 000 00 / (4999 + 1) = 50 Hz = 0.02 s = 20 ms
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //ʱ�Ӳ��ָ�	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	NVIC_Config(0,0,TIM2_IRQn,2);	
	TIM_ITConfig(TIM2,TIM_IT_Update , ENABLE);	
	TIM_Cmd(TIM2,ENABLE);	
}

void TIM2_IRQHandler(void)		//�жϺ��������ܴ���һЩ�򵥵Ĺ���,���ӵĳ��������������е���
{	
	if(TIM_GetFlagStatus(TIM2,TIM_IT_Update)!=RESET)
	{
		Key_Val=Get_Key();				//���жϳ����л�ȡ��ֵ
		DATA_Task();							//ͨ��������ȡʹ���Ǹ����ݴ��书�ܣ����ݴ��亯�����������е���
//		(0==Key_Val)?Key_Val:printf("%2d  ",Key_Val); 		//��ӡ������ֵ
		TIM_ClearFlag(TIM2,TIM_IT_Update);	
	}
}

/**
	*	@breif  ҡ��AD����ת�����������������ͣ����ղ��ú���ָ����е���
	*	@param  None
	*	@retval None
	*/
void DATA_Task(void)
{
	u8 static n;	
//	void (*FLB)(void);//	FLB=LEV_BLE;
//	void (*FMB)(void);//	FMB=MPU_BLE;
//	void (*FLN)(void);//	FLN=LEV_NRF;
//	void (*FMN)(void);//	FMN=MPU_NRF;
//	void (*FNB)(void);//	FNB=NRF_BLE;
//	void (*FBN)(void);//	FBN=BLE_NRF;
	if(Flag.SET_Flag==0 && Key_Val==3)		//��û�н������ù��ܵ�����°������ü�
	{
		Flag.SET_Flag=1;										//�������ù���,ͬʱ����ʾ�����иı�ΪSET		
		OLED_Area_Clear();
	}
	if(Flag.SET_Flag==1 && Key_Val==13)	
	{
		Flag.SET_Flag=0;										//������ģʽ�³����ü����˳�����ģʽ
	}
	if(Flag.SET_Flag==1 && Key_Val==2)		//������ģʽ��,����2Ϊѡ��ģʽ
	{
		switch(++n)
		{
			case 1:		bit_k=0x01;break;//LEV-->BLE
			case 2:		bit_k=0x02;break;//MPU-->BLE
			case 3:		bit_k=0x04;break;//LEV-->NRF
			case 4:		bit_k=0x08;break;//MPU-->NRF
			case 5:		bit_k=0x10;break;//NRF-->BLE
			case 6:		n=0;bit_k=0x20;break;//BLE-->NRF
			default:	bit_k=0xff;break;
		}
	}
	if((bit_k == 0x01) && Key_Val==1) 
	{
//		n=0;
		Flag.SET_Flag=0;
		OLED_Area_Clear();
		Flag.LEV_BLE_Flag=1; Flag.MPU_BLE_Flag=0; Flag.LEV_NRF_Flag=0;
		Flag.MPU_NRF_Flag=0; Flag.NRF_BLE_Flag=0; Flag.BLE_NRF_Flag=0;
	}
	if((bit_k == 0x02) && Key_Val==1) 
	{
//		n=0;
		Flag.SET_Flag=0;
		OLED_Area_Clear();
		Flag.LEV_BLE_Flag=0; Flag.MPU_BLE_Flag=1; Flag.LEV_NRF_Flag=0;
		Flag.MPU_NRF_Flag=0; Flag.NRF_BLE_Flag=0; Flag.BLE_NRF_Flag=0;
	}
	if((bit_k == 0x04) && Key_Val==1) 
	{
//		n=0;
		Flag.SET_Flag=0;
		OLED_Area_Clear();
		Flag.LEV_BLE_Flag=0; Flag.MPU_BLE_Flag=0; Flag.LEV_NRF_Flag=1;
		Flag.MPU_NRF_Flag=0; Flag.NRF_BLE_Flag=0; Flag.BLE_NRF_Flag=0;
	}
	if((bit_k == 0x08) && Key_Val==1) 
	{
	//	n=0;
		Flag.SET_Flag=0;
		OLED_Area_Clear();
		Flag.LEV_BLE_Flag=0; Flag.MPU_BLE_Flag=0; Flag.LEV_NRF_Flag=0;
		Flag.MPU_NRF_Flag=1; Flag.NRF_BLE_Flag=0; Flag.BLE_NRF_Flag=0;
	}
	if((bit_k == 0x10) && Key_Val==1)
	{
//		n=0;
		Flag.SET_Flag=0;
		OLED_Area_Clear();
		Flag.LEV_BLE_Flag=0; Flag.MPU_BLE_Flag=0; Flag.LEV_NRF_Flag=0;
		Flag.MPU_NRF_Flag=0; Flag.NRF_BLE_Flag=1; Flag.BLE_NRF_Flag=0;
	}		
	if((bit_k == 0x20) && Key_Val==1)
	{
//		n=0;
		Flag.SET_Flag=0;
		OLED_Area_Clear();
		Flag.LEV_BLE_Flag=0; Flag.MPU_BLE_Flag=0; Flag.LEV_NRF_Flag=0;
		Flag.MPU_NRF_Flag=0; Flag.NRF_BLE_Flag=0; Flag.BLE_NRF_Flag=1;
	}		
}

/**
	*	@breif  ҡ��AD����ת�����������������ͣ����ղ��ú���ָ����е���
	*	@param  None
	*	@retval None
	*/
void LEV_BLE(void)				//ҡ��������������
{
	u8 i;
	u8 static temp[4];
//	OLED_S6X8(24,30,"LEV");OLED_S6X8(24,80,"BLE");		//OLED��ʾ���䷽��
	BLE_Tx('L');						//���ͷ����豸ʶ����
	OLED_C6X8(56,40,'L');		//OLED��ʾ
	for(i=0;i<4;i++)
	{
		temp[i]=AD_Value[i]/410;		//����ҡ�˾��ȵ�,����ת��Ϊ10���ȼ����ȣ�����ASCII���շ�
		BLE_Tx(ASCII[temp[i]]);			//ÿ�η���һ��ת����ASCII��
		OLED_C6X8(56,48+8*i,ASCII[temp[i]]);	//OLED��ʾ���͵�����
	}
//	if(Key_Val!=0)	BLE_Tx('K'),BLE_Tx((Key_Val/10)+48),BLE_Tx((Key_Val%10)+48);	//�������ͼ�ֵ
}

/**
	*	@breif  MPU6050�������̬�������������ͣ����ղ��ú���ָ����е���
	*	@param  None
	*	@retval None
	*/
void MPU_BLE(void)				//MPU6050��������̬������������
{
	u8 i,j;
	u8 static temp[3][3];
//	OLED_S6X8(24,30,"MPU");OLED_S6X8(24,80,"BLE");		//OLED��ʾ���䷽��
	BLE_Tx('M');						//����USART2�жϷ���
	OLED_C6X8(56,40,'M');		//OLED��ʾ
	IMU_Update();						//��̬����
	for(i=0;i<3;i++)
	{		
		temp[i][0]=MPU_Value[i]/100;
		temp[i][1]=MPU_Value[i]%100/10;
		temp[i][2]=MPU_Value[i]%10;
		for(j=0;j<3;j++)
		{			
			BLE_Tx(ASCII[temp[i][j]]);
			OLED_C6X8(56,48+8*j+24*i,ASCII[temp[i][j]]);	//OLED��ʾ���͵�����
		}
	}
//	if(Key_Val!=0)	BLE_Tx('K'),BLE_Tx((Key_Val/10)+48),BLE_Tx((Key_Val%10)+48);	//�������ͼ�ֵ
}

/**
	*	@breif  ҡ��AD����ת����������NRF���ͣ����ղ��ú���ָ����е���
	*	@param  None
	*	@retval None
	*/
void LEV_NRF(void)							//ҡ������NRF����
{
	u8 i,n=100;
	u8 static temp[4],ASC[5];
//	OLED_S6X8(24,30,"LEV");OLED_S6X8(24,80,"NRF");
	NRF_Tx_Mode();
	ASC[0]='L';										//��һ��ֵ��ŷ����豸ʶ����
	OLED_C6X8(56,40,'L');
	for(i=0;i<4;i++)
	{
		temp[i]=AD_Value[i]/410;		//ת������
		ASC[i+1]=ASCII[temp[i]];		//NRF�����ַ�����ʽ���ͣ�������
		OLED_C6X8(56,48+8*i,ASCII[temp[i]]);
	}
	while(n--)
	{
		(NRF_TxPacket(ASC)!=0x20)?n=0,LED3_Set(1):LED3_Set(0);	//�ȴ��������豸ʶ���������
	}
}

/**
	*	@breif  MPU6050�������̬������NRF���ͣ����ղ��ú���ָ����е���
	*	@param  None
	*	@retval None
	*/
void MPU_NRF(void)				//MPU6050��������̬����NRF����
{
	u8 i,j,n=100;
	u8 static temp[3][3],ASC[10];
//	OLED_S6X8(24,30,"MPU");OLED_S6X8(24,80,"NRF");		//OLED��ʾ���䷽��
	NRF_Tx_Mode();
	ASC[0]='M';							//��һ��ֵ��ŷ����豸ʶ����
	OLED_C6X8(56,40,'M');		//OLED��ʾ
	IMU_Update();						//��̬����
	for(i=0;i<3;i++)
	{		
		temp[i][0]=MPU_Value[i]/100;
		temp[i][1]=MPU_Value[i]%100/10;
		temp[i][2]=MPU_Value[i]%10;
		for(j=0;j<3;j++)
		{	
			ASC[3*i+j+1]=ASCII[temp[i][j]];
			OLED_C6X8(56,48+8*j+24*i,ASCII[temp[i][j]]);	//OLED��ʾ���͵�����
		}
	}
	while(n--)
	{
		(NRF_TxPacket(ASC)!=0x20)?n=0,LED3_Set(1):LED3_Set(0);	//�ȴ��������豸ʶ���������
	}
}

/**
	*	@breif  NRF���յ�����ͨ��BLE����
	*	@param  None
	*	@retval None
	*/
void NRF_BLE(void)
{
	u8 i,temp_buf[2],buff[3];
//	OLED_S6X8(24,30,"NRF");OLED_S6X8(24,80,"BLE");
	OLED_C6X8(40,40,'N');OLED_C6X8(56,40,'B');
	NRF_RX_Mode(); 									//NRF����ģʽ
	if(NRF_RxPacket(temp_buf)==0)		//һ���յ���Ϣ����ʾ
	{
		buff[0]='N';									//���뷢���豸��ʶ��
		buff[1]=temp_buf[0];
		temp_buf[1]=0;								//�����ַ���������
		buff[2]=temp_buf[1];
		NRF_WReg(NRF_WRITE_REG + STATUS, 0xff);//����жϱ�־λ
		NRF_WReg(FLUSH_RX, 0xff);			//������ݻ��� RX FIFO
		
	}
	for(i=1;i<3;i++)
	{
		BLE_Tx(ASCII[buff[i]]);
		OLED_C6X8(40,48+8*i,ASCII[buff[i]]);
		OLED_C6X8(56,48+8*i,ASCII[buff[i]]);
	}
//	if(Key_Val!=0)	BLE_Tx('K'),BLE_Tx((Key_Val/10)+48),BLE_Tx((Key_Val%10)+48);	//�������ͼ�ֵ
}

/**
	*	@breif  BLE���յ�����ͨ����NRF��
	*	@param  None
	*	@retval None
	*/
void BLE_NRF(void)
{
	u8 i,n=100;
	u8 static temp,ASC[4]={'B','0','0','0'};	//��ʼ��
//	OLED_S6X8(24,30,"BLE");OLED_S6X8(24,80,"NRF");
	OLED_C6X8(40,40,'B');OLED_C6X8(56,40,'N');
	NRF_Tx_Mode();
//	ASC[0]='B';			//�����豸��ʶ��
	temp=USART_ReceiveData(USART2);		//NRF�����ַ�����ʽ���ͣ����������ַ���ʽ
	ASC[1]=temp/100;
	ASC[2]=temp%100/10;
	ASC[3]=temp%10;
	for(i=1;i<4;i++)	//ASC[0]Ϊ�����豸��ʶ������������
	{
		ASC[i]=ASCII[ASC[i]];
		OLED_C6X8(40,48+i*8,ASC[i]);		//��ʾ��������
		OLED_C6X8(56,48+i*8,ASC[i]);		//��ʾ��������
		printf("%d ",ASC[i]);
	}
	printf("\r\n");
	while(n--)
	{
		(NRF_TxPacket(ASC)!=0x20)?n=0,LED3_Set(1):LED3_Set(0);	//�ȴ��������豸ʶ���������
	}
}

void Show_Screen(void)
{			
	//=======================��һ��������ʾ���� REMOTE_CONTROL =========================
	if(Flag.SET_Flag==0)				OLED_S8X16(0,0," REMOTE_CONTROL ");	
	else if(Flag.SET_Flag==1)		OLED_S8X16(0,0,"      SET       ");
															OLED_S6X8(16,0,"==SET=====CH=====OK==");		
	//=======================��������ʾ���ݴ��䷽��=========================	
	OLED_C6X8(24,10,Switch_Code()+'0');			OLED_S6X8(24,56,"-->");		
	if((Flag.LEV_BLE_Flag==1 && Flag.SET_Flag==0) || bit_k==0x01)			OLED_S6X8(24,30,"LEV"),OLED_S6X8(24,80,"BLE");
	else if((Flag.MPU_BLE_Flag==1 && Flag.SET_Flag==0) || bit_k==0x02)	OLED_S6X8(24,30,"MPU"),OLED_S6X8(24,80,"BLE");
	else if((Flag.LEV_NRF_Flag==1 && Flag.SET_Flag==0) || bit_k==0x04)	OLED_S6X8(24,30,"LEV"),OLED_S6X8(24,80,"NRF");
	else if((Flag.MPU_NRF_Flag==1 && Flag.SET_Flag==0) || bit_k==0x08)	OLED_S6X8(24,30,"MPU"),OLED_S6X8(24,80,"NRF");
	else if((Flag.NRF_BLE_Flag==1 && Flag.SET_Flag==0) || bit_k==0x10)	OLED_S6X8(24,30,"NRF"),OLED_S6X8(24,80,"BLE");
	else if((Flag.BLE_NRF_Flag==1 && Flag.SET_Flag==0) || bit_k==0x20)	OLED_S6X8(24,30,"BLE"),OLED_S6X8(24,80,"NRF");
	else													OLED_S6X8(24,30,"err"),OLED_S6X8(24,80,"err");
	//=======================��������ʾ�շ�������===================================
															OLED_S8X16(32,10,"Rx:");OLED_S8X16(48,10,"Tx:");		
	//=======================�ڰ�����ʾ��ֵ===================================
}






