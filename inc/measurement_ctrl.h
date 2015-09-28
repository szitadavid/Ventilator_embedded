#ifndef MEASUREMENT_CTRL_H_
#define MEASUREMENT_CTRL_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "misc.h"
#include "global.h"

uint16_t Distance;					// Ball position in centimeters
uint16_t RPM;						// Motor frequency (rotation per minute)
uint16_t RefPos;


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

#endif
