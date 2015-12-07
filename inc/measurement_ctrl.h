#ifndef MEASUREMENT_CTRL_H_
#define MEASUREMENT_CTRL_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "misc.h"
#include "global.h"

uint16_t Distance;						// Ball position in centimeters
uint16_t RPM;						// Motor frequency (rotation per minute)
uint16_t RefPos;

typedef struct
{
	float Kc;
	float Ti;
	float Td;
	float Ts;

	float u_prev;
	float u_prevprev;
	float e_prev;
	float e_prevprev;

	float a1;
	float a2;
	float b0;
	float b1;
	float b2;
} PIDstruct;

PIDstruct myPID;


void Start_Ping(void);
void Init_Ping_IN(void);
void Init_Ping_OUT(void);
void Init_CNY70(void);
void Set_Distance(uint32_t time);
void Set_RPM(uint16_t data);
uint16_t Get_Distance(void);
uint16_t Get_RPM(void);
void Set_RefPos(uint16_t data);
uint16_t Get_RefPos(void);
float MyPIDControl(float error);
void SetMyPIDParameters(void);

#endif
