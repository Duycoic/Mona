#include "ds1302.h"
#include "sys.h"
#include "delay.h"
 
struct TIMEData TimeData;
struct TIMERAM TimeRAM;
u8 read_time[7];
 
void DS1302_GPIO_Init(void)//CE,SCLK端口初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);   //打开AFIO时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //关闭JTAG，PB3/PB4作为普通IO
	RCC_APB2PeriphClockCmd(DS1302_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = DS1302_SCLK_PIN; //CE
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
	GPIO_Init(DS1302_SCLK_PORT, &GPIO_InitStructure);//��ʼ��
	GPIO_ResetBits(DS1302_SCLK_PORT,DS1302_SCLK_PIN); 
	 
	GPIO_InitStructure.GPIO_Pin = DS1302_CE_PIN; //SCLK
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
	GPIO_Init(DS1302_CE_PORT, &GPIO_InitStructure);//��ʼ��
	GPIO_ResetBits(DS1302_CE_PORT,DS1302_CE_PIN); 
}
 
void DS1302_DATAOUT_init()//����˫��I/O�˿�Ϊ���̬
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DS1302_CLK, ENABLE);
	 
	GPIO_InitStructure.GPIO_Pin = DS1302_DATA_PIN; // DATA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(DS1302_DATA_PORT, &GPIO_InitStructure);//��ʼ��
	GPIO_ResetBits(DS1302_DATA_PORT,DS1302_DATA_PIN);
}

void DS1302_DATAINPUT_init()//����˫��I/O�˿�Ϊ����̬
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DS1302_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = DS1302_DATA_PIN; //DATA
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(DS1302_DATA_PORT, &GPIO_InitStructure);//��ʼ��
}
 
 
void DS1302_write_onebyte(u8 data)//��DS1302����һ�ֽ�����
{
		u8 count=0;
		SCLK_L;
		DS1302_DATAOUT_init();


	for(count=0;count<8;count++)
		{	SCLK_L;
			if(data&0x01)
			{DATA_H;}
			else{DATA_L;}//��׼���������ٷ���
			SCLK_H;//����ʱ���ߣ���������
			data>>=1;
		}
}
 
void DS1302_wirte_rig(u8 address,u8 data)//��ָ���Ĵ�����ַ��������
{
	u8 temp1=address;
	u8 temp2=data;
	CE_L;SCLK_L;Delay_us(1);
	CE_H;Delay_us(3);
	DS1302_write_onebyte(temp1);
	DS1302_write_onebyte(temp2);
	CE_L;SCLK_L;Delay_us(3);
}
 
u8 DS1302_read_rig(u8 address)//��ָ����ַ��ȡһ�ֽ�����
{
	u8 temp3=address;
	u8 count=0;
	u8 return_data=0x00;
	CE_L;SCLK_L;Delay_us(3);
	CE_H;Delay_us(3);
	DS1302_write_onebyte(temp3);
	DS1302_DATAINPUT_init();//����I/O��Ϊ����
	Delay_us(3);
	for(count=0;count<8;count++)
		{
		Delay_us(3);//ʹ��ƽ����һ��ʱ��
		return_data>>=1;
		SCLK_H;Delay_us(5);//ʹ�ߵ�ƽ����һ��ʱ��
		SCLK_L;Delay_us(30);//��ʱ14us����ȥ��ȡ��ѹ������׼ȷ
		if(GPIO_ReadInputDataBit(DS1302_DATA_PORT,DS1302_DATA_PIN))
		{return_data=return_data|0x80;}
		
		}
	Delay_us(2);
	CE_L;DATA_L;
	return return_data;
}
 
void DS1302_Init(void)
{
	DS1302_wirte_rig(0x8e,0x00);//�ر�д����
	DS1302_wirte_rig(0x80,0x00);//seconds00��
	DS1302_wirte_rig(0x82,0x30);//minutes30��
	DS1302_wirte_rig(0x84,0x20);//hours20ʱ
	DS1302_wirte_rig(0x86,0x03);//date3��
	DS1302_wirte_rig(0x88,0x10);//months10��
	DS1302_wirte_rig(0x8a,0x04);//days������
	DS1302_wirte_rig(0x8c,0x24);//year2024��
	DS1302_wirte_rig(0x8e,0x80);//����д����
}
 
void DS1302_read_time(void)
{
	read_time[0]=DS1302_read_rig(0x81);//����
	read_time[1]=DS1302_read_rig(0x83);//����
	read_time[2]=DS1302_read_rig(0x85);//��ʱ
	read_time[3]=DS1302_read_rig(0x87);//����
	read_time[4]=DS1302_read_rig(0x89);//����
	read_time[5]=DS1302_read_rig(0x8B);//������
	read_time[6]=DS1302_read_rig(0x8D);//����
}
 
void DS1302_read_realTime(void)
{
	DS1302_read_time();  //BCD��ת��Ϊ10����
	TimeData.second=(read_time[0]>>4)*10+(read_time[0]&0x0f);
	TimeData.minute=((read_time[1]>>4))*10+(read_time[1]&0x0f);
	TimeData.hour=(read_time[2]>>4)*10+(read_time[2]&0x0f);
	TimeData.day=(read_time[3]>>4)*10+(read_time[3]&0x0f);
	TimeData.month=(read_time[4]>>4)*10+(read_time[4]&0x0f);
	TimeData.week=read_time[5];
	TimeData.year=(read_time[6]>>4)*10+(read_time[6]&0x0f)+2000;	
}


void DS1302_wirteRAM(void)
{
	DS1302_wirte_rig(0x8e,0x00);//�ر�д����
	DS1302_wirte_rig(0xC0,TimeRAM.hour_kai);//��ʱ
	DS1302_wirte_rig(0xC2,TimeRAM.minute_kai);//����
	DS1302_wirte_rig(0xC4,TimeRAM.hour_guan);//��ʱ
	DS1302_wirte_rig(0xC6,TimeRAM.minute_guan);//�ط�
	DS1302_wirte_rig(0xC8,TimeRAM.kai);//�ط�
	DS1302_wirte_rig(0xCA,TimeRAM.guan);//�ط�
	DS1302_wirte_rig(0x8e,0x80);//�ر�д����
}
void DS1302_readRAM(void)
{
	TimeRAM.hour_kai=DS1302_read_rig(0xC1);//����
	TimeRAM.minute_kai=DS1302_read_rig(0xC3);//����
	TimeRAM.hour_guan=DS1302_read_rig(0xC5);//��ʱ
	TimeRAM.minute_guan=DS1302_read_rig(0xC7);//����	
	TimeRAM.kai=DS1302_read_rig(0xC9);//����
	TimeRAM.guan=DS1302_read_rig(0xCB);//����
}
 




