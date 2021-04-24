#include "MyCommon.h"
#include "remote.h"

//通信协议：
//起始帧0XAA,0XAF + 地址帧 0X03 + 数据长度 + 12个遥控数据 + 校验字节
__align(4) _st_Remote Remote={1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000};

void key(void);
	
uint16_t styb;
struct //校准数据
{
	int16_t flag;		//校准标志位
	int16_t roll;
	int16_t pitch;	
	int16_t thr;	
	int16_t yaw;	
}offset ;
/******************************************************************************
函数原型：
功    能：	按键初始化
*******************************************************************************/ 
void key_init(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;
	
	AFIO->MAPR = 0X02000000;   //使用四线SWD下载程序
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;    
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    //上拉输入
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

//	GPIO_Init(GPIOB, &GPIO_InitStructure);	

}
/******************************************************************************
函数原型：
功    能：	校准数据初始化
*******************************************************************************/ 
void RC_INIT(void)
{
	offset.flag = *(int16_t*)FLASH_Start_Addr;   //读出存储的数据
	
	if(offset.flag != 0x5AA5) //检查是否有校准数据 改变
	{
		delay_ms(1000);
		offset.roll = 0;//(uint16_t)(0.25f*ADC_ConvertedValue[0]) - 500;//方向;
		offset.pitch = 0;//500 -(uint16_t)(0.25f*ADC_ConvertedValue[1]);//方向
		
		offset.thr = (uint16_t)(0.25f*ADC_ConvertedValue[3]) - 1000;//油门;
		offset.yaw = (uint16_t)(0.25f*ADC_ConvertedValue[2]) - 500;//偏航
		offset.flag = 0x5AA5;
    
		printf("offset thr: %d   offset pitch: %d   offset yaw: %d   offset roll: %d \r\n",offset.thr,offset.pitch,offset.yaw,offset.roll);  //串口1输出2个摇杆数据		
		
		if(offset.thr <= 100)  //判断油门打最低才存储摇杆值  不然存储了就解锁不了飞机
	  {
			FLASH_write((int16_t *)&offset,5);
		}
	}
	else
	{
		offset.roll = *((int16_t*)FLASH_Start_Addr+1);
		offset.pitch = *((int16_t*)FLASH_Start_Addr+2);
		offset.thr = *((int16_t*)FLASH_Start_Addr+3);
		offset.yaw = *((int16_t*)FLASH_Start_Addr+4);	  
	}
	key_init();
}
/******************************************************************************
函数原型：
功    能：	10ms发送一次数据，每个通道都是高位在前低位在后
*******************************************************************************/ 
uint8_t tx_data[32] = {0xAA,0xAF,0x03,32-5};//匿名通信协议
void NRF_SEND(void)
{
	for(uint8_t i=4;i<26;i+=2)
	{
		tx_data[i] = *((uint8_t*)&Remote+i-3);	 //高位在前
		tx_data[31] += tx_data[i];
		tx_data[i+1] = *((uint8_t*)&Remote+i-4);	
		tx_data[31] += tx_data[i];
	}
	tx_data[31] = 0;
	for(uint8_t i=0;i<31;i++)  //校验位
	{
		tx_data[31] +=  tx_data[i];
	}
	NRF24L01_TxPacket((uint8_t*)&tx_data); //调用NRF发射数据
}
/******************************************************************************
函数原型：
功    能：	遥控数据解析   左油门程序
*******************************************************************************/ 
void RC_Analy(void) //10ms调用一次
{
	static uint16_t last_thr,last_roll,last_pitch,last_yaw;

	Remote.thr = 2000 - (uint16_t)(0.25f*ADC_ConvertedValue[3]) + offset.thr;//油门
	last_thr = Remote.thr = Remote.thr*0.25f + 0.75f*last_thr;

	Remote.pitch 	= 	1000 + (uint16_t)(0.25f*ADC_ConvertedValue[1]) + offset.pitch;//
	last_pitch = Remote.pitch = Remote.pitch*0.25f + 0.75f*last_pitch;
	
	Remote.yaw 		= 	2000 - (uint16_t)(0.25f*ADC_ConvertedValue[2]) + offset.yaw;//方向
	last_yaw = Remote.yaw = Remote.yaw*0.25f + 0.75f*last_yaw;
	
	Remote.roll 	=	2000 - (uint16_t)(0.25f*ADC_ConvertedValue[0]) + offset.roll;//副翼
	last_roll = Remote.roll = Remote.roll*0.25f + 0.75f*last_roll;
	
	styb++;
	if(styb >= 200)
	{	
		printf("前后: %d   左右: %d   航向: %d   油门: %d \r\n",Remote.pitch,Remote.roll,Remote.yaw,Remote.thr);  //串口1输出2个摇杆数据
		styb=0;
	}
	key();//扫描按键
	NRF_SEND();
}

#define _OK_	0
#define _ERR_	1
#define REMOOTE_KEY_VALUE_L1	1687	
#define REMOOTE_KEY_VALUE_L2	1000
#define REMOOTE_KEY_VALUE_L3	1856
#define REMOOTE_KEY_VALUE_L4	1822
#define REMOOTE_KEY_VALUE_L5	1771
#define REMOOTE_KEY_VALUE_L6	1519

#define REMOOTE_KEY_VALUE_R1	3350	
#define REMOOTE_KEY_VALUE_R2	0
#define REMOOTE_KEY_VALUE_R3	2060
#define REMOOTE_KEY_VALUE_R4	2728
#define REMOOTE_KEY_VALUE_R5	3046
#define REMOOTE_KEY_VALUE_R6	3230

