/*

	Copyright (C) 2004-2006 Michael Liebscher <johnnycanuck@users.sourceforge.net>
	Copyright (C) 1998 Todd C. Miller <Todd.Miller@courtesan.com>

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
 * \file wtstring.c
 * \brief String functions done in a portable manner.
 * \author Michael Liebscher
 * \author Todd C. Miller  
 * \date 2004-2006
 * \note Portion of this code was derived from code that was originally written by Todd C. Miller.
 */

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "wtstring.h"

#include "../common/common_utils.h"
#include "../memory/memory.h"


/**
 * \brief Copies a specified number of characters from a source string into a buffer.
 * \param[in,out] dest Pointer to a buffer into which the function copies characters.
 * \param[in] source Pointer to a NUL-terminated string from which the function copies characters. 
 * \param[in] nMaxLength Specifies the number of bytes to be copied from the string pointed to by source into the buffer pointed to by dest.
 * \return Returns the string length of \c source. If \c retval >= \c nMaxLength, truncation occurred.
 * \note At most \c nMaxLength-1 characters will be copied. Always NUL-terminates (unless \c nMaxLength == 0). 
 */
PUBLIC size_t wt_strlcpy( char *dest, const char *source, size_t nMaxLength )
{
	char *d = dest;
	const char *s = source;
	size_t n = nMaxLength;

	// Copy as many bytes as will fit 
	if( n != 0 && --n != 0 )
	{
		do
		{
			if( (*d++ = *s++) == 0 )
			{
				break;
			}

		} while( --n != 0 );
	}

	// Not enough room in dest, add NUL and traverse rest of source 
	if( n == 0 ) 
	{
		if( nMaxLength != 0 )
		{
			*d = '\0';		// NUL-terminate dest 
		}

		while( *s++ )
		{
			;
		}
	}

	return( s - source - 1 );	// count does not include NUL 
}

/**
 * \brief Appends one string to another. 
 * \param[in,out] dest Pointer to a NUL-terminated string. The buffer must be large enough to contain both strings or else truncation will occur.
 * \param[in] source Pointer to a NUL-terminated string from which the function copies characters. 
 * \param[in] nMaxLength Full size of dest, not space left.
 * \return Returns strlen( source ) + MIN( nMaxLength, strlen( initial dest ) ). If retval >= nMaxLength, truncation occurred.
 * \note At most \c nMaxLength-1 characters will be copied. Always NUL-terminates (unless \c nMaxLength == 0). 
 */
PUBLIC size_t wt_strlcat( char *dest, const char *source, size_t nMaxLength )
{
	char *d = dest;
	const char *s = source;
	size_t n = nMaxLength;
	size_t dlen;

	// Find the end of dest and adjust bytes left but don't go past end 
	while( n-- != 0 && *d != '\0' )
	{
		d++;
	}

	dlen = d - dest;
	n = nMaxLength - dlen;

	// No room left to append string 
	if( n == 0 )
	{
		return( dlen + strlen( s ) );
	}

	while( *s != '\0' )
	{
		if( n != 1 ) 
		{
			*d++ = *s;
			n--;
		}
		s++;
	}
	*d = '\0';	// NUL-terminate string 

	return( dlen + (s - source) );	// count does not include NUL 
	
}

/**
 * \brief Compare characters of two strings without regard to case.
 * \param[in] string1 NUL-terminated strings to compare. 
 * \param[in] string2 NUL-terminated strings to compare. 
 * \param[in] count Number of characters to compare.
 * \return The return value indicates the relationship between the substrings as follows.
 * \arg \c <0  \c string1 substring less than \c string2 substring 
 * \arg \c 0  \c string1 substring identical to \c string2 substring  
 * \arg \c >0 \c string1 substring greater than \c string2 substring 
 */
