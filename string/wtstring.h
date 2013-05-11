/*

	Copyright (C) 2004-2006 Michael Liebscher <johnnycanuck@users.sourceforge.net>
	Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>

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
 * \file wtstring.h
 * \brief Common string functions done in a portable manner.
 * \author Michael Liebscher 
 * \author Todd C. Miller
 * \date 2004-2006 
 * \note Portion of this code was derived from code that was originally written by Todd C. Miller.
 */


#ifndef __WTSTRING_H__
#define __WTSTRING_H__

#include <stdlib.h>

#include "../common/platform.h"


#define ISSPACE( c )	( ( c ) == ' ' || ( c ) == '\f' || ( c ) == '\n' || ( c ) == '\r' || ( c ) == '\t' || ( c ) == '\v' )
#define ISUPPER( c )	( ( c ) >= 'A' && ( c ) <= 'Z' )
#define ISLOWER( c )	( ( c ) >= 'a' && ( c ) <= 'z' )
#define ISALPHA( c )	( ISUPPER( c ) || ISLOWER( c ) )
#define TOUPPER( c )	( ISLOWER( c ) ? (c) - 'a' + 'A' : ( c ) )
#define TOLOWER( c )	( ISUPPER( c ) ? (c) - 'A' + 'a' : ( c ) )

#define ISNUMERIC( c )	( ( c ) >= '0' && ( c ) <= '9' )
#define ISALPHANUMERIC( c )	( ISALPHA( c ) || ISNUMERIC( c ) )




size_t wt_strlcpy( char *dest, const char *source, size_t nMaxLength );
size_t wt_strlcat( char *dest, const char *source, size_t nMaxLength );


SW32 wt_stricmp( const char *string1, const char *string2 );
SW32 wt_strnicmp( const char *string1, const char *string2, size_t count );


void wt_snprintf( char *dest, size_t size, const char *format, ... );

char *wt_strCopy( const char *source );

W32 wt_strhash( const char *string, W32 length, W32 mask );

char *wt_strupr( char *string );
char *wt_strlwr( char *string );


/* String conversion error */
#define	SCE_NON_NUMERIC		(1 << 0)	// Non-numeric value was encountered 
#define	SCE_BUFFER_OVERFLOW	(1 << 1)	// Numberic overflowed 
#define	SCE_NULL_VALUE		(1 << 2)	// NULL string was passed into function 


SW32 StringToInteger( const char *string, W32 *error );
double StringToFloat( const char *string, W32 *error );


#endif /* __WTSTRING_H__ */
