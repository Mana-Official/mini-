#include "MyCommon.h"

void BEEP_INIT(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);
	
int main(void)
{
	RCC_Configuration();	
	CycleCounterInit();
	SysTick_Config(SystemCoreClock / 1000);	//ϵͳ�δ�ʱ�� 1ms����һ���ж�
	NVIC_Configuration();
	
	All_Init(); //15ms�жϴ����������SYS.C
	
	while(1)
	{

	}
}

void SysTick_Handler(void)
{   
	static uint8_t Count_10ms;
	
	SysTick_count++;	
	
	if(Count_10ms++>=1)
	{
		Count_10ms = 0;
		RC_Analy();
	}
}



