#include "myiic.h"
#include "stm32f10x.h"
#include "sys.h"
#include "codetab.h"

//��ʼ��Ӳ��IIC����
void I2C_Configuration(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		I2C_InitTypeDef I2C_InitStructure;
		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		
		//PB6����SCL PB7����SDA
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		I2C_DeInit(I2C1);
		I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		I2C_InitStructure.I2C_ClockSpeed          = 400000;
		I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
		I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
		I2C_InitStructure.I2C_OwnAddress1         = 0x30;
		
		I2C_Init(I2C1, &I2C_InitStructure);
		I2C_Cmd(I2C1, ENABLE);
}

//��OLED�Ĵ�����ַдһ��byte������
void I2C_WriteByte(uint8_t addr,uint8_t data)
{
		while( I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) );
		I2C_GenerateSTART(I2C1, ENABLE);
		
		while( !I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) );
		I2C_Send7bitAddress(I2C1, OLED_ADDRESS, I2C_Direction_Transmitter);
		
		while( !I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) );
		I2C_SendData(I2C1, addr);
		
		while( !I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) );
		I2C_SendData(I2C1, data);
		
		while( !I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) );
		I2C_GenerateSTOP(I2C1, ENABLE);
}

//дָ��
void WriteCmd(unsigned char I2C_Command)
{
		I2C_WriteByte(0x00,I2C_Command);
}

//д����
void WriteData(unsigned char I2C_Data)
{
		I2C_WriteByte(0x40,I2C_Data);
}

//���ҳ�ʼ������
void OLED_Init(void)
{
		delay_ms(100);
		WriteCmd(0xAE); //display off
		WriteCmd(0x20);	//Set Memory Addressing Mode	
		WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
		WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
		WriteCmd(0xc8);	//Set COM Output Scan Direction
		WriteCmd(0x00); //---set low column address
		WriteCmd(0x10); //---set high column address
		WriteCmd(0x40); //--set start line address
		WriteCmd(0x81); //--set contrast control register
		WriteCmd(0xff); //����?���̡�?�� 0x00~0xff
		WriteCmd(0xa1); //--set segment re-map 0 to 127
		WriteCmd(0xa6); //--set normal display
		WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
		WriteCmd(0x3F); //
		WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
		WriteCmd(0xd3); //-set display offset
		WriteCmd(0x00); //-not offset
		WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
		WriteCmd(0xf0); //--set divide ratio
		WriteCmd(0xd9); //--set pre-charge period
		WriteCmd(0x22); //
		WriteCmd(0xda); //--set com pins hardware configuration
		WriteCmd(0x12);
		WriteCmd(0xdb); //--set vcomh
		WriteCmd(0x20); //0x20,0.77xVcc
		WriteCmd(0x8d); //--set DC-DC enable
		WriteCmd(0x14); //
		WriteCmd(0xaf); //--turn on oled panel

}

//���ù����ʼ���꣨x,y��
void OLED_SetPos(unsigned char x,unsigned char y)
{
		WriteCmd(0xb0+y);
		WriteCmd( (x & 0xf0) >> 4 | 0x10 );
		WriteCmd( (x & 0x0f) | 0x01 );
}

//���������Ļ
void OLED_Fill(unsigned char Fill_Data)
{
		unsigned char m,n;
		
		for(m=0;m<8;m++)
		{
				WriteCmd(0xb0+m);
				WriteCmd(0x00);
				WriteCmd(0x10);
				
				for(n=0;n<128;n++)
				{
						WriteData(Fill_Data);
				}
		}
}

//����
void OLED_CLS(void)
{
		OLED_Fill(0x00);
}

//��OLED�������л���
void OLED_ON(void)
{
		WriteCmd(0xAF);
		WriteCmd(0x8D);
		WriteCmd(0x14);
}

//��OLED���� -- ����ģʽ��,OLED���Ĳ���10uA
void OLED_OFF(void)
{
		WriteCmd(0xAE);
		WriteCmd(0x8D);
		WriteCmd(0x10);
}

void OLED_ShowStr(unsigned char x,unsigned char y,unsigned char ch[],unsigned char TextSize)
{
		unsigned char c = 0,i = 0,j = 0;
		
		switch(TextSize)
		{
			case 1:
			{
					while(ch[j] != '\0')
					{
							c = ch[j] - 32;
							if(x>126)
							{
									x = 0;
									y++;
							}
							
							OLED_SetPos(x,y);
							
							for(i=0;i<6;i++)
							{
									WriteData(F6x8[c][i]);
							}
							x+=6;
							j++;
					}
			}
			break;
			
			case 2:
			{
					while(ch[j] != '\0')
					{
							c = ch[j] - 32;
							
							if(x>120)
							{
									x = 0;
									y++;
							}
							
							OLED_SetPos(x,y);
							
							for(i=0;i<8;i++)
							{
									WriteData(F8X16[c*16+i]);	
							}
							
							OLED_SetPos(x,y+1);
							
							for(i=0;i<8;i++)
							{
									WriteData(F8X16[c*16+i+8]);
							}
							x+=8;
							j++;		
					}
			}
			break;
		}
}

void OLED_ShowCN(unsigned char x,unsigned char y,unsigned char N)
{
		unsigned char i = 0;
		unsigned char addr = 32*N;
		
		OLED_SetPos(x,y);
		
		for(i=0;i<16;i++)
		{
				WriteData(F16X16[addr]);
				addr += 1;
		}
		
		OLED_SetPos(x,y+1);
		
		for(i=0;i<16;i++)
		{
				WriteData(F16X16[addr]);
				addr += 1;
		}
}

void OLED_ShowBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
		unsigned char x,y;
		unsigned int j = 0;
		
		if(y1 % 8 == 0)
		{
				y = y1 / 8;
		}
		else
		{
				y = y1 / 8+1;
		}
		
		for(y=y0;y<y1;y++)
		{
				OLED_SetPos(x0,y);

				for(x=x0;x<x1;x++)
				{
						WriteData(BMP1[j++]);
				}
		}
}

