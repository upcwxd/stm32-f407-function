#include "sys.h"
#include "usart.h"	
////////////////////////////////////////////////////////////////////////////////// 	 
//Èç¹ûÊ¹ÓÃucos,Ôò°üÀ¨ÏÂÃæµÄÍ·ÎÄ¼ş¼´¿É.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos Ê¹ÓÃ	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌĞòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßĞí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEK STM32F4Ì½Ë÷Õß¿ª·¢°å
//´®¿Ú1³õÊ¼»¯		   
//ÕıµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//ĞŞ¸ÄÈÕÆÚ:2014/6/10
//°æ±¾£ºV1.5
//°æÈ¨ËùÓĞ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3ĞŞ¸ÄËµÃ÷ 
//Ö§³ÖÊÊÓ¦²»Í¬ÆµÂÊÏÂµÄ´®¿Ú²¨ÌØÂÊÉèÖÃ.
//¼ÓÈëÁË¶ÔprintfµÄÖ§³Ö
//Ôö¼ÓÁË´®¿Ú½ÓÊÕÃüÁî¹¦ÄÜ.
//ĞŞÕıÁËprintfµÚÒ»¸ö×Ö·û¶ªÊ§µÄbug
//V1.4ĞŞ¸ÄËµÃ÷
//1,ĞŞ¸Ä´®¿Ú³õÊ¼»¯IOµÄbug
//2,ĞŞ¸ÄÁËUSART_RX_STA,Ê¹µÃ´®¿Ú×î´ó½ÓÊÕ×Ö½ÚÊıÎª2µÄ14´Î·½
//3,Ôö¼ÓÁËUSART_REC_LEN,ÓÃÓÚ¶¨Òå´®¿Ú×î´óÔÊĞí½ÓÊÕµÄ×Ö½ÚÊı(²»´óÓÚ2µÄ14´Î·½)
//4,ĞŞ¸ÄÁËEN_USART1_RXµÄÊ¹ÄÜ·½Ê½
//V1.5ĞŞ¸ÄËµÃ÷
//1,Ôö¼ÓÁË¶ÔUCOSIIµÄÖ§³Ö
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//¼ÓÈëÒÔÏÂ´úÂë,Ö§³Öprintfº¯Êı,¶ø²»ĞèÒªÑ¡Ôñuse MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//±ê×¼¿âĞèÒªµÄÖ§³Öº¯Êı                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//¶¨Òå_sys_exit()ÒÔ±ÜÃâÊ¹ÓÃ°ëÖ÷»úÄ£Ê½    
_sys_exit(int x) 
{ 
	x = x; 
} 
//ÖØ¶¨Òåfputcº¯Êı 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//Ñ­»··¢ËÍ,Ö±µ½·¢ËÍÍê±Ï   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
#if EN_USART1_RX   //Èç¹ûÊ¹ÄÜÁË½ÓÊÕ
//´®¿Ú1ÖĞ¶Ï·şÎñ³ÌĞò
//×¢Òâ,¶ÁÈ¡USARTx->SRÄÜ±ÜÃâÄªÃûÆäÃîµÄ´íÎó   	
u8 USART_RX_BUF[USART_REC_LEN];     //½ÓÊÕ»º³å,×î´óUSART_REC_LEN¸ö×Ö½Ú.
//½ÓÊÕ×´Ì¬
//bit15£¬	½ÓÊÕÍê³É±êÖ¾
//bit14£¬	½ÓÊÕµ½0x0d
//bit13~0£¬	½ÓÊÕµ½µÄÓĞĞ§×Ö½ÚÊıÄ¿
u16 USART_RX_STA=0;       //½ÓÊÕ×´Ì¬±ê¼Ç	

