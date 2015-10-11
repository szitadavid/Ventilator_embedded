//*****************************************************************************
// Dependencies
//*****************************************************************************

#include "ci.h"
//*****************************************************************************
// END: Dependencies
//*****************************************************************************

//*****************************************************************************
// Private Defines + Private Variables
//*****************************************************************************

const stCommItem G_staCommands[] = {
                                   {"SP", d_SP, "/Speed/ Motor speed (duty cycle) given in hexa, range: 0x000 - 0xFFFF. (e.g. SP=12AB)"},
                                   {"DC", d_DC, "/Duty Cycle/ Motor speed (duty cycle) given in decimal, range: 0-100 [%]. (e.g. DC=50)"},
                                   {"GS", d_GS, "/Get Speed/ Getting the motor speed (in RPM) is enabled (1) or disabled (0). (e.g. GS=0)"},
                                   {"GD", d_GD, "/Get Distance Getting the distance (in cm) is enabled (1) or disabled (0). (e.g. GD=0)"},
                                   {"RP", d_RP, "/Reference position/ Set reference position. Minimum 20 cm, maximum 140 cm."},
                                   {"",0}
                                   };
                                  
//*****************************************************************************
// END: Private Defines + Private Variables
//*****************************************************************************


//*****************************************************************************
// Private function prototypes
//*****************************************************************************

void CI_BuildAnswer(char* ucpDest, const char* ucpStatus,char* ucpAnswer);
uint8_t ucCI_SpaceFilter(char* ucpString);
uint8_t ucCI_CheckHelp(char* ucpString);
uint8_t ucCI_CommandCoder(char* ucpString);
uint8_t ucCI_GetNumberOfParameters(uint8_t* ucpCommand);
uint8_t ucCI_CheckParameter(uint8_t ucParameterNumber,char* ucpCommand,
                                     uint8_t ucParamType, uint8_t ucMaxLength);
uint8_t ucCI_IsHex(uint8_t ucChar);
uint8_t ucCI_GetByteParameter(uint8_t ucParamNum, char* ucpCommand);

//saját
uint8_t ucCI_GetParameter(char* ucpCommand, char answer[]);

//*****************************************************************************
// END: Private function prototypes
//*****************************************************************************


/******************************************************************************
* FUNCTION:  uiCI_CommandInterpreter                                          *
*-----------------------------------------------------------------------------*
* PURPOSE                                                                     *
*   The main CI command interpreter                                           *
*-----------------------------------------------------------------------------*
* PARAMETER                                                                   *
*    char* ucpCommand:          Address of the Master command string          *
*    uint8_t  ucLength: 		ID of the client			                  *
*    char* ucpAnswer: 	        Address of the Slave buffer	              *
* END                                                                         *
* RETURN:                                                                     *
*    uint8_t:                   Size of the contents of the slave buffer      *
******************************************************************************/
uint8_t ucCI_CommandInterpreter(char* ucpCommand,
                                uint8_t  clientID,
                                char* ucpAnswer)
{
  uint8_t ucCommandCode;
  uint32_t val;


  ucCommandCode = ucCI_CommandCoder(ucpCommand);

  switch(ucCommandCode)
  {
	/////////////////////////////////////////////////////////////////////
	case d_SP:
		if (!ucCI_CheckParameter(1, ucpCommand, d_PTYP_X | d_PTYP_LAST, 4))
		{

			val = Str2uint16(ucpCommand+3);
			val = val *1000 / 0xFFFF;
			M_Set_DC((uint16_t) val);
			CI_BuildAnswer(ucpAnswer, d_E0, NULL);
		}
		else
			CI_BuildAnswer(ucpAnswer, d_InvalidParameter, NULL);
		break;
	case d_DC:
		if (!ucCI_CheckParameter(1, ucpCommand, d_PTYP_X | d_PTYP_LAST, 4))
		{
			val = DecString2uint16(ucpCommand+3);
			M_Set_DC((uint16_t) val);
			CI_BuildAnswer(ucpAnswer, d_E0, NULL);
		}
		else
			CI_BuildAnswer(ucpAnswer, d_InvalidParameter, NULL);
		break;
	case d_GS:
		if (!ucCI_CheckParameter(1, ucpCommand, d_PTYP_10 | d_PTYP_LAST, 0))
		{
			if(ucpCommand[3] == '1')
				SendData_Mask |= (1<<0);
			else if(ucpCommand[3] == '0')
				SendData_Mask &= ~(1<<0);
			CI_BuildAnswer(ucpAnswer, d_E0, NULL);
		}
		else
			CI_BuildAnswer(ucpAnswer, d_InvalidParameter, NULL);
		break;
	case d_GD:
		if (!ucCI_CheckParameter(1, ucpCommand, d_PTYP_10 | d_PTYP_LAST, 0))
		{
			if(ucpCommand[3] == '1')
				SendData_Mask |= (1<<1);
			else if(ucpCommand[3] == '0')
				SendData_Mask &= ~(1<<1);
			CI_BuildAnswer(ucpAnswer, d_E0, NULL);
		}
		else
			CI_BuildAnswer(ucpAnswer, d_InvalidParameter, NULL);
		break;
	case d_RP:
		if (!ucCI_CheckParameter(1, ucpCommand, d_PTYP_X | d_PTYP_LAST, 3))
		{

			val = DecString2uint16(ucpCommand+3);
			Set_RefPos((uint16_t) val);
			CI_BuildAnswer(ucpAnswer, d_E0, NULL);
		}
		else
			CI_BuildAnswer(ucpAnswer, d_InvalidParameter, NULL);
		break;
	default:

	  CI_BuildAnswer(ucpAnswer, d_UnknownASCIICommand, NULL);
	  break;
  }

  return (strlen(ucpAnswer) );

}// ucCI_CommandInterpreter


