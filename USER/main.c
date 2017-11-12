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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);		//��ʱ��ʼ�� 
	uart_init(9600);	//���ڳ�ʼ��������Ϊ9600
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�  
  Adc_Init();
	while(DHT11_Init())	//DHT11��ʼ��	
	{
		
	}							
	while(1)
	{
		if(t%100==0)//ÿ100ms��ȡһ��
		{									  
			DHT11_Read_Data(&temperature,&humidity);		//��ȡ��ʪ��ֵ
			tem_hum[0]=	0x00;
			tem_hum[1]=	temperature;
			tem_hum[2]=	humidity;			
			send_date(tem_hum,5);
		}			
		delay_ms(1000);
		if(t%100==0)//ÿ1s��ȡһ��
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

