           

#include "delay.h"
#include "sys.h"
#include "myiic.h"

 int main(void)
  {	u8 t;
		delay_init();	    	 //延时函数初始化	  
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
		
		I2C_Configuration();
		OLED_Init();
//		delay_ms(2000);
//		
//		OLED_Fill(0xFF);
//		delay_ms(2000);
//		
//		OLED_Fill(0x00);
//		delay_ms(2000);
		while(1)
		{
			OLED_ShowStr(31,2,"Hello World",1);
			OLED_ShowStr(21,5,"Hello World",2);
		}
}
	