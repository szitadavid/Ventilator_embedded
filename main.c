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

extern Client clients[];


int main(void)
{
	int j;
	uint16_t DataTemp;
	char temp[100];
	char temp2[20];
	float error;
	char message[100];

	Init_CNY70();
	Init_TIM6();
	Init_TIM7();
	Init_UART1();		//Bluetooth
	Init_Ports();		//LEDs
	Init_UART3();		//WiFi
	Init_Hbridge();
	M_Default();
//	Init_TIM4();		//timer for data sending frequency

//	Default_MyPIDParam();

	SendData_Mask = 0;	//
	UserTyping  = 0;
	SendData = 0;
	control = 0;

	Set_RefPos((uint16_t) 80);

//	Create_Message(8,"message",BT_message);

	//Delay - Wifi and Bluetooth module startup
	for(j=0;j<1000000;j++);
	for(j=0;j<1000000;j++);
	for(j=0;j<1000000;j++);


	//Start client server on WiFi module
//	SendString_USART("AT+S.SOCKD=32000\r", USART_WIFI);

	for(j=0;j<1000000;j++);
	for(j=0;j<1000000;j++);
	for(j=0;j<1000000;j++);

	//Start distance measurement
	TIM_Cmd(TIM6,ENABLE);
	Start_Ping();

	//source: bluetooth
	BT_message[0]='B';
	Wifi_message[0]='W';


	clients[0].CLIENT_ACTIVE_SET;
	while(1)
    {
		if(BT_messagearrived)
		{
			Interpret_Message(BT_message, BT_messagearrived);
			SendData = 1;
			BT_messagearrived = 0;
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
			SendData = 0;
			error = Get_RefPos() - Get_Distance();
			double2StrDec(temp, error);
			string_cat(temp,";");
			DataTemp = M_Get_DC();
			uint162StrDec(temp2, DataTemp);
			string_cat(temp2,";");
			string_cat(temp,temp2);
			DataTemp = Get_RPM();
			uint162StrDec(temp2, DataTemp);
			string_cat(temp,temp2);
			Create_Message(0,temp,message);
			SendString_USART(message, USART_BLUETOOTH);
		}
    }
}

