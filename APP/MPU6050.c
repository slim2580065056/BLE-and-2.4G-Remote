/**
  ******************************************************************************
  * @file    MPU6050.c
  * @author  LiuSongHu_QQ2580065056
  * @version V1.0.0
  * @date    2016-12-25
  * @brief   This file provides MPU6050 operation gyro and accel firmware functions. 
  ******************************************************************************
  * @attention
	*	1.�������ٶȹ�һ��
	*	2.��ȡ��Ԫ�صĵ�Ч���Ҿ����е���������
	*	3.��������ó���̬���
	*	4.�������л���
	*	5.�����˲�����̬���������ٶ��ϣ��������ٶȵĻ���Ư��
	*	6.һ�����������������Ԫ��
	*	7.��Ԫ�ع�һ��
	*	8.��Ԫ��תŷ����
  */
	
#include "MPU6050.h"

typedef struct IMU_DATA
{
	float q0,q1,q2,q3;									//��Ԫ��	
	float ax,ay,az,gx,gy,gz;						//�����Ǻͼ��ٶȼƵ�ģ��������Ӧ��������
	float ex,ey,ez;											//�˲��ں�֮���������
	float PITCH;				//������
	float YAW;					//ƫ����
	float ROLL;					//�����

}IMU_param;
IMU_param IMU={1.0,0.0,0.0,0.0};	//��ʼ����Ԫ��;

u16 MPU_Value[3];

//extern float PITCH;				//������
//extern float YAW;					//ƫ����
//extern float ROLL;					//�����

float exInt,eyInt,ezInt;

/***********************************************************************************************
*		������MPU6050оƬ��λ��ʼ������
*		���룺��
*		�����(���ڶ�ȡ)��ʼ���ɹ� OR ��ʼ��ʧ��
*		˵������ʼ����Դѡ���ʼ�������ʣ���ʼ�����̵�
************************************************************************************************/
void MPU6050_Init(void)
{
	I2C_Send_Data(PWR_MGMT_1,0x80);						//��λ��Դ��������λ���мĴ���
	Delay_ms(500);
	I2C_Send_Data(PWR_MGMT_1,0x00);						//ֹͣ����
	I2C_Send_Data(SMPRT_DIV,0x07);						//�����ǲ����ʣ�����ֵ0x07 -> 125HZ
	I2C_Send_Data(CONFIG,0x06);								//��ͨ�˲�����,��ֹƵ��1K,����5K,��ʱ19Ms
	I2C_Send_Data(SIGNAL_PATH_RESET,0x07);		//��λ���ٶȼƣ���λ�����ǣ���λ�¶ȴ�����
	I2C_Send_Data(GYRO_CONFIG,0x18);					//�����ǲ��Լ죬��Χ2000deg/s��(GYRO_CONFIG,0x10--1000deg/s)
	I2C_Send_Data(ACCEL_CONFIG,0x01);					//���ٶȲ��Լ죬��Χ+-4g��(00--+-2g)
//	printf("WHO_AM_I 0x%X  PWR_DIV 0x%X\r\n",I2C_Read_Data(WHO_AM_I),I2C_Read_Data(PWR_MGMT_1));//��ӡ������ַ�͸�λ�Ĵ���ֵ
	if(I2C_Read_Data(WHO_AM_I)==0x68)					//����������ַ��ȷ�����ʼ���ɹ�
		printf("MPU6050 Initialized Complete \r\n");
	else
	{
		printf("MPU6050 Initialized Error \r\n");
		printf("driver id is %#x\r\n",I2C_Read_Data(WHO_AM_I));
	}
}
void test_read(void)
{
	IMU_Update();
//	printf("ax=%f  ay=%f  az=%f    ",IMU.ax,IMU.ay,IMU.az);
//	printf("gx=%f  gy=%f  gz=%f    \r\n",IMU.gx,IMU.gy,IMU.gz);
	printf("pitch->%.3f    roll->%.3f    yaw->%.3f \r\n",IMU.PITCH,IMU.ROLL,IMU.YAW);
}
/***********************************************************************************************
*		������MPU6050����ȡ��������ת��Ϊ���õ�ģ��ֵ
*		���룺��
*		���������ļ��ٶ�ģ��ֵ��������ģ��ֵ(��Ϊȫ�ֱ����������Ƿ���ֵ)
*		˵������ƫ�����ںϵ������������������Ļ�е���
************************************************************************************************/
void Data2Value(void)
{
		/*	���ٶȼ�����+-4g��������ֶ�ֵ���������ȣ�2^13=8192LSB/g		*/
	IMU.gx=(float)I2C_Read_16(GYRO_XOUT_H)/8192;
	IMU.gy=(float)I2C_Read_16(GYRO_YOUT_H)/8192;
	IMU.gz=(float)I2C_Read_16(GYRO_ZOUT_H)/8192;
	/*	������ٶȺ������ǽǶȣ�ע�ⵥλ	*/
	/*	����������+-2000deg/s��������ֶ�ֵ���������ȣ�=2^14=16.384LSB��/s		*/
	IMU.ax=(float)I2C_Read_16(ACCEL_XOUT_H)/16.384;
	IMU.ay=(float)I2C_Read_16(ACCEL_YOUT_H)/16.384;
	IMU.az=(float)I2C_Read_16(ACCEL_ZOUT_H)/16.384;
}

