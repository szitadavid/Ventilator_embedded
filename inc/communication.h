#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "global.h"
#include <string.h>
#include "motor.h"

void SendString_USART(char * str, USART_TypeDef* usartx);
void Interpret_Message(char* str, uint16_t len, char* answer);
void Create_Message(uint8_t ID, const char* message, char* src);
void SendHeartbeatRequest(void);
void CreateFormattedMessage(uint8_t ID, char* formattedMessage);

uint8_t string_cat(char* dest, const char* src);
uint8_t string_cpy(char* dest, char* src);


#endif
