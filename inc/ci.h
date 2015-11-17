#ifndef __SLAVE_CI_H__
#define __SLAVE_CI_H__


//*****************************************************************************
// Dependencies
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x.h"
#include "conv.h"
#include "motor.h"
#include "global.h"
#include "measurement_ctrl.h"

//*****************************************************************************
// END: Dependencies
//*****************************************************************************


//*****************************************************************************
// Public Defines + Public Variables
//*****************************************************************************


// Command codes

#define d_SP        0x01
#define d_DC        0x02
#define d_RP        0x03
#define d_GS        0x04
#define d_GD        0x05
#define d_HB        0x06
#define d_FM        0x07



// Defined error strings
#define d_E0                        "E0="
#define d_SyntaxError               "E1=Syntax Error"
#define d_InvalidParameter          "E2=Invalid parameter"
#define d_UnknownASCIICommand       "E3=Unknown command"

// Parameter types
#define d_PTYP_X            1       // Hexadecimal parameter format
#define d_PTYP_10           2       // Binary parameter format
#define d_PTYP_ab           3       // Parameter is "A" or "B"
#define d_PTYP_dbl			4		// Double parameter format
#define d_PTYP_LAST         0x80    // Last parameter


// Typedefs
typedef struct
{
    const char* cpString;
    uint8_t ucCode;
    const char* ucHelp;
}stCommItem;


//*****************************************************************************
// END: Public Defines + Public Variables
//*****************************************************************************


//*****************************************************************************
// Public function prototypes
//*****************************************************************************


/******************************************************************************
* FUNCTION:  uiCI_CommandInterpreter                                          *
*-----------------------------------------------------------------------------*
* PURPOSE                                                                     *
*   The main CI command interpreter                                           *
*-----------------------------------------------------------------------------*
* PARAMETER                                                                   *
*    char* ucpCommand:          Address of the Master command string          *
*    uint8_t  ucLength: 	Length of the master message                  *
*    char* ucpAnswer: 	        Address of the Slave buffer	              *
* END                                                                         *
* RETURN:                                                                     *
*    uint8_t:                   Size of the contents of the slave buffer      *
******************************************************************************/
uint8_t ucCI_CommandInterpreter(char* ucpCommand,
                                uint8_t  ucLength,
                                char* ucpAnswer);


//*****************************************************************************
// END: Public function prototypes
//*****************************************************************************


#endif // __SLAVE_CI_H__