float InvSqrt(float x)
{
float xhalf = 0.5f*x;
int i = *(int*)&x;
i = 0x5f3759df - (i >> 1); // ????????
x = *(float*)&i;
x = x*(1.5f - xhalf*x*x); // ?????
return x;
}
/***********************************************************************************************
*		������MPU6050��̬�����ںϽ���
*		���룺����������ֵ��������ٶ�ֵ��ֱ�ӽ��л�ȡȫ�ֱ���
*		�����ROLL(�����)��PITCH(������)��YAW(ƫ����)
*		˵����
************************************************************************************************/
void IMU_Update(void)
{
	float vx,vy,vz,ex,ey,ez;
	float norm;
	
	Data2Value();
	
	//��һ�����ݣ����㴦��
	norm=sqrt(IMU.ax*IMU.ax+IMU.ay*IMU.ay+IMU.az*IMU.az);		//sqrt(x^2+y^2+z^2)
	IMU.ax=IMU.ax/norm;
	IMU.ay=IMU.ay/norm;
	IMU.az=IMU.az/norm;
	
	vx=2*(IMU.q1*IMU.q3-IMU.q0*IMU.q2);
	vy=2*(IMU.q2*IMU.q3+IMU.q0*IMU.q1);
	vz=(IMU.q0*IMU.q0-IMU.q1*IMU.q1-IMU.q2*IMU.q2+IMU.q3*IMU.q3);
	
	ex=(IMU.ay*vz-IMU.az*vy);
	ey=(IMU.az*vx-IMU.ax*vz);
	ez=(IMU.ax*vy-IMU.ay*vx);
	
	exInt+=ex*Ki;
	eyInt+=ey*Ki;
	ezInt+=ez*Ki;
	
	IMU.gx+=Kp*ex+exInt;
	IMU.gy+=Kp*ey+eyInt;
	IMU.gz+=Kp*ez+ezInt;
	
	IMU.q0=IMU.q0+(-IMU.q1*IMU.gx-IMU.q2*IMU.gy-IMU.q3*IMU.gz)*halfT;
	IMU.q1=IMU.q1+(IMU.q0*IMU.gx+IMU.q2*IMU.gz-IMU.q3*IMU.gy)*halfT;
	IMU.q2=IMU.q2+(IMU.q0*IMU.gy-IMU.q1*IMU.gz+IMU.q3*IMU.gx)*halfT;
	IMU.q3=IMU.q3+(IMU.q0*IMU.gz+IMU.q1*IMU.gy-IMU.q2*IMU.gx)*halfT;
	
	//��һ����Ԫ�����ݣ����㴦��
	norm=sqrt(IMU.q0*IMU.q0+IMU.q1*IMU.q1+IMU.q2*IMU.q2+IMU.q3*IMU.q3);
	IMU.q0=IMU.q0/norm;
	IMU.q1=IMU.q1/norm;
	IMU.q2=IMU.q2/norm;
	IMU.q3=IMU.q3/norm;

	/*	calculation  angle	,remark 180/Pi=57.293	*/
	IMU.YAW=atan2(2*IMU.q1*IMU.q2+IMU.q0*IMU.q3,-2*IMU.q2*IMU.q2-2*IMU.q3*IMU.q3+1)*57.297;
	IMU.PITCH=asin(-2*IMU.q1*IMU.q3+2*IMU.q0*IMU.q2)*57.297;
	IMU.ROLL=atan2(2*IMU.q2*IMU.q3+2*IMU.q0*IMU.q1,-2*IMU.q1*IMU.q1-2*IMU.q2*IMU.q2+1)*57.297;
	
	MPU_Value[0]=(u16)IMU.PITCH;	MPU_Value[1]=(u16)IMU.ROLL;	MPU_Value[2]=(u16)IMU.YAW;	
}
/***********************************************************************************************
*		������MPU6050��̬���͵�����������λ��
*		˵������������������λ����Э��
*			��λ�������Զ������ݣ���ʽΪ��0x88+FUN+LEN+DATA+SUM
*	SUM���ڴӸ�����֡��һ�ֽڿ�ʼ��Ҳ����֡ͷ(0x88)��ʼ������֡���ݵ����һ�ֽ������ֽڵĺͣ�ֻ�����Ͱ�λ(u8).
*	Э���г����ֽ�LEN��ʾ������֡�ڰ������ݵ��ֽ��ܳ��ȣ�������֡ͷ�������֡������ֽں�����У��λ��ֻ�����ݵ��ֽڳ��Ⱥ͡�
*			�����֡��������Ϊ3��int16�����ݣ���ôLEN����6
*	֡��ʽ��0x88+0xAF+0x1C+ACC DATA+GYRO DATA+MAG DATA+ANGLE DATA+ 0x00 0x00 + 0x00 0x00+SUM����32�ֽ�
*	ACC/GYRO/MAG/ANGLE(roll/pitch/yaw)����Ϊint16��ʽ
*	����ANGLE��roll��pitch����Ϊʵ��ֵ����100�Ժ�õ�������ֵ��yawΪ����10�Ժ�õ�������ֵ����λ������ʾʱ�ٳ���100��10.
************************************************************************************************/
void ANO_IMU(void)
{
	u8 i,arr[32],sum=0;	
//	IMU_Update();		//��̬���¿�
	IMU.ROLL*=100;
	IMU.PITCH*=100;
	IMU.YAW*=10;
	
	arr[0]=0X88;					//֡ͷ0x88
	arr[1]=0XAF;					//FUN
	arr[2]=0X1C;					//LEN
	arr[3]=((short)IMU.ax>>8)&0xFF;		//AX
	arr[4]=((short)IMU.ax)&0xFF;
	arr[5]=((short)IMU.ay>>8)&0xFF;		//AY
	arr[6]=((short)IMU.ay)&0xFF;
	arr[7]=((short)IMU.az>>8)&0xFF;		//AZ
	arr[8]=((short)IMU.az)&0xFF;
	arr[9]=((short)IMU.gx>>8)&0xFF;		//GX
	arr[10]=((short)IMU.gx)&0xFF;
	arr[11]=((short)IMU.gy>>8)&0xFF;	//GY
	arr[12]=((short)IMU.gy)&0xFF;
	arr[13]=((short)IMU.gz>>8)&0xFF;	//GZ
	arr[14]=((short)IMU.gz)&0xFF;
	arr[15]=0;												//MX	
	arr[16]=0;
	arr[17]=0;												//MY	
	arr[18]=0;
	arr[19]=0;												//MZ	
	arr[20]=0;
	arr[21]=((short)IMU.ROLL)>>8&0xFF;		//ROLL
	arr[22]=((short)IMU.ROLL)&0xFF;
	arr[23]=((short)IMU.PITCH)>>8&0xFF;	//PITCH
	arr[24]=((short)IMU.PITCH)&0xFF;
	arr[25]=((short)IMU.YAW)>>8&0xFF;			//YAW
	arr[26]=((short)IMU.YAW)&0xFF;
	arr[27]=arr[28]=arr[29]=arr[30]=0;	//0x0000 0000;
	
	for(i=0;i<31;i++)	sum+=arr[i];		//��У��
	arr[31]=sum;
	for(i=0;i<32;i++)									//��Э�鷢������
	{	
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART1,arr[i]);  
	}
}