PUBLIC SW32 wt_strnicmp( const char *string1, const char *string2, size_t count )
{
	SW32 diff;
	const char *c1;
	const char *c2;
	
	for( c1 = string1, c2 = string2 ; ; c1++, c2++ )
	{
		if( c1 == string1 + count )
		{
			return 0;	// Match up to count characters 
		}

	    if( ! (*c1 && *c2) )
		{
			return *c1 - *c2;
		}

	    diff = TOLOWER( *c1 ) - TOLOWER( *c2 );
	    
		if( diff )
		{
			return diff;
		}
	}
	
    // should never get here
	return 1;		
}

/**
 * \brief Perform a lowercase comparison of strings.
 * \param[in] string1 NUL-terminated strings to compare. 
 * \param[in] string2 NUL-terminated strings to compare. 
 * \return The return value indicates the relation of \c string1 to \c string2 as follows.
 * \arg \c <0  \c string1 less than \c string2 
 * \arg \c 0  \c string1 identical to \c string2 
 * \arg \c >0 \c string1 greater than \c string2 
 */
PUBLIC SW32 wt_stricmp( const char *string1, const char *string2 )
{
	SW32 diff;

    for(  ; *string1 && *string2 ; string1++, string2++ ) 
	{
		if( (diff = TOLOWER( *string1 ) - TOLOWER( *string2 )) )
		{
			return diff;
		}
    }

    if( *string1 )
	{
		return 1;	// string1 is longer than string2 
	}

    if( *string2 )
	{
		return -1;	// string1 is shorter than string2
	}
    	
	return 0;	// identical 
}

/**
 * \brief Write formatted data to a string.
 * \param[out] dest Storage location for output.
 * \param[in] size Maximum number of characters to store.
 * \param[in] format Format-control string.
 * \param[in]  ... Optional arguments.
 * \note If the format string is longer than 32768 truncation will occur. Also, if \c format is longer than \c dest truncation will occur.
 */
PUBLIC void wt_snprintf( char *dest, size_t size, const char *format, ... )
{
	va_list	argptr;
	char	bigbuffer[ 0x8000 ];

	va_start( argptr, format );
	(void)vsnprintf( bigbuffer, sizeof( bigbuffer ), format, argptr );
	va_end( argptr );
	
	bigbuffer[ sizeof( bigbuffer ) - 1 ] = '\0';
	
	wt_strlcpy( dest, bigbuffer, size );
}

/**
 * \brief Convert a string to uppercase.
 * \param[in,out] string NUL-terminated string to capitalize. 
 * \return This functions returns a pointer to the converted string. Because the modification is done in place, 
 *  the pointer returned is the same as the pointer passed as the input argument. No return value is reserved to indicate an error.
 */
PUBLIC char *wt_strupr( char *string )
{
	char *ptr;

	if( ! string || ! *string )
	{
		return string;
	}

	ptr = string;

	do
	{
		*ptr = TOUPPER( *ptr );		

	} while( *ptr++ );


	return string;
}

/**
 * \brief Convert a string to lowercase. 
 * \param[in,out] string NUL-terminated string to convert to lowercase.
 * \return This functions returns a pointer to the converted string. Because the modification is done in place, 
 *  the pointer returned is the same as the pointer passed as the input argument. No return value is reserved to indicate an error.
 */
PUBLIC char *wt_strlwr( char *string )
{
	char *ptr;

	if( ! string || ! *string )
	{
		return string; 
	}

	ptr = string;

	do
	{
		*ptr = TOLOWER( *ptr );		

	} while( *ptr++ );


	return string;
}

/**
 * \brief Create another copy of string. 
 * \param[in] source NUL-terminated string to copy.
 * \return Pointer to copy, otherwise NULL.
 * \note Caller is responsible for freeing allocated data by calling MM_FREE
 */
PUBLIC char *wt_strCopy( const char *source )
{
	char *dest;
	W32 length;

	length = strlen( source) + 1; 
	dest = (char *) MM_MALLOC( length );
	if( dest == NULL )
	{
		return NULL;
	}

	wt_strlcpy( dest, source, length );

	return dest;
}
