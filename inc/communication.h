#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "global.h"

void SendString_USART(char * str, USART_TypeDef* usartx);
void Interpret_Message(char* str, uint16_t len);

#endif