/******************************************************************************
* FUNCTION: CI_BuildAnswer                                                    *
*-----------------------------------------------------------------------------*
* PURPOSE                                                                     *
*   Builds the answer string for a command.
*-----------------------------------------------------------------------------*
* PARAMETER                                                                   *
*   char* ucpDest: The destination string                                     *
*   char* ucpStatus:  The command status string                               *
*   char* ucpAnswer:  The result of the command (NULL if not needed)          *
* END                                                                         *
* RETURN:                                                                     *
*   void                                                                      *
******************************************************************************/
void CI_BuildAnswer(char* ucpDest, const char* ucpStatus,char* ucpAnswer)
{
	  if(ucpStatus != NULL)
	  {
	    strcpy(ucpDest, ucpStatus);
	  }

	  if( (ucpAnswer != NULL) && (ucpAnswer[0] != '\0'))
	  { // Answer is appended
	    strcat(ucpDest, ucpAnswer);
	  }
	  strcat(ucpDest, "\r\n");
}

uint8_t ucCI_CheckHelp(char* ucpString)
{
	if(ucpString[2] != '=')
		return 0;
	else
		return 1;
}

/******************************************************************************
* FUNCTION: ucCI_CommandCoder                                                 *
*-----------------------------------------------------------------------------*
* PURPOSE                                                                     *
*   Generates the code of the command from the command string.                *
*-----------------------------------------------------------------------------*
* PARAMETER                                                                   *
*   char* ucpString: The command string                                       *
* END                                                                         *
* RETURN:                                                                     *
*   uint8_t: The code of the command                                          *
******************************************************************************/
uint8_t ucCI_CommandCoder(char* ucpString)
{
uint8_t ucByte1;
uint8_t ucByte2;
uint8_t i = 0;
uint8_t ucReturn = 0;

    ucByte1 = ucpString[0];
    ucByte2 = ucpString[1];
    if ( (ucByte1 >= 'a') && (ucByte1 <= 'z') ) ucByte1 -= 0x20;
    if ( (ucByte2 >= 'a') && (ucByte2 <= 'z') ) ucByte2 -= 0x20;

    while (G_staCommands[i].cpString[0] != '\0')
    {
        if ( (ucByte1 == G_staCommands[i].cpString[0]) && (ucByte2 == G_staCommands[i].cpString[1]) )
        {
            ucReturn = G_staCommands[i].ucCode;
            break;
        }// if
        i++;
    }// while (G_staCommands[i][0] != '\0')
    return(ucReturn);
}// uint8_t ucCI_CommandCoder( uint8_t* ucpString)

