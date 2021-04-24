#include "MyCommon.h"

uint32_t SysTick_count = 0;

//����ϵͳʱ��,ʹ�ܸ�����ʱ��
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
	
	/* ʹ�ܴ���1�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	//USART1  ����1ȫ���ж� 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //�����ȼ�1
	/*IRQͨ��ʹ��*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	/*����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1*/
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

void BEEP_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//����ʹ�ܸ��ù��ܲ��ܽ��йر�jtag
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);    //ʹ��GPIO��ʱ��  
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//�ر�jtag����ΪPB4��jtag���ܽ�
		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;          //������	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	BEEP_OFF;
}

void All_Init(void)
{		

	LED_ON;	//��ledָʾ��	
	
	NRF24L01_init();        //��ʼ��NRF24L01
	USART1_Config();        //��ʼ������1	
	delay_ms(100);	
	printf("\r\nstm32f103C8t6\r\n");//����1��ӡ��Ƭ���ͺ�

	ADC1_GPIO_Config();      //��ʼ��ADC IO	
	ADC1_Mode_Config();      //��ʼ��ADCģʽ	

	RC_INIT();               //У׼ҡ�����ݳ�ʼ��
	
	BEEP_INIT(); //����������
	{
		int32_t beep_cnt = 400000;
		while(beep_cnt-->0)
		{
			
		}
		BEEP_ON;
	}
		
	BEEP_OFF;	
	
}






