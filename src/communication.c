#include "communication.h"

char answer[200];
uint8_t answerlen;


void SendString_USART(char * str, USART_TypeDef* usartx)
{
	while(*str != '\0')
	{
		USART_SendData(usartx, *str);
		str++;
		/* Loop until the end of transmission */
		while(USART_GetFlagStatus(usartx, USART_FLAG_TC) == RESET)
		{
		}
	}
}

//üzenet értelmezése, küldõ, címzett és típus megállapítása, ez alapján cselekvés
void Interpret_Message(char* str, uint16_t len)
{
	//Config message
	if(str[2] == 'C')
	{
		//source: bluetooth
		if(str[0] == 'B')
		{
			switch(str[1])
			{
			//target: bluetooth
			case 'B':
				//not implemented
				break;
			//target: wifi
			case 'W':
				str[len] = '\r'; 		//close for wifi command
				str[len+1] = '\0';		//close string
				SendString_USART("Sending wifi command: ", USART_BLUETOOTH);
				SendString_USART(str+3,USART_BLUETOOTH);
				SendString_USART("\r\n", USART_BLUETOOTH);
				SendString_USART(str+3,USART_WIFI);
				break;
			case 'S':
				answerlen = ucCI_CommandInterpreter(str+3, len-3, answer);
				answer[answerlen] = '\0';
				SendString_USART("System answer: ",USART_BLUETOOTH);
				SendString_USART(answer,USART_BLUETOOTH);
				break;
			default:
				SendString_USART("Wrong target code!", USART_BLUETOOTH);
				break;
			}
		}

		//source: wifi
		else if(str[0] == 'W')
		{}
	}
}
