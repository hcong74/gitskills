/**********************51实验开发板例程************************

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

uint g_endcount; //终止延时计数
bit g_flag; //红外发送标记
uchar g_iraddr1 = 0x10; //十六位地址的第一个字节
uchar g_iraddr2 = 0x6f; //十六位地址的第一个字节

uchar keyscan();
void delay_ms(uint ms);
void delay500ms(void);

//定时器0中断处理/////////////////
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
		//先发送0.56ms的38KHZ红外波
		SendIRdata_38KHZ(70,1); //8*70=0.56ms 

		//停止发送红外信号（即编码中的低电平）
		if(irdata & 1)  //判断最低位为1还是0. 低位先发送
			SendIRdata_38KHZ(210,0);  //1为宽电平，8*210=1.68ms
		else
			SendIRdata_38KHZ(70,0);  //0为窄电平，8*70=0.56ms
		irdata = irdata >> 1;
	}
}

/////////////////////////////////
void SendIRdata(unsigned char p_irdata)
{
	//有的遥控器会发一个前脉冲，如果不灵，可以试试加上前脉冲
	//发送起始码前脉冲，高电平有38KHZ载波
//	SendIRdata_38KHZ(18, 1);
	//发送起始码前脉冲，低电平无38KHZ载波
//	SendIRdata_38KHZ(18, 0);

	//发送9ms的起始码，高电平有38KHZ载波
	SendIRdata_38KHZ(1125,1); //8*1125=9ms

	//发送4.5ms的起始码，低电平无38KHZ载波
	SendIRdata_38KHZ(562,0); //8*562=4.496ms

	//发送十六位地址的前八位
	SendIRdata_BYTE(g_iraddr1);

	//发送十六位地址的后八位
	SendIRdata_BYTE(g_iraddr2);

	//发送八位数据
	SendIRdata_BYTE(p_irdata);

	//发送八位数据反码
	SendIRdata_BYTE(~p_irdata);

	//发送总的结束位1bit
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
	HW = 0;   //LED接电源正极，不点亮
	TMOD = 0x02; //定时器0：方式2，自动重装，8位
	TH0 = 243;
	TL0 = 243;	 //即计数12次中断一次，12MHZ晶振机器周期1us，13*1=13us，这样达到38KHZ
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


  