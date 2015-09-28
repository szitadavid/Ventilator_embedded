#ifndef GLOBAL_H_
#define GLOBAL_H_

#define REVISION		"V2.0"
#define SPEED_OF_SOUND	344
#define HAND_CTRL		0
#define T_SAMPLE		0.04


//V2.0 defines

#define USART_WIFI			USART3
#define USART_BLUETOOTH		USART1
#define TERM_FBCK			0			//feedback through serial port

uint8_t BT_messagearrived;
uint8_t Wifi_messagearrived;
uint8_t SendData_Mask;
uint8_t UserTyping;
uint8_t SendData;
uint8_t control;

#endif
