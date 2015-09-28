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
char Wifi_message[2000];

int main(void)
{
	int j;
	uint16_t DataTemp;
	char temp[20];

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


	//Delay - Wifi and Bluetooth module startup
	for(j=0;j<1000000;j++);
	for(j=0;j<1000000;j++);
	for(j=0;j<1000000;j++);


	//Start distance measurement
	TIM_Cmd(TIM6,ENABLE);
	Start_Ping();

	//source: bluetooth
	BT_message[0]='B';

	while(1)
    {
		if(BT_messagearrived)
		{
			Interpret_Message(BT_message, BT_messagearrived);

			BT_messagearrived = 0;
		}
		if(Wifi_messagearrived)
		{
			Wifi_messagearrived = 0;
			SendString_USART(Wifi_message,USART_BLUETOOTH);
		}

		if(SendData && !UserTyping && SendData_Mask)
		{
			SendData = 0;
			DataTemp = Get_Distance();
			uint162StrDec(temp, DataTemp);
			SendString_USART(temp, USART_BLUETOOTH);
			SendString_USART("\r\n", USART_BLUETOOTH);
		}


    }
}

