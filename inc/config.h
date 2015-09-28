#ifndef CONFIG_H_
#define CONFIG_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "misc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_usart.h"

void Init_Ports(void);
void Init_TIM3(void);
void Init_TIM4(void);
void Init_TIM6(void);
void Init_TIM7(void);
void Init_Hbridge(void);
void Init_UART1(void);
void Init_UART3(void);

#endif