//³õÊ¼»¯IO ´®¿Ú1 
//bound:²¨ÌØÂÊ
void uart_init(u32 bound){
   //GPIO¶Ë¿ÚÉèÖÃ
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //Ê¹ÄÜGPIOAÊ±ÖÓ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//Ê¹ÄÜUSART1Ê±ÖÓ
 
	//´®¿Ú1¶ÔÓ¦Òı½Å¸´ÓÃÓ³Éä
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9¸´ÓÃÎªUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10¸´ÓÃÎªUSART1
	
	//USART1¶Ë¿ÚÅäÖÃ
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9ÓëGPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//¸´ÓÃ¹¦ÄÜ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//ËÙ¶È50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //ÍÆÍì¸´ÓÃÊä³ö
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //ÉÏÀ­
	GPIO_Init(GPIOA,&GPIO_InitStructure); //³õÊ¼»¯PA9£¬PA10

   //USART1 ³õÊ¼»¯ÉèÖÃ
	USART_InitStructure.USART_BaudRate = bound;//²¨ÌØÂÊÉèÖÃ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//×Ö³¤Îª8Î»Êı¾İ¸ñÊ½
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//Ò»¸öÍ£Ö¹Î»
	USART_InitStructure.USART_Parity = USART_Parity_No;//ÎŞÆæÅ¼Ğ£ÑéÎ»
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ÎŞÓ²¼şÊı¾İÁ÷¿ØÖÆ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//ÊÕ·¢Ä£Ê½
  USART_Init(USART1, &USART_InitStructure); //³õÊ¼»¯´®¿Ú1
	
  USART_Cmd(USART1, ENABLE);  //Ê¹ÄÜ´®¿Ú1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//¿ªÆôÏà¹ØÖĞ¶Ï

	//Usart1 NVIC ÅäÖÃ
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//´®¿Ú1ÖĞ¶ÏÍ¨µÀ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//ÇÀÕ¼ÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//×ÓÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
	NVIC_Init(&NVIC_InitStructure);	//¸ù¾İÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯VIC¼Ä´æÆ÷¡¢

#endif
	
}

void send_date(u8 date[],u8 len)
{
	int t;
	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//µÈ´ı·¢ËÍ½áÊ
	USART_SendData(USART1,0x01);  
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//µÈ´ı·¢ËÍ½áÊø
	USART_SendData(USART1,len);  
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//µÈ´ı·¢ËÍ½áÊø
		for(t=0;t<len;t++)
			{
				USART_SendData(USART1, date[t]);         //Ïò´®¿Ú1·¢ËÍÊı¾İ
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//µÈ´ı·¢ËÍ½áÊø
			}
	USART_SendData(USART1,0x03);  
}

void USART1_IRQHandler(void)                	//´®¿Ú1ÖĞ¶Ï·şÎñ³ÌĞò
{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//Èç¹ûÊ±ÖÓ½ÚÅÄÊı¶¨ÒåÁË,ËµÃ÷ÒªÊ¹ÓÃucosIIÁË.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //½ÓÊÕÖĞ¶Ï(½ÓÊÕµ½µÄÊı¾İ±ØĞëÊÇ0x0d 0x0a½áÎ²)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//¶ÁÈ¡½ÓÊÕµ½µÄÊı¾İ
		
		if((USART_RX_STA&0x8000)==0)//½ÓÊÕÎ´Íê³É
		{
			if(USART_RX_STA&0x4000)//½ÓÊÕµ½ÁË0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//½ÓÊÕ´íÎó,ÖØĞÂ¿ªÊ¼
				else USART_RX_STA|=0x8000;	//½ÓÊÕÍê³ÉÁË 
			}
			else //»¹Ã»ÊÕµ½0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//½ÓÊÕÊı¾İ´íÎó,ÖØĞÂ¿ªÊ¼½ÓÊÕ	  
				}		 
			}
		}   		 
  } 
#ifdef OS_TICKS_PER_SEC	 	//Èç¹ûÊ±ÖÓ½ÚÅÄÊı¶¨ÒåÁË,ËµÃ÷ÒªÊ¹ÓÃucosIIÁË.
	OSIntExit();  											 
#endif
} 
#endif	

 