/******************************************************************************
* FUNCTION: ucCI_GetNumberOfParameters                                        *
*-----------------------------------------------------------------------------*
* PURPOSE                                                                     *
*   Returns with the number of parameters in the command string.              *
*-----------------------------------------------------------------------------*
* PARAMETER                                                                   *
*    uint8_t* ucpCommand:       Address of the command string                 *
* END                                                                         *
* RETURN:                                                                     *
*   uint8_t: The number of the parameters in the command string.              *
******************************************************************************/
uint8_t ucCI_GetNumberOfParameters(uint8_t* ucpCommand)
{
    uint8_t ucParamNum = 0;
    uint8_t* ucpStartPoi = ucpCommand;

    ucpCommand += 2;
    if (*ucpCommand == '=') ucpCommand++;
    while (*ucpCommand != 0)
    {
        if( (*ucpCommand == ',') || (*ucpCommand == '=') ) ucParamNum++;
        ucpCommand++;
    }//while (*ucpCommand != 0)
    if ( (*(ucpCommand-1) != ',') && (*(ucpCommand-1) != '=') && (ucpCommand > (ucpStartPoi + 2)) ) ucParamNum++;
    return(ucParamNum);
}


/******************************************************************************
* FUNCTION: ucCI_CheckParameter                                               *
*-----------------------------------------------------------------------------*
* PURPOSE                                                                     *
*   Checks the specified parameter in the command string.                     *
*-----------------------------------------------------------------------------*
* PARAMETER                                                                   *
*   uint8_t ucParameterNumber: The number of the parameter in the             *
*                                    command                                  *
*   char* ucpCommand:          Address of the command string                  *
*   uint8_t ucParamType:       The type of the parameter                      *
*   uint8_t ucMaxLength:       The maximum length of the parameter            *
*                                    in characters                            *
* END                                                                         *
* RETURN:                                                                     *
*   uint8_t: 0 - Parameter OK; otherwise the error code                       *
******************************************************************************/
uint8_t ucCI_CheckParameter(uint8_t ucParameterNumber,char* ucpCommand,
                                     uint8_t ucParamType, uint8_t ucMaxLength)
{
    uint8_t ucTyp = ucParamType & 0x0f;
    char* ucpPoi;
    uint8_t i,j;

    ucpPoi = ucpCommand + 2;// Behind the two command character

    // Search the number of the parameter
    if (*ucpPoi == 0)
    {
        if (ucParameterNumber == 0) return(0);// No parameter
        else return(1);// There is unknown parameter
    }

    if (*ucpPoi == '=') ucpPoi++;
    else return(11); //missing '='
    for (i = 1; i < ucParameterNumber; i++)
    {
        while ((*ucpPoi != '=') && (*ucpPoi != ';') && (*ucpPoi != 0)) ucpPoi++;		//paraméterek pontosvesszõvel elválasztva
        if (*ucpPoi == 0) break;// Not enough parameter     // BUGFIX: KDO (*ucpPoi)
        ucpPoi++;
    }// for (i = 1; i <= ucParameterNumber; i++)
    if (i != ucParameterNumber) return(2); // Not enough parameter
    if (*ucpPoi == ',') return(10);// ",,"
    
    switch (ucTyp)
    {
        //--------------------------------------------------------------------------------------
        case d_PTYP_X:
            for (i = 0; i < ucMaxLength; i++)
            {
                if (ucCI_IsHex(*ucpPoi)== 255) break;
                ucpPoi++;
            }// for (i = 0; i < ucMaxLength; i++)
            if (i == 0) return(4);// No parameter
            if (*ucpPoi == 0)
            {
                if (ucParamType & d_PTYP_LAST) return(0);
                else return(5);
            }
            if (ucCI_IsHex(*ucpPoi) != 255) return(6);// There is still hex character
            if ((*ucpPoi != ',') && (*ucpPoi != '=') && !(ucParamType & d_PTYP_LAST)) return(7);// syntax errror between two parameters
            if ((*ucpPoi != 0)   &&  (ucParamType & d_PTYP_LAST)) return(8);// Too many parameters
            return(0);
        //--------------------------------------------------------------------------------------
        case d_PTYP_10:
            if ((*ucpPoi != '0') && (*ucpPoi != '1')) return(9);
            ucpPoi++;
            if ((*ucpPoi != ',') && (*ucpPoi != '=') && !(ucParamType & d_PTYP_LAST)) return(7);// syntax errror between two parameters
            if ((*ucpPoi != 0)   &&  (ucParamType & d_PTYP_LAST)) return(8);// Too many parameters
            return(0);
        //--------------------------------------------------------------------------------------
        case d_PTYP_ab:
            if ( (*ucpPoi != 'a') && (*ucpPoi != 'b') && (*ucpPoi != 'A') && (*ucpPoi != 'B') ) return(1);
            ucpPoi++;
            if ( !(ucParamType & d_PTYP_LAST) && (*ucpPoi == 0)) return(2);
            if ( !(ucParamType & d_PTYP_LAST) && (*ucpPoi != '=') && (*ucpPoi != ',') ) return(3);
            if ( !(ucParamType & d_PTYP_LAST) && ((*ucpPoi == '=') || (*ucpPoi == ',')) ) return(0);
            if (  (ucParamType & d_PTYP_LAST) && (*ucpPoi != 0))  return(4);
            if (  (ucParamType & d_PTYP_LAST) && (*ucpPoi == 0))  return(0);
            return(5);
        case d_PTYP_dbl:
        		i = 0;
        		j = 0;
        		while((*ucpPoi >= '0' && *ucpPoi <= '9') || *ucpPoi == '.' || *ucpPoi == '-')
        		{
        			if(*ucpPoi == '-')
        				if(i != 0)
        					return(5);
        			if(*ucpPoi == '.')
        				j++;
        			if(j > 1)
        				return(5);
        			ucpPoi++;
        			i++;
        		}
        		if(*ucpPoi != '\0')
        			return(5);
            return(0);
        //--------------------------------------------------------------------------------------
        default:// Unknown parameter
            return(3);
    }// switch (ucTyp)
}// ucCI_CheckParameter


