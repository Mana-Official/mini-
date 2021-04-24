#ifndef _REMOTE_H
#define _REMOTE_H

#include "stdint.h"

typedef volatile struct 
{
	uint16_t roll;
	uint16_t pitch;
	uint16_t thr;
	uint16_t yaw;	
	uint16_t AUX1;
	uint16_t AUX2;
	uint16_t AUX3;
	uint16_t AUX4;
	uint16_t AUX5;
	uint16_t AUX6;
	uint16_t AUX7;
}_st_Remote;

extern _st_Remote Remote;

void  RC_INIT(void);
void RC_Analy(void);

#endif





