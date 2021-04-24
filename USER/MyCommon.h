#ifndef _MY_COMMON_H_
#define _MY_COMMON_H_

#include "stm32f10x.h"
#include "stdio.h"
#include <string.h>

#include "delay.h"

#include "ADC_DMA_Config.h"
#include "SPI.h"
#include "TIM.h"
#include "eeprom.h"
#include "usart.h"

#include "nrf24l01.h"

#include "remote.h"


#define BEEP_OFF	GPIOB->BSRR = GPIO_Pin_4		//∑‰√˘∆˜œÏ
#define BEEP_ON		GPIOB->BRR = GPIO_Pin_4		//∑‰√˘∆˜≤ªœÏ

#define LED_OFF		GPIOB->BSRR = GPIO_Pin_6		//LED≤ª¡¡
#define LED_ON		GPIOB->BRR = GPIO_Pin_6		//LED¡¡

extern uint32_t SysTick_count;

void All_Init(void);

#endif




