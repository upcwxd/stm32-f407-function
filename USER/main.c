#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "dht11.h"
#include "adc.h"


int main(void)
{ 
 
	u8 t;
	u8 temperature;  	    
	u8 humidity;  
	u8 tem_hum[5];
	u16 adcx;
	float temp;
	u8 zs,xs;
//	u8 len;	
	u16 times=0;  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);		//延时初始化 
	uart_init(9600);	//串口初始化波特率为9600
	LED_Init();		  		//初始化与LED连接的硬件接口  
  Adc_Init();
	while(DHT11_Init())	//DHT11初始化	
	{
		
	}							
	while(1)
	{
		if(t%100==0)//每100ms读取一次
		{									  
			DHT11_Read_Data(&temperature,&humidity);		//读取温湿度值
			tem_hum[0]=	0x00;
			tem_hum[1]=	temperature;
			tem_hum[2]=	humidity;			
			send_date(tem_hum,5);
		}			
		delay_ms(1000);
		if(t%100==0)//每1s读取一次
		{		
			adcx=Get_Adc_Average(ADC_Channel_5,20);
			temp=(float)adcx*(3.3/4096);
			zs=temp;
			xs=(temp-zs)*1000;			
			tem_hum[0]=	0x01;	
			tem_hum[3]=	zs;
			tem_hum[4]=	xs;			
			send_date(tem_hum,5);
		}	
	}
}

