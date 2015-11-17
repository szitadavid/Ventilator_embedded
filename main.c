#include "config.h"
#include "stm32f10x.h"
#include "motor.h"
#include "ci.h"
#include "communication.h"
#include "stm32f10x_gpio.h"
#include "measurement_ctrl.h"
#include "conv.h"
#include <string.h>

char BT_message[100];
char command2[100];
char Wifi_message[100];




int main(void)
{
	int j;
	uint16_t i=0;
	uint16_t DataTemp;
	char temp[100];
	char temp2[20];
	float error;
	char message[100];
	char fmessage[100];
	char answer[100];
	uint8_t currentID = 0;

	uint16_t testtomb[120];

	Init_CNY70();
	Init_TIM6();
	Init_TIM7();
	Init_UART1();		//Bluetooth
	Init_Ports();		//LEDs
	Init_UART3();		//WiFi
	Init_Hbridge();
	M_Default();
	Init_TIM4();		//timer for data sending frequency

//	Default_MyPIDParam();


	SendData_Mask = 0;	//
	UserTyping  = 0;
	SendData = 0;
	control = 0;
	SendHeartbeat = 0;

	Set_RefPos((uint16_t) 80);

//	Create_Message(8,"message",BT_message);

	for(j=0;j<5;j++)
	{
		clients[j].active = 0;
		clients[j].heartbeat = 0;
		clients[j].settings = 0;
		clients[j].format = 0x01;
		clients[j].distance = 0;
		clients[j].speed = 0;
	}

	//Delay - Wifi and Bluetooth module startup
	for(j=0;j<1000000;j++);
	for(j=0;j<1000000;j++);
	for(j=0;j<1000000;j++);


	//Start client server on WiFi module
//	SendString_USART("AT+S.SOCKD=32000\r", USART_WIFI);

	for(j=0;j<1000000;j++);
	for(j=0;j<1000000;j++);
	for(j=0;j<1000000;j++);
	j=0;
	//Start distance measurement
	TIM_Cmd(TIM6,ENABLE);
	Start_Ping();

	//source: bluetooth
	BT_message[0]='B';
	Wifi_message[0]='W';


	j=0;


	while(1)
    {
		if(BT_messagearrived)
		{
			Interpret_Message(BT_message, BT_messagearrived, answer);
			//SendData = 1;
			BT_messagearrived = 0;
			if(answer[0] != '\0')
			{
				Create_Message(0,answer,message);
				SendString_USART(message, USART_BLUETOOTH);
			}
		}

		if(Wifi_messagearrived)
		{
			//Interpret_Message(Wifi_message,Wifi_messagearrived);
			//Create_Message(0,message,Wifi_message);
			//SendString_USART(message,USART_BLUETOOTH);

			Wifi_messagearrived = 0;
		}

		//ToDo: parancs annak eldöntésére, h .csv formátumban kellenek az adatok, vagy szemmel is olvasható formátumban

		//format: error;control(duty cycle);motorspeed
		if(SendData)
		{
			if(clients[currentID].active != 0)
			{
				CreateFormattedMessage(currentID, fmessage);
				Create_Message(currentID, fmessage, message);
				SendString_USART(message, USART_BLUETOOTH);
			}
			currentID++;
			if(currentID > BT_CLIENT_MAX)
			{
				currentID = 0;
				SendData = 0;
			}
		}

		if(SendHeartbeat)
		{
			SendHeartbeat = 0;
			SendString_USART(message, USART_BLUETOOTH);
			SendHeartbeatRequest();
		}
    }
}

