/*

	Copyright (C) 2006-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \brief Interface to console window [Windows].
 * \author Michael Liebscher
 * \date 2006-2013
 * \note When doing a port these functions need to be converted.
 */



#include <windows.h>

#include <stdio.h>
#include <stdarg.h>


#include "../console.h"
#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../string/wtstring.h"



PRIVATE HANDLE hConsoleScrnBuffer = INVALID_HANDLE_VALUE;
PRIVATE HANDLE hStdin = INVALID_HANDLE_VALUE;



PRIVATE char oldConsoleTitle[ MAX_PATH ];
PRIVATE DWORD nOldTitleSize;

WORD wBGColourAttribute = 0;

DWORD saveOldMode;


extern const char *APPLICATION_STRING;
extern const char *VERSION_STRING;


/**
 * \brief Initialize hooks to console window
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean ConsoleWindow_Init( void )
{
	CONSOLE_SCREEN_BUFFER_INFO csbInfo;
	char szNewTitleName[ 256 ];

	/* Backup current console title name. */
	nOldTitleSize = GetConsoleTitle( oldConsoleTitle, sizeof( oldConsoleTitle ) );


	wt_snprintf( szNewTitleName, sizeof( szNewTitleName ), "%s %s", APPLICATION_STRING, VERSION_STRING );
	(void)SetConsoleTitle( szNewTitleName );


	hConsoleScrnBuffer = GetStdHandle( STD_OUTPUT_HANDLE );
	if( hConsoleScrnBuffer == INVALID_HANDLE_VALUE )
	{
        goto BufferFailed;
	}

	if( ! GetConsoleScreenBufferInfo( hConsoleScrnBuffer, &csbInfo ) )
	{
		goto BufferFailed;
	}



	hStdin = GetStdHandle( STD_INPUT_HANDLE );
	if( hStdin == INVALID_HANDLE_VALUE )
	{
        goto BufferFailed;
	}

	if( ! GetConsoleMode( hStdin, &saveOldMode ) )
	{
        goto BufferFailed;
	}

    if( ! SetConsoleMode( hStdin, ENABLE_WINDOW_INPUT ) )
	{
         goto BufferFailed;
	}


	return true;



BufferFailed:
	ConsoleWindow_Shutdown();

	return false;

}

/**
 * \brief Remove hooks to console window
 * \return Nothing.
 */
PUBLIC void ConsoleWindow_Shutdown( void )
{

	if( nOldTitleSize ) /* Restore console title name? */
	{
		SetConsoleTitle( oldConsoleTitle );
	}

	if( hConsoleScrnBuffer != INVALID_HANDLE_VALUE ) /* Destroy handle? */
	{
		CloseHandle( hConsoleScrnBuffer );
		hConsoleScrnBuffer = INVALID_HANDLE_VALUE;
	}

	if( hStdin != INVALID_HANDLE_VALUE ) /* Destroy handle? */
	{
		SetConsoleMode( hStdin, saveOldMode );
		CloseHandle( hStdin );
		hStdin = INVALID_HANDLE_VALUE;
	}


}

/**
 * \brief Wait for user input on console window
 * \return Nothing.
 */
PUBLIC void CWaitForConsoleKeyInput( void )
{
	INPUT_RECORD ConInBuf[ 12 ];
	DWORD cNumRead;
	DWORD i;

	printf( "\nPress a key to exit.\n" );

	for( ; ; )
	{
		if( ! ReadConsoleInput( hStdin, ConInBuf, sizeof( ConInBuf ), &cNumRead ) )
		{
			return;
		}

		for( i = 0 ; i < cNumRead ; ++i )
		{
			if( i < 12 && ConInBuf[ i ].EventType == KEY_EVENT )
			{
				return;
			}
		}

	}

	return;
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

	WORD wColourAttribute = 0;	/* default is TC_BLACK */
	DWORD nLength;
	DWORD nWritten;



	va_start( pArg, msg );
	(void)vsnprintf( textbuffer, sizeof( textbuffer ), msg, pArg );
	va_end( pArg );

	textbuffer[ sizeof( textbuffer ) - 1 ] = '\0';


	if( hConsoleScrnBuffer == INVALID_HANDLE_VALUE )
	{
		printf( "%s", textbuffer );
	}


	nLength = strlen( textbuffer );

	switch( textColour )
	{
		case TC_RED:
			wColourAttribute = FOREGROUND_RED | FOREGROUND_INTENSITY;
			break;

		case TC_GREEN:
			wColourAttribute = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			break;

		case TC_BLUE:
			wColourAttribute = FOREGROUND_BLUE;
			break;

		case TC_WHITE:
			wColourAttribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;

		case TC_CYAN:
			wColourAttribute = FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;

		case TC_YELLOW:
			wColourAttribute = FOREGROUND_RED | FOREGROUND_GREEN;
			break;

		case TC_MAGENTA:
			wColourAttribute = FOREGROUND_RED | FOREGROUND_BLUE;
			break;
	}


	SetConsoleTextAttribute( hConsoleScrnBuffer, (WORD)(wColourAttribute | wBGColourAttribute) );

	WriteConsole( hConsoleScrnBuffer, textbuffer, nLength, &nWritten, NULL );


	SetConsoleTextAttribute( hConsoleScrnBuffer, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE  );

}

PRIVATE const char progressText[ 4 ] = { '|', '/', '-', '\\' };
PRIVATE W32 index = 0;

/**
 * \brief Print progress spinner to console output stream.
 * \return Nothing.
 */
PUBLIC void CProgressBar( void )
{
	DWORD nWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbInfo;

	if( hConsoleScrnBuffer == INVALID_HANDLE_VALUE )
	{
		return;
	}

	SetConsoleTextAttribute( hConsoleScrnBuffer, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );

	WriteConsole( hConsoleScrnBuffer, &progressText[ index % 4 ], 1, &nWritten, NULL );

	GetConsoleScreenBufferInfo( hConsoleScrnBuffer, &csbInfo );

	csbInfo.dwCursorPosition.X--;

	SetConsoleCursorPosition( hConsoleScrnBuffer, csbInfo.dwCursorPosition );

	++index;
}

