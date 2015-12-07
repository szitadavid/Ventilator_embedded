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
char Wifi_message[100];


int main(void)
{
	int j;
	float error;
	char message[100];
	char fmessage[100];
	char answer[100];
	uint8_t currentID = 0;


	Init_CNY70();
	Init_TIM6();
	Init_TIM7();
	Init_UART1();		//Bluetooth
	Init_Ports();		//LEDs
	Init_UART3();		//WiFi
	Init_Hbridge();
	M_Default();
	//Init_TIM4();		//timer for data sending frequency

	Default_MyPIDParam();


	SendData_Mask = 0;
	UserTyping  = 0;
	SendData = 0;
	control = 0;
	SendHeartbeat = 0;

	Set_RefPos((uint16_t) 80);

	for(j=0; j<BT_CLIENT_MAX; j++)
	{
		clients[j].active = 0;
		clients[j].heartbeat = 0;
		clients[j].format = 0x01;
		clients[j].distance = 0;
		clients[j].speed = 0;
	}

	//Delay - Wifi and Bluetooth module startup
	for(j=0;j<1000000;j++);
	for(j=0;j<1000000;j++);
	for(j=0;j<1000000;j++);


	//Start soceket server on WiFi module
	SendString_USART("AT+S.SOCKD=32000\r", USART_WIFI);

	for(j=0;j<1000000;j++);
	for(j=0;j<1000000;j++);
	for(j=0;j<1000000;j++);

	//Start distance measurement
	TIM_Cmd(TIM6,ENABLE);
	Start_Ping();

	//source: bluetooth
	BT_message[0]='B';
	Wifi_message[0]='W';

	while(1)
    {
		if(BT_messagearrived)
		{
			Interpret_Message(BT_message, BT_messagearrived, answer);
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
			Create_Message(0,Wifi_message+1,message);
			SendString_USART(message,USART_BLUETOOTH);

			if(Wifi_message[1] == 'B' && Wifi_message[2] == 'Y' && Wifi_message[3] == 'E')
				SendString_USART("AT+S.\r",USART_WIFI);
			Wifi_messagearrived = 0;
		}

		//ToDo: parancs annak eldöntésére, h .csv formátumban kellenek az adatok, vagy szemmel is olvasható formátumban



		if(control == 2)
		{
			control = 0;
			error = Get_RefPos() - Get_Distance();
			M_Set_DC((uint16_t) MyPIDControl(error));
			SendData = 1;
		}

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

