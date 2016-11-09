/**********************51ʵ�鿪��������************************

**************************************************************/
#include<reg52.h>    
#include<intrins.h>

#define uchar unsigned char
#define uint unsigned int

sbit K2 = P1^0;
sbit K3 = P1^1;
sbit K4 = P1^2;
sbit K5 = P1^3;
sbit SM = P2^0;
sbit HW = P2^7;

uint g_endcount; //��ֹ��ʱ����
bit g_flag; //���ⷢ�ͱ��
uchar g_iraddr1 = 0x10; //ʮ��λ��ַ�ĵ�һ���ֽ�
uchar g_iraddr2 = 0x6f; //ʮ��λ��ַ�ĵ�һ���ֽ�

uchar keyscan();
void delay_ms(uint ms);
void delay500ms(void);

//��ʱ��0�жϴ���/////////////////
void timeint(void) interrupt 1
{
	HW = !HW;
}

/////////////////////////////////
void SendIRdata_38KHZ(unsigned int temp1, bit temp2)
{
	uint i;
	g_endcount = temp1;
	g_flag = temp2;
	if(g_flag == 1)
	{
		ET0 = 1;
		TR0 = 1;
		for(i = g_endcount; i > 0; i--)
		{;}
		ET0=0; TR0=0;
	}
	else
	{
		ET0 = 0;
		TR0 = 0;
		for(i = g_endcount; i > 0; i--)
		{;}
	}
	HW = 0;
}

/////////////////////////////////
void SendIRdata_BYTE(unsigned char irdata)
{
	unsigned char i;
	for(i = 0; i < 8; i++)
	{
		//�ȷ���0.56ms��38KHZ���Ⲩ
		SendIRdata_38KHZ(70,1); //8*70=0.56ms 

		//ֹͣ���ͺ����źţ��������еĵ͵�ƽ��
		if(irdata & 1)  //�ж����λΪ1����0. ��λ�ȷ���
			SendIRdata_38KHZ(210,0);  //1Ϊ���ƽ��8*210=1.68ms
		else
			SendIRdata_38KHZ(70,0);  //0Ϊխ��ƽ��8*70=0.56ms
		irdata = irdata >> 1;
	}
}

/////////////////////////////////
void SendIRdata(unsigned char p_irdata)
{
	//�е�ң�����ᷢһ��ǰ���壬������飬�������Լ���ǰ����
	//������ʼ��ǰ���壬�ߵ�ƽ��38KHZ�ز�
//	SendIRdata_38KHZ(18, 1);
	//������ʼ��ǰ���壬�͵�ƽ��38KHZ�ز�
//	SendIRdata_38KHZ(18, 0);

	//����9ms����ʼ�룬�ߵ�ƽ��38KHZ�ز�
	SendIRdata_38KHZ(1125,1); //8*1125=9ms

	//����4.5ms����ʼ�룬�͵�ƽ��38KHZ�ز�
	SendIRdata_38KHZ(562,0); //8*562=4.496ms

	//����ʮ��λ��ַ��ǰ��λ
	SendIRdata_BYTE(g_iraddr1);

	//����ʮ��λ��ַ�ĺ��λ
	SendIRdata_BYTE(g_iraddr2);

	//���Ͱ�λ����
	SendIRdata_BYTE(p_irdata);

	//���Ͱ�λ���ݷ���
	SendIRdata_BYTE(~p_irdata);

	//�����ܵĽ���λ1bit
	SendIRdata_38KHZ(70,1); //8*70=0.56ms
}
  
/////////////////////////////////
void main(void)
{
	unsigned char key = 0;
	unsigned char temp[16] =
	{
		0xd7, 0x14, 0xcd, 0x5d, 0x1e, 0x5b, 0xdb, 0x15,
		0xdf, 0x5f, 0x9f, 0xda, 0xc8, 0xdc, 0xcb, 0x8b
	};

	g_flag = 0;
	HW = 0;   //LED�ӵ�Դ������������
	TMOD = 0x02; //��ʱ��0����ʽ2���Զ���װ��8λ
	TH0 = 243;
	TL0 = 243;	 //������12���ж�һ�Σ�12MHZ�����������1us��13*1=13us�������ﵽ38KHZ
	EA  = 1;
	
	P2 = 0x01;
	P0 = temp[key];
	while(1)
	{	
		key = keyscan();
		if(key == 2)
		{
			P0 = temp[key];
			SendIRdata(0xc2);
		}
		else if(key == 3)
		{
			P0 = temp[key];
			SendIRdata(0x7a);
		}
		else if(key == 4)
		{
			P0 = temp[key];
			SendIRdata(0x3d);
		}
		else if(key == 5)
		{
			P0 = temp[key];
			SendIRdata(0x85);
		}
	}
}

void delay_ms(uint ms)
{
	unsigned int j,i;

	for(i = ms; i > 0; i--)
		for(j = 123; j > 0; j--);
}
void delay500ms(void)
{
	unsigned char i,j,k;
	for(i=15;i>0;i--)
		for(j=202;j>0;j--)
			for(k=81;k>0;k--);
}

uchar keyscan()
{
	if(K2 == 0)
	{
		delay_ms(10);
		if(K2 == 0)
		{
			while(K2 == 0) ;
			return 2;
		}
	}
	if(K3 == 0)
	{
		delay_ms(10);
		if(K3 == 0);
		{
			while(K3 == 0);
			return 3;
		}
	}
	if(K4 == 0)
	{
		delay_ms(10);
		if(K4 == 0);
		{
			while(K4 == 0);
			return 4;
		}
	}
	if(K5 == 0)
	{
		delay_ms(10);
		if(K5 == 0);
		{
			while(K5 == 0);
			return 5;
		}
	}
	return 0;
}


  