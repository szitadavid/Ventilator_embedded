#ifndef GLOBAL_H_
#define GLOBAL_H_

#define REVISION		"V2.0"
#define SPEED_OF_SOUND	344
#define HAND_CTRL		0
#define T_SAMPLE		0.06


//V2.0 defines

#define USART_WIFI			USART3
#define USART_BLUETOOTH		USART1
#define TERM_FBCK			0			//feedback through serial port

#define BT_CLIENT_MAX		5


uint8_t BT_messagearrived;
uint8_t Wifi_messagearrived;
uint8_t SendData_Mask;
uint8_t UserTyping;
uint8_t SendData;
uint8_t control;
uint8_t SendHeartbeat;

typedef struct Client
{
	uint8_t active;
	uint8_t heartbeat;
	uint8_t distance;
	uint8_t speed;
	uint8_t format;
} Client;

Client clients[5];

#endif
