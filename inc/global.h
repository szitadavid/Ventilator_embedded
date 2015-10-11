#ifndef GLOBAL_H_
#define GLOBAL_H_

#define REVISION		"V2.0"
#define SPEED_OF_SOUND	344
#define HAND_CTRL		1
#define T_SAMPLE		0.04


//V2.0 defines

#define USART_WIFI			USART3
#define USART_BLUETOOTH		USART1
#define TERM_FBCK			0			//feedback through serial port

#define CLIENT_ACTIVE_SET		settings |= (1<<0)
#define CLIENT_ACTIVE_RESET		settings &= ~(1<<0)
#define CLIENT_MASTER_SET		settings |= (1<<1)
#define CLIENT_MASTER_RESET		settings &= ~(1<<1)

uint8_t BT_messagearrived;
uint8_t Wifi_messagearrived;
uint8_t SendData_Mask;
uint8_t UserTyping;
uint8_t SendData;
uint8_t control;

typedef struct Client
{
	uint8_t settings;
} Client;


#endif
