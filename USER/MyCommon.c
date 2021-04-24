#include "MyCommon.h"

uint32_t SysTick_count = 0;

//配置系统时钟,使能各外设时钟
void RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA 
                           |RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC
						   |RCC_APB2Periph_ADC1  | RCC_APB2Periph_AFIO 
                           |RCC_APB2Periph_SPI1  
						   	, ENABLE );
	
   	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 | RCC_APB1Periph_I2C1| RCC_APB1Periph_TIM3, ENABLE );
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	/* 使能串口1中断 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	//USART1  串口1全局中断 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //子优先级1
	/*IRQ通道使能*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	/*根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1*/
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

void BEEP_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//必须使能复用功能才能进行关闭jtag
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);    //使能GPIO的时钟  
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//关闭jtag，因为PB4是jtag功能脚
		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;          //蜂鸣器	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	BEEP_OFF;
}

void All_Init(void)
{		

	LED_ON;	//打开led指示灯	
	
	NRF24L01_init();        //初始化NRF24L01
	USART1_Config();        //初始化串口1	
	delay_ms(100);	
	printf("\r\nstm32f103C8t6\r\n");//串口1打印单片机型号

	ADC1_GPIO_Config();      //初始化ADC IO	
	ADC1_Mode_Config();      //初始化ADC模式	

	RC_INIT();               //校准摇杆数据初始化
	
	BEEP_INIT(); //开机蜂鸣声
	{
		int32_t beep_cnt = 400000;
		while(beep_cnt-->0)
		{
			
		}
		BEEP_ON;
	}
		
	BEEP_OFF;	
	
}






