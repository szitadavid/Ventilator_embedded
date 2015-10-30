#include "communication.h"


char msg[100];

// Client table
Client clients[5] = {
		{0},
		{0},
		{0},
		{0},
		{0},
};

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
		clients[client_index].CLIENT_ACTIVE_SET;
		clients[client_index].CLIENT_MASTER_RESET;
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
