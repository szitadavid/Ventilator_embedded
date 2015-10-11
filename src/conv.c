//*****************************************************************************
// Dependencies
//*****************************************************************************

#include "conv.h"


//*****************************************************************************
// END: Dependencies
//*****************************************************************************


//*****************************************************************************
// Private Defines + Private Variables
//*****************************************************************************

//*****************************************************************************
// END: Private Defines + Private Variables
//*****************************************************************************


//*****************************************************************************
// Private function prototypes
//*****************************************************************************

//*****************************************************************************
// END: Private function prototypes
//*****************************************************************************


/******************************************************************************
* FUNCTION: uint8_t ucHexStr2Char                                             *
*-----------------------------------------------------------------------------*
* PURPOSE                                                                     *
*   Converts hex string to uint8_t.                                           *
*-----------------------------------------------------------------------------*
* PARAMETER                                                                   *
*    char* ucpString: The source string.                                      *
* END                                                                         *
* RETURN:                                                                     *
*   int16_t: The converted value                                              *
******************************************************************************/
uint8_t ucHexStr2Char(char* ucpString)
{
uint8_t ucChar;
uint8_t ucLength = 0;
uint8_t ucResult = 0;

    if ((ucpString[1] == 'x') || (ucpString[1] == 'X')) ucpString += 2;

    while (  (    ('0' <= ucpString[ucLength] && '9' >= ucpString[ucLength])
               || ('A' <= ucpString[ucLength] && 'F' >= ucpString[ucLength])
               || ('a' <= ucpString[ucLength] && 'f' >= ucpString[ucLength]) )
          && (ucLength < 2) )
    {
        ucLength++;
    }//while
    if (!ucLength) return(0);
    
    if (ucLength == 2)
    {
        ucChar = *ucpString;
        if( ('0'<= ucChar) && ('9'>= ucChar) ) ucChar = (ucChar & 0x0F);
        else if(   ( ('A' <= ucChar) && ('F' >= ucChar) ) 
                || ( ('a' <= ucChar) && ('f' >= ucChar) ) ) 
                    ucChar = (ucChar & 0x0F) + 9;
        ucResult |= ucChar << 4;
        ucpString++;
    }//if (ucLength == 2)
    ucChar = *ucpString;
    if( ('0'<= ucChar) && ('9'>= ucChar) ) ucChar = (ucChar & 0x0F);
    else if(   ( ('A' <= ucChar) && ('F' >= ucChar) ) 
            || ( ('a' <= ucChar) && ('f' >= ucChar) ) ) 
                ucChar = (ucChar & 0x0F) + 9;
    ucResult |= ucChar;
    return(ucResult);
}// int16_t iHexStr2Int( uint8_t* ucpString)


/******************************************************************************
* FUNCTION: void Char2HexStr                                                  *
*-----------------------------------------------------------------------------*
* PURPOSE                                                                     *
*   Converts character to hex string.                                         *
*-----------------------------------------------------------------------------*
* PARAMETER                                                                   *
*   uint8_t ucValue: The source value                                         *
*   char* ucpString: The destination string                                   *
* END                                                                         *
* RETURN:                                                                     *
*   void                                                                      *
******************************************************************************/
void Char2HexStr(uint8_t ucValue, char* ucpString,uint8_t uc0x)
{
    uint8_t ucTemp;

    if (uc0x)
    {
        *ucpString++ = '0';
        *ucpString++ = 'x';
    }//if (uc0x)
    
    if (ucValue >= 16)
    {
      ucTemp = ucValue >> 4;
      if (ucTemp < 10) *ucpString++ = '0' + ucTemp;
      else *ucpString++ = 'A' + ucTemp - 10;
    }
    
    ucTemp = ucValue & 0x0f;
    if (ucTemp < 10) *ucpString++ = '0' + ucTemp;
    else *ucpString++ = 'A' + ucTemp - 10;

    *ucpString = '\0';
}// void Char2HexStr(uint8_t ucValue,  uint8_t* ucpString)

uint16_t Str2uint16(char *str)
{
	uint16_t val;
	val = (uint16_t) strtol(str, NULL, 16);

	return val;
}

void uint162Str(char *str, uint16_t data)
{
	str[4] = '\r';
	str[5] = '\n';

	if(((0x000F & data) >> 0) < 10)
		str[3] = '0' + ((0x000F & data) >> 0);
	else
		str[3] = 'A' + ((0x000F & data) >> 0) - 10;

	if(((0x00F0 & data) >> 4) < 10)
		str[2] = '0' + ((0x00F0 & data) >> 4);
	else
		str[2] = 'A' + ((0x00F0 & data) >> 4) - 10;

	if(((0x0F00 & data)>>8) < 10)
		str[1] = '0' + ((0x0F00 & data) >> 8);
	else
		str[1] = 'A' + ((0x0F00 & data) >> 8) - 10;

	if(((0xF000 & data) >> 12) < 10)
		str[0] = '0' + ((0xF000 & data) >> 12);
	else
		str[0] = 'A' + ((0xF000 & data) >> 12) - 10;
}

void uint162StrDec (char* str, uint16_t data)
{
	uint8_t i, j;
	//max value 65535 -> 5 characters
	if(data == 0)
	{
		str[0]='0';
		str[1]='\0';
	}
	else
	{
		for(i=1; i<6; i++)
		{
			str[5-i] = (uint8_t) ((data % 10UL) + '0');
			data/=10;
		}
		str[i-1] = '\0';

		//cutting zeros
		i=0;
		while(str[i] == '0')
		  i++;
		for(j=0; i+j < 6; j++)
			str[j] = str [i+j];
	}
}

void uint162StrDec_nc (char* str, uint16_t data)
{
	uint8_t i, j;

	if(data == 0)
	{
		str[0]='0';
		str[1]='\0';
	}
	else
	{
		for(i=1; i<6; i++)
		{
			str[5-i] = (uint8_t) ((data % 10UL) + '0');
			data/=10;
		}
		str[i-1] = '\0';
	}
}

uint16_t DecString2uint16(char* str)
{
	uint8_t len, i=0;
	uint16_t num = 0, mply;

	len = strlen(str);

	if(len > 4)
		return 0;

	mply=1;

	for(len; len > 0; len--)
	{
		num = num + mply * (str[len-1] - '0');
		mply *= 10;
	}
	return num;
}

double DecString2double(char* str)
{
	uint8_t dot = 0, i, minus = 0;
	double mply = 1;
	double val = 0;
	if(str[0] == '-')
	{
		str = str + 1;
		minus = 1;
	}

	while(str[dot] != '.' && str[dot] != ',' && str[dot] != '\0')
		dot++;

	for(i = 1; i <= dot; i++)
	{
		val += mply * (str[dot-i] - '0');
		mply *= 10;
	}

	if(str[dot] != '\0')
	{
		i = dot+1;
		mply = 0.1;
		while(str[i] != '\0')
		{
			val += mply * (str[i] - '0');
			mply /= 10;
			i++;
		}
	}
	if(minus)
		return -1 * val;
	else
		return val;
}


void double2StrDec (char* str, float data)
{
	uint8_t len;
	if(data < 0)
	{
		*str = '-';
		str++;
		data = -1 * data;
	}
	uint162StrDec(str, (uint16_t) data);
	len = strlen(str);
	str[len]=',';
//	str[len]='.';
	data = data - (uint16_t) data;
	data *= 10000;

	uint162StrDec_nc(str + len + 1, (uint16_t) data);
}
