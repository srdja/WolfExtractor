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
 * \file zipfile.c
 * \brief Write zip file.
 * \author  Michael Liebscher
 * \date 2004-2005
 * \note Reference document: http://www.pkware.com/company/standards/appnote/appnote.txt
 */


#include <stdio.h>

#include "../common/platform.h"
#include "../common/common_utils.h"
#include "zip.h"

/* Signatures for zip file information headers */
#define SIG_LOCAL			0x04034b50L
#define SIG_CENTRAL			0x02014b50L
#define SIG_END				0x06054b50L
#define SIG_EXTENDLOCAL		0x08074b50L
#define SIG_EXTENDSPLOCAL	0x30304b50L

/* Length of header (not counting the signature) */
#define LOCALHEAD_SIZE		26
#define CENTRALHEAD_SIZE	42
#define ENDHEAD_SIZE		18


/**
 * \brief Write local header to file.
 * \param[in] z zip entry to write local header for.
 * \param[in] f File to write to.
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean zip_WriteLocalChunk( zipHead_t *z, FILE *f )
{
	W32 temp32;
	W16 temp16;
	W32 retval;

	temp32 = LittleLong( SIG_LOCAL );
	retval = fwrite( &temp32, 1, 4, f );

	temp16 = LittleShort( z->versionneeded );
	retval += fwrite( &temp16, 1, 2, f );

	temp16 = LittleShort( z->localflag );
	retval += fwrite( &temp16, 1, 2, f );

	temp16 = LittleShort( z->compression_method );
	retval += fwrite( &temp16, 1, 2, f );

	temp32 = LittleLong( z->timedate );
	retval += fwrite( &temp32, 1, 4, f );

	temp32 = LittleLong( z->crc32 );
	retval += fwrite( &temp32, 1, 4, f );

	temp32 = LittleLong( z->compressed_size );
	retval += fwrite( &temp32, 1, 4, f );

	temp32 = LittleLong( z->uncompressed_size );
	retval += fwrite( &temp32, 1, 4, f );

	temp16 = LittleShort( z->filename_length );
	retval += fwrite( &temp16, 1, 2, f );

	temp16 = LittleShort( z->extrafield_length );
	retval += fwrite( &temp16, 1, 2, f );

	if( retval != LOCALHEAD_SIZE + 4 )
	{
		return false;
	}

	if( fwrite( z->filename, 1, z->filename_length, f ) != z->filename_length ||
		(z->extrafield && ( fwrite( z->extrafield, 1, z->extrafield_length, f) != z->extrafield_length) ) )
	{
		return false;
	}

	return true;
}

/**
 * \brief Write central header to file.
 * \param[in] z zip entry to write central header for.
 * \param[in] f File to write to.
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean zip_WriteCentralChunk( zipHead_t *z, FILE *f )
{
	W32 temp32;
	W16 temp16;
	W32 retval;

	temp32 = LittleLong( SIG_CENTRAL );
	retval = fwrite( &temp32, 1, 4, f );

	temp16 = LittleShort( z->versionmadeby );
	retval += fwrite( &temp16, 1, 2, f );

	temp16 = LittleShort( z->versionneeded );
	retval += fwrite( &temp16, 1, 2, f );

	temp16 = LittleShort( z->flag );
	retval += fwrite( &temp16, 1, 2, f );

	temp16 = LittleShort( z->compression_method );
	retval += fwrite( &temp16, 1, 2, f );

	temp32 = LittleLong( z->timedate );
	retval += fwrite( &temp32, 1, 4, f );

	temp32 = LittleLong( z->crc32 );
	retval += fwrite( &temp32, 1, 4, f );

	temp32 = LittleLong( z->compressed_size );
	retval += fwrite( &temp32, 1, 4, f );

	temp32 = LittleLong( z->uncompressed_size );
	retval += fwrite( &temp32, 1, 4, f );

	temp16 = LittleShort( z->filename_length );
	retval += fwrite( &temp16, 1, 2, f );

	temp16 = LittleShort( z->centralextra_length );
	retval += fwrite( &temp16, 1, 2, f );

	temp16 = LittleShort( z->comment_length );
	retval += fwrite( &temp16, 1, 2, f );

	temp16 = LittleShort( z->disknumstart );
	retval += fwrite( &temp16, 1, 2, f );

	temp16 = LittleShort( z->internalattribute );
	retval += fwrite( &temp16, 1, 2, f );

	temp32 = LittleLong( z->externalattribute );
	retval += fwrite( &temp32, 1, 4, f );

	temp32 = LittleLong( z->offset );
	retval += fwrite( &temp32, 1, 4, f );

	if( retval != CENTRALHEAD_SIZE + 4 )
	{
		return false;
	}

	if( fwrite(z->filename, 1, z->filename_length, f) != z->filename_length ||
		(z->centralextra_length && fwrite(z->centralextra, 1, z->centralextra_length, f) != z->centralextra_length) ||
		(z->comment_length && fwrite(z->comment, 1, z->comment_length, f) != z->comment_length) )
	{
		return false;
	}

	return true;
}

/**
 * \brief Write end of central directory data to file.
 * \param[in] num Number of entries in central directory.
 * \param[in] size Size of central directory in bytes.
 * \param[in] offset Offset of central directory.
 * \param[in] len Length of zip file comment in bytes (0 if none).
 * \param[in] comment Zip file comment if len != 0.
 * \param[in] f File to write to.
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean zip_WriteEndChunk( W16 num, W32 size, W32 offset, W16 len, char *comment, FILE *f )
{
	W32 temp32;
	W16 temp16;
	W32 retval;

	temp32 = LittleLong( SIG_END );
	retval = fwrite( &temp32, 1, 4, f );

	temp16 = 0;
	retval += fwrite( &temp16, 1, 2, f );

	temp16 = 0;
	retval += fwrite( &temp16, 1, 2, f );

	temp16 = LittleShort( num );
	retval += fwrite( &temp16, 1, 2, f );

	temp16 = LittleShort( num );
	retval += fwrite( &temp16, 1, 2, f );

	temp32 = LittleLong( size );
	retval += fwrite( &temp32, 1, 4, f );

	temp32 = LittleLong( offset );
	retval += fwrite( &temp32, 1, 4, f );

	temp16 = LittleShort( len );
	retval += fwrite( &temp16, 1, 2, f );

	if( retval != ENDHEAD_SIZE + 4 )
	{
		return false;
	}


	if( len && fwrite( comment, 1, len, f ) != len )
	{
		return false;
	}

	return true;
}