uint32_t Myabs(int value)	
{
	if(value < 0) value = -value;
	return value;
}
/***********************************************比较目标键值和遥控器的按键值*************************/
static u8 CompareRemoteKeyValue(u16 key_value , u16 com_value)
{
	int err_value;

	err_value = key_value - com_value;
	if(Myabs(err_value) <= 150)
	{
		return _OK_;	
	}
	return _ERR_;
}  

/***********************************************获取遥控器的按键值函数*************************/
uint8_t GetKeyNum(void)		 //高4位位左侧值，低4位位右侧键值，BCD码格式
{
	u8 key_code = 0;

//	if(CompareRemoteKeyValue(ADC_ConvertedValue[4] , REMOOTE_KEY_VALUE_L1) == _OK_)	key_code |= 1<<4; 		//遥控器左侧按键阵列
//	else if(CompareRemoteKeyValue(ADC_ConvertedValue[4] , REMOOTE_KEY_VALUE_L2) == _OK_)	key_code |= 2<<4; 	//遥控器左侧按键阵列
//	else if(CompareRemoteKeyValue(ADC_ConvertedValue[4] , REMOOTE_KEY_VALUE_L3) == _OK_)	key_code |= 3<<4; 	//遥控器左侧按键阵列
//	else if(CompareRemoteKeyValue(ADC_ConvertedValue[4] , REMOOTE_KEY_VALUE_L4) == _OK_)	key_code |= 4<<4; 	//遥控器左侧按键阵列
//	else if(CompareRemoteKeyValue(ADC_ConvertedValue[4] , REMOOTE_KEY_VALUE_L5) == _OK_)	key_code |= 5<<4; 	//遥控器左侧按键阵列
//	else if(CompareRemoteKeyValue(ADC_ConvertedValue[4] , REMOOTE_KEY_VALUE_L6) == _OK_)	key_code |= 6<<4; 	//遥控器左侧按键阵列
////	else key_code |= 0<<4; 	//遥控器左侧按键阵列 

	if(CompareRemoteKeyValue(ADC_ConvertedValue[4] , REMOOTE_KEY_VALUE_R1) == _OK_)	key_code |= 1; 		//遥控器右侧按键阵列
	else if(CompareRemoteKeyValue(ADC_ConvertedValue[4] , REMOOTE_KEY_VALUE_R2) == _OK_)	key_code |= 2; 	//遥控器右侧按键阵列
	else if(CompareRemoteKeyValue(ADC_ConvertedValue[4] , REMOOTE_KEY_VALUE_R3) == _OK_)	key_code |= 3; 	//遥控器右侧按键阵列
	else if(CompareRemoteKeyValue(ADC_ConvertedValue[4] , REMOOTE_KEY_VALUE_R4) == _OK_)	key_code |= 4; 	//遥控器右侧按键阵列
	else if(CompareRemoteKeyValue(ADC_ConvertedValue[4] , REMOOTE_KEY_VALUE_R5) == _OK_)	key_code |= 5; 	//遥控器右侧按键阵列
	else if(CompareRemoteKeyValue(ADC_ConvertedValue[4] , REMOOTE_KEY_VALUE_R6) == _OK_)	key_code |= 6; 	//遥控器右侧按键阵列
//	else key_code |= 0; 	//遥控器左侧按键阵列 

	key_code = (key_code >> 4)*10 + (key_code & 0x0F);

//	if(key_code & (1 << 0))	P2P0 = 1;
//	else P2P0 = 0;	
//	if(key_code & (1 << 1))	P2P1 = 1;
//	else P2P1 = 0;	
//	if(key_code & (1 << 2))	P2P2 = 1;
//	else P2P2 = 0;	
//	if(key_code & (1 << 3))	P2P3 = 1;
//	else P2P3 = 0;	
	return key_code;
}

/******************************************************************************
函数原型：
功    能：	按键扫描
*******************************************************************************/ 
u8 key_num = 0;
u8 get_key_num = 0;
void key(void)
{
	#define KEY_RIGHT_UP		1
	#define KEY_PITCH_LEFT		2
	#define KEY_PITCH_RITHT		3	
	#define KEY_ROLL_UP			4
	#define KEY_ROLL_DOWN		5
	#define KEY_OK				6
	volatile static uint8_t status = 0;	
	static uint32_t temp;
	
	get_key_num = GetKeyNum();
	switch(status)
	{
		case 0:
			if(SysTick_count - temp >20) //200ms 按键音
			{
				if(get_key_num != 0) 
				{
					key_num = get_key_num;
					status = 1;
					BEEP_ON;
					LED_OFF;				
				}
			}
			break;
		case 1:
			{
				if(get_key_num == 0) status = 2;
			}
			break;
		case 2:
			{			
				switch(key_num)
				{
					case KEY_RIGHT_UP	: {offset.thr = 0;FLASH_write((int16_t *)&offset,5);}
						break;
					case KEY_ROLL_UP	: offset.roll = offset.roll - 5;	;
						break;
					case KEY_ROLL_DOWN	: offset.roll = offset.roll + 5;;
						break;	
					case KEY_PITCH_LEFT	: offset.pitch = offset.pitch + 5;;
						break;
					case KEY_PITCH_RITHT: offset.pitch = offset.pitch - 5;;
						break;
					case KEY_OK			: Remote.AUX7 ^= (2000^1000); //1000和2000之间变化	  可以留来一键空翻; 
						break;
					default				: key_num = 0;
				}
				key_num = 0;
				status = 0;			
				BEEP_OFF;  
				LED_ON;	
				temp = SysTick_count;
			}
			break;
		}			
}






