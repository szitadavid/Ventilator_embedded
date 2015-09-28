#ifndef MOTOR_H_
#define MOTOR_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "global.h"

#define M_D1_PORT			GPIOB
#define M_D1_PIN			GPIO_Pin_6
#define M_EN_PORT			GPIOC
#define M_EN_PIN			GPIO_Pin_7
#define M_IN1_PORT			GPIOB
#define M_IN1_PIN			GPIO_Pin_0
#define M_IN2_PORT			GPIOB
#define M_IN2_PIN			GPIO_Pin_1

uint16_t M_DutyCycle;

void M_Default(void);
void M_Rotate_CW(void);
void M_Set_DC(uint16_t speed);
uint16_t M_Get_DC(void);

#endif
