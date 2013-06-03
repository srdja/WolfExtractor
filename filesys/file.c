/*

	Copyright (C) 2005-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file file.c
 * \brief File I/O layer
 * \author Michael Liebscher 
 * \date 2005-2013
 */

#include <stdio.h>

#include "file.h"

#include "../memory/memory.h"


/////////////////////////////////////////////////////////////////////
//
//	File I/O functions
//
/////////////////////////////////////////////////////////////////////


/**
 * \brief Calculate file length.
 * \param[in] filestream Pointer to valid FILE structure.
 * \return Length of the file in bytes on success, otherwise -1.
 */
PUBLIC SW32 FS_FileLength( FILE *stream )
{
	SW32	cur;	/* Current position of file pointer */
	SW32	end;

	cur = ftell( stream );
	if( cur == -1 )
	{
		return -1;
	}
	fseek( stream, 0, SEEK_END );

	end = ftell( stream );
	fseek( stream, cur, SEEK_SET );

	return end;
}

/**
 * \brief Reads data from binary file stream.
 * \param[in,out] ptr Storage location.
 * \param[in] size Size of object in bytes.
 * \param[in] nmemb Number of objects to read.
 * \param[in] stream Stream to read from.
 * \return Length of the file in bytes on success, otherwise -1.
 */
PUBLIC SW32 FS_FileRead( void *ptr, size_t size, size_t nmemb, FILE *stream )
{
	return fread( ptr, size, nmemb, stream );
}

/**
 * \brief Open a file stream.
 * \param[in] filename Name of file to open.
 * \param[out] file  FILE pointer. 
 * \return On success length of the file in bytes, otherwise -1.
 */
PUBLIC SW32 FS_FileOpen( const char *filename, FILE **file )
{
	*file = fopen( filename, "rb" );
	if( ! *file )
	{
		*file = NULL;

		return -1;
	}

	return FS_FileLength( *file );

}

/**
 * \brief Load a file into memory.
 * \param[in] path File name to load.
 * \param[in,out] buffer  Buffer to hold data
 * \return On success file length in bytes, otherwise -1. Returned data is NUL-terminated.
 * \note Caller is responsible for freeing buffered data. Data is also NUL-terminated.
 */
PUBLIC SW32 FS_FileLoad( const char *path, void **buffer )
{
	FILE	*fhandle;
	W8	*buf;
	SW32	length;

	buf = NULL;


	length = FS_FileOpen( path, &fhandle );
	if( ! fhandle )
	{
		if( buffer )
		{
			*buffer = NULL;
		}

		return -1;
	}

	if( ! buffer )
	{
		fclose( fhandle );
		return length;
	}

	buf = (PW8)MM_MALLOC( length + 1 );
	*buffer = buf;

	
	if( FS_FileRead( buf, 1, length, fhandle ) < 0 )
	{
		MM_FREE( buf );

		fclose( fhandle );

		return -1;
	}

	buf[ length ] = '\0';


	fclose( fhandle );

	return length;
}

