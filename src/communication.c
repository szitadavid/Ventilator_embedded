#include "communication.h"


char msg[100];

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
void Interpret_Message(char* str, uint16_t len, char* answer)
{
	uint8_t client_index;

	answer[0]='\0';
	str[len] = '\0';

	client_index = str[1] - '0';
	//valid BT ID
	if(client_index >= 0 && client_index < 10)
	{
		if(str[5] == 'C')
		{
			len = ucCI_CommandInterpreter(str+6, client_index, answer);
			answer[len] = '\0';
		}
	}
}


uint8_t string_cat(char* dest, const char* src)
{
	uint8_t i = 0, dest_len = 0;
	while(dest[dest_len] != '\0')
		dest_len++;
	for(i=0; src[i]!='\0'; i++)
	{
		dest[dest_len + i] = src[i];
	}
	dest[dest_len + i] = '\0';
	return dest_len + i;
}

uint8_t string_cpy(char* dest, char* src)
{
	uint8_t cnt = 0;
	while(src[cnt] != '\0')
	{
		dest[cnt] = src[cnt];
		cnt++;
	}
	dest[cnt]  = '\0';
	return cnt;
}


/*
 * Message format: [ID][len][len][len][message]...[/0] up to 315 byte message
 */
void Create_Message(uint8_t ID, const char* message, char* dest)
{
	char temp[6];
	uint8_t msg_len;
	dest[0] = ID + '0';

	msg_len = string_cpy(dest + 4, message);

	uint162StrDec_nc(temp,(uint16_t) msg_len);
	dest[1] = temp[2];
	dest[2] = temp[3];
	dest[3] = temp[4];
}

void SendHeartbeatRequest(void)
{
	uint8_t id;
	char msg[20];
	for(id=0; id<BT_CLIENT_MAX; id++)
	{
		if(clients[id].active != 0)
		{
			Create_Message(id,"HB",msg);
			SendString_USART(msg,USART_BLUETOOTH);
			SendData = 1;
		}
	}
}

void CreateFormattedMessage(uint8_t ID, char* formattedMessage)
{
	char temp2[20];
	uint16_t error;
	uint16_t DataTemp;
	formattedMessage[0] = '\0';
	//Readable format
	if(clients[ID].format == 0x00)
	{
		strcat(formattedMessage,"Not implemented function.\n");
	}
	//CSV format
	else if(clients[ID].format == 0x01)
	{
		error = Get_Distance();
		uint162StrDec(formattedMessage, error);
		strcat(formattedMessage,";");
		DataTemp = M_Get_DC();
		uint162StrDec(temp2, DataTemp);
		strcat(temp2,";");
		strcat(formattedMessage,temp2);
		DataTemp = Get_RPM();
		uint162StrDec(temp2, DataTemp);
		strcat(temp2,";\n");
		strcat(formattedMessage,temp2);
	}
}
