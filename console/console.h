/*

	Copyright (C) 2006 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file win_console.h
 * \brief Interface to console.
 * \author Michael Liebscher 
 * \date 2006
 */


#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "../common/platform.h"


enum
{
	TC_BLACK,
	TC_RED,
	TC_GREEN,
	TC_BLUE,	
	TC_WHITE,
	TC_CYAN,
	TC_YELLOW,
	TC_MAGENTA,

	TC_COUNT	/* Must be last */
};


wtBoolean ConsoleWindow_Init( void );
void ConsoleWindow_Shutdown( void );
void CWaitForConsoleKeyInput( void );
void CPrintMsg( W32 textColour, const char *msg, ... );

void CProgressBar( void );

#endif /* __CONSOLE_H__ */
