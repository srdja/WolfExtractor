/*

	Copyright (C) 2007 Michael Liebscher <johnnycanuck@users.sourceforge.net>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/**
 * \file console_win.c
 * \brief Interface to console window [UNIX].
 * \author Michael Liebscher
 * \date 2007
 * \note When doing a port these functions need to be converted.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "../console.h"
#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../string/wtstring.h"


/**
 * \brief Initialize hooks to console window
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean ConsoleWindow_Init( void )
{
	return true;
}

/**
 * \brief Remove hooks to console window
 * \return Nothing.
 */
PUBLIC void ConsoleWindow_Shutdown( void )
{
	;
}

/**
 * \brief Wait for user input on console window
 * \return Nothing.
 */
PUBLIC void CWaitForConsoleKeyInput( void )
{
	;
}

/**
 * \brief Print formatted output to the console output stream.
 * \param[in] textColour Text colour [ TC_RED, TC_GREEN, TC_BLUE, TC_WHITE, TC_CYAN, TC_YELLOW, TC_MAGENTA ]
 * \param[in] msg Format control.
 * \param[in] ... Optional arguments.
 * \return Nothing.
 */
PUBLIC void CPrintMsg( W32 textColour, const char *msg, ... )
{
	va_list	pArg;
	char	textbuffer[ 0x8000 ];

	va_start( pArg, msg );
	(void)vsnprintf( textbuffer, sizeof( textbuffer ), msg, pArg );
	va_end( pArg );

	textbuffer[ sizeof( textbuffer ) - 1 ] = '\0';


	printf( "%s", textbuffer );

}

PRIVATE char progressText[ 4 ] = { '|', '/', '-', '\\' };
PRIVATE W32 index = 0;

/**
 * \brief Print progress spinner to console output stream.
 * \return Nothing.
 */
PUBLIC void CProgressBar( void )
{
	;
}
