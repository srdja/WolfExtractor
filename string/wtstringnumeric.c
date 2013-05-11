/*

	Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file wtstringnumeric.c
 * \brief String conversions done in a portable manner.
 * \author  Michael Liebscher
 * \date 2004-2005
 */


#include <math.h>

#include "wtstring.h"


#include "../common/common_utils.h"


/**
 * \brief Generates a hash value from string. 
 * \param[in] string NUL-terminated string to hash.
 * \return Returns hash value
 */
PUBLIC W32 wt_strhash( const char *string, W32 length, W32 mask )
{

	W32 hash = 0;
    W32 i;

	for ( i = 0; i < length; i++ ) 
	{
		hash += string[ i ];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);


    return hash & mask;

}


/**
 * \brief Convert string to integer.
 * \param[in] string The String to be converted to an Integer.
 * \param[out] error Error code. See header.	
 * \return An integer value.
 */
PUBLIC SW32 StringToInteger( const char *string, W32 *error )
{
	const char *ptr = string;
	SW32 temp;
	SW32 number = 0;
	W32 errortag = 0;
	wtBoolean isNegative = false;

	if( ! string || ! *string )
	{	
		*error = SCE_NULL_VALUE;
		
		return 0;
	}

	if( *ptr == '-' )
	{
		isNegative = true;

		ptr++;
	}	

	while( *ptr && ISNUMERIC( *ptr ) )
	{
		temp = number;
		number = (number * 10) + *ptr - '0';

		if( number < temp )
		{
			errortag &= SCE_BUFFER_OVERFLOW;
		}

		ptr++;
	}

	if( *ptr )
	{
		errortag &= SCE_NON_NUMERIC;
	}


	if( isNegative )
	{
		number = -number;
	}
	

	*error = errortag;

	return number;
}

/**
 * \brief Convert string to float.
 * \param[in] string The String to be converted to a float.
 * \param[out] error Error code. See header.	
 * \return A float value.
 */
PUBLIC double StringToFloat( const char *string, W32 *error )
{
	const char *ptr = string;
	double number = 0;
	SW32 exponent = 0;
	W32 expError = 0;
	wtBoolean isNegative = false;

	*error = 0;

	if( ! string || ! *string )
	{	
		*error &= SCE_NULL_VALUE;
		
		return 0;
	}

	if( *ptr == '-' )
	{
		isNegative = true;

		ptr++;
	}
	else if( *ptr == '+' )
	{
		ptr++;
	}

	while( *ptr && ISNUMERIC( *ptr ) )
	{
		number = (number * 10) + (double)(*ptr - '0');

		ptr++;
	}

	if( *ptr == '.' )
	{
		ptr++;

		while( *ptr && ISNUMERIC( *ptr ) )
		{
			number = (number * 10) + (double)(*ptr - '0');
			exponent--;

			ptr++;
		}
	}

	if( TOLOWER( *ptr ) == 'e' )
	{		
		ptr++;

		exponent += StringToInteger( ptr, &expError );
	}

	if( isNegative )
	{
		number = -number;
	}

	if( expError )
	{
		*error |= expError;
	}

	return ( number * pow( 10, exponent ) );
}