/******************************************************************************
* FUNCTION: ucCI_IsHex                                                        *
*-----------------------------------------------------------------------------*
* PURPOSE                                                                     *
*   Returns with the value of the character, if the character is              *
*   ASCII hex character.                                                      *
*-----------------------------------------------------------------------------*
* PARAMETER                                                                   *
*   uint8_t ucChar: The character                                             *
* END                                                                         *
* RETURN:                                                                     *
*   uint8_t: 255 - Character is not hex character;                            *
*                  otherwise the value of the character                       *
******************************************************************************/
uint8_t ucCI_IsHex(uint8_t ucChar)
{
    if ( (ucChar >= 'A') && (ucChar <= 'F') )
    {
        return(ucChar - 'A' + 10);
    }
    else if ( (ucChar >= 'a') && (ucChar <= 'f') )
    {
        return(ucChar - 'a' + 10);
    }
    else if ( (ucChar >= '0') && (ucChar <= '9') )
    {
        return(ucChar - '0');
    }
    else
    {
        return(255);
    }// if
}// uint8_t ucCI_IsHex(uint8_t ucChar)


/******************************************************************************
* FUNCTION: ucCI_GetByteParameter                                             *
*-----------------------------------------------------------------------------*
* PURPOSE                                                                     *
*   Imports a value of the byte parameter from the command string.            *
*-----------------------------------------------------------------------------*
* PARAMETER                                                                   *
*   uint8_t ucParamNum:   The number of the parameter                         *
*   char* ucpCommand:  Address of the command string                          *
* END                                                                         *
* RETURN:                                                                     *
*   uint8_t: The value of the parameter in the command string.                *
******************************************************************************/
uint8_t ucCI_GetByteParameter(uint8_t ucParamNum, char* ucpCommand)
{
    uint8_t i;
    
    ucpCommand += 2;
    if (*ucpCommand == '=') ucpCommand++;
    for (i = 1; i < ucParamNum; i++)
    {
        while ((*ucpCommand != ',') && (*ucpCommand != '=') && (*ucpCommand != 0)) ucpCommand++;
        ucpCommand++;
    }//for (i = 1; i < ucParamNum; i++)
    return (ucHexStr2Char(ucpCommand));
}


uint8_t ucCI_GetParameter(char* ucpCommand, char answer[])
{
	uint8_t i, j=0;

	for(i=0;ucpCommand[i] != '\0' && ucpCommand[i] != ';'; i++)
	{
		if(ucpCommand[i] != ' ')
			answer[j++]=ucpCommand[i];
	}
	answer[j]='\0';
	return i+1;
}
