/*

	Copyright (C) 2004-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file mac.c
 * \brief This module is used to extract data from the Macintosh version of Wolfenstein 3-D.
 * \author Michael Liebscher
 * \date 2004-2013
 * \note Portion of this code was derived from code that was originally written by Id Software, Inc., Bill Heineman and Chris DeSalvo.
 */

#include <stdio.h>
#include <string.h>

#include "mac.h"
#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../loaders/tga.h"
#include "../../memory/memory.h"
#include "../../string/wtstring.h"

#include "../../filesys/file.h"



PRIVATE W32 DATAFORKLENGTH = 105754L;
PRIVATE W32 RESFORKLENGTH  = 2424697L;

/* Resource file Palette offsets */
PRIVATE W32 MACPAL		= 898370L;
PRIVATE W32 IDPAL		= 899142L;
PRIVATE W32 GAMEPAL		= 946811L;
PRIVATE W32 INTERPAL	= 997211L;
PRIVATE W32 YUMMYPAL	= 1011345L;
PRIVATE W32 TITLEPAL	= 2235007L;

PRIVATE W32 PALETTE_SIZE = 768;


PRIVATE const char *MAC_FEXT = "*.REZ";

PRIVATE const char *MACBINFILENAME = "Wolfenstein 3D";
PRIVATE const char *FILETYPECREATOR	= "APPLWOLF";



PRIVATE W8 *macPalette;

PRIVATE FILE *fResHandle;



/**
 * \brief Get block of resource data.
 * \param[in] offset Number of bytes from start of file.
 * \param[in] length Length of resource block.
 * \param[in] glen Next four bytes after block.
 * \return NULL on error, otherwise pointer to block of memory.
 * \note Caller is responsible for freeing memory block.
 */
PRIVATE W8 *getResourceBlock( W32 offset, W32 length, W32 *glen )
{
	W8 *buf;

	fseek( fResHandle, offset, SEEK_SET );

	buf = (PW8)MM_MALLOC( length );
	if( buf == NULL )
	{
		printf( "Could not allocate memory block\n" );

		return NULL;
	}

	if( fread( buf, 1, length, fResHandle ) != length )
	{
		printf( "read error on resource file\n" );
		MM_FREE( buf );
		return NULL;
	}

	if( fread( glen, 1, 4, fResHandle ) != 4 )
	{
		printf( "read error on resource file\n" );
		MM_FREE( buf );
		return NULL;
	}

	*glen = BigLong( *glen );

	return buf;
}

/**
 * \brief Decodes a block of data that was encoded with LZSS
 * \param[in,out] dst Buffer to hold uncompressed data.
 * \param[in] src Compressed data block.
 * \param[in] Length Length of source data block in bytes.
 * \return Nothing.
 */
PRIVATE void Decode_LZSS( W8 *dst, const W8 *src, W32 length )
{
	W32 bitBucket;
	W32 runCount;
	W32 Fun;
	W8 *ptrBack;

	if( ! length )
	{
		return;
	}

	bitBucket = (W32) src[ 0 ] | 0x100;
	++src;

	do
	{
		if( bitBucket & 1 )
		{
			dst[ 0 ] = src[ 0 ];
			++src;
			++dst;
			--length;
		}
		else
		{
			runCount = (W32) src[ 0 ] | ((W32) src[ 1 ] << 8);
			Fun = 0x1000 - (runCount & 0xFFF);
			ptrBack = dst - Fun;
			runCount = ((runCount >> 12) & 0x0F) + 3;
			if( length >= runCount )
			{
				length -= runCount;
			}
			else
			{
				runCount = length;
				length = 0;
			}

			while( runCount-- )
			{
				*dst++ = *ptrBack++;
			}
			src += 2;
		}

		bitBucket >>= 1;
		if( bitBucket == 1 )
		{
			bitBucket = (W32) src[ 0 ] | 0x100;
			++src;
		}

	} while( length );
}

///////////////////////////////////////////////////////////////////////////////
//
//	Palette Routines
//
///////////////////////////////////////////////////////////////////////////////


/**
 * \brief Convert 256 palette data to RGB.
 * \param[in,out] dst Destination buffer to convert to.
 * \param[in] src 256 palette data.
 * \param[in] length Length of source data.
 * \param[in] palette Pointer to 256*3 array.
 * \return Nothing.
 */
PRIVATE void ConvertPaletteToRGB( W8 *dst, W8 *src, W32 length, W8 *palette )
{
	W32 i;

	for( i = 0 ; i < length ; ++i )
	{
		dst[ i * 3 ]		= palette[ src[ i ] * 3 ];
		dst[ i * 3 + 1 ]	= palette[ src[ i ] * 3 + 1 ];
		dst[ i * 3 + 2 ]	= palette[ src[ i ] * 3 + 2 ];
	}
}

/**
 * \brief Set global palette.
 * \param[in] offset Offset of palette in resource file.
 * \return Nothing.
 */
PRIVATE void setPalette( W32 offset )
{
	W32 junk;

	if( macPalette )
	{
		MM_FREE( macPalette );
	}

	macPalette = getResourceBlock( offset, PALETTE_SIZE, &junk );

}

///////////////////////////////////////////////////////////////////////////////
//
//	Screen Routines
//
///////////////////////////////////////////////////////////////////////////////


/**
 * \brief Decodes BJ map image and writes data to tga file.
 * \param[in] offset Offset in bytes the resource block.
 * \param[in] length Length of encoded data in bytes.
 * \return Nothing.
 */
PRIVATE void DecodeBJMapImage( W32 offset, W32 length )
{
	W8 *ptrResource;
	W8 *buffer;
	char filename[ 32 ];
	W32 junk;

	ptrResource = (PW8)getResourceBlock( offset, length, &junk );
	if( ! ptrResource )
	{
		return;
	}

	buffer = (PW8) MM_MALLOC( 16 * 16 * 3 );
	if( buffer == NULL )
	{
		printf( "Could not allocate memory block\n" );

		MM_FREE( ptrResource );

		return;
	}

	ConvertPaletteToRGB( buffer, ptrResource, 16 * 16, macPalette );

    wt_snprintf( filename, sizeof( filename ), "%s%cbjautomap.tga", DIRPATHPICS, PATH_SEP );

	TGA_write( filename, 24, 16, 16, buffer, 0, 1 );

	MM_FREE( buffer );

	MM_FREE( ptrResource );

}

/**
 * \brief Decodes BJ intermission images and writes data to tga file.
 * \param[in] offset Offset in bytes the resource block.
 * \param[in] length Length of encoded data in bytes.
 * \return Nothing.
 */
PRIVATE void DecodeBJIntermImages( W32 offset, W32 length )
{
	W32 *ptrResource;
	W32 uncomprLength;
	W8 *uncompr, *buffer;
	W32 indexs[3];
	W16 *ptr;
	W32 width, height, i;
	W32 junk;
	char filename[ 32 ];

	ptrResource = (PW32)getResourceBlock( offset, length, &junk );
	if( ! ptrResource )
	{
		return;
	}

	uncomprLength = BigLong( ptrResource[ 0 ] );

	uncompr = (PW8)MM_MALLOC( uncomprLength );
	if( uncompr == NULL )
	{
		printf( "Could not allocate memory block\n" );

		MM_FREE( ptrResource );

		return;
	}

	Decode_LZSS( uncompr, (PW8)&ptrResource[ 1 ], uncomprLength );

	MM_FREE( ptrResource );

	MM_MEMCPY( indexs, uncompr, 12 );

	indexs[ 0 ] = BigLong( indexs[ 0 ] );
	indexs[ 1 ] = BigLong( indexs[ 1 ] );
	indexs[ 2 ] = BigLong( indexs[ 2 ] );


	buffer = (PW8) MM_MALLOC( 256 * 256 * 3 );
	if( buffer == NULL )
	{
		printf( "Could not allocate memory block\n" );

		MM_FREE( uncompr );

		return;
	}

	for( i = 0 ; i < 3 ; ++i )
	{
		ptr = (PW16)&uncompr[ indexs[ i ]  ];
		width = BigShort( ptr[ 0 ] );
		height = BigShort( ptr[ 1 ] );

		ConvertPaletteToRGB( buffer, (PW8)&ptr[ 2 ], width * height, macPalette );

        wt_snprintf( filename, sizeof( filename ), "%s%cbj%d.tga", DIRPATHPICS, PATH_SEP, i );

		TGA_write( filename, 24, width, height, buffer, 0, 1 );
	}

	MM_FREE( buffer );

	MM_FREE( uncompr );
}

/**
 * \brief Decode screen from Macintosh resource fork.
 * \param[in] offset Offset in bytes the resource block.
 * \param[in] length Length of encoded data in bytes.
 * \param[in] filename Name of file to save as.
 * \return Nothing.
 */
PRIVATE void DecodeScreen( W32 offset, W32 length, const char *filename )
{
	W8 *ptrResource;
	W32 uncomprLength;
	W16 *uncompr;
	W16 width, height;
	W8 *buffer;
	W32 junk;

	ptrResource = getResourceBlock( offset, length, &junk );
	if( ! ptrResource )
	{
		return;
	}

	uncomprLength = BigLong( *((PW32)ptrResource) );
	uncompr = (PW16)MM_MALLOC( uncomprLength );
	if( uncompr == NULL )
	{
		printf( "Could not allocate memory block\n" );

		MM_FREE( ptrResource );

		return;
	}

	Decode_LZSS( (PW8)uncompr, ptrResource+4, uncomprLength );

	width = BigShort( uncompr[ 0 ] );
	height = BigShort( uncompr[ 1 ] );

	buffer = (W8 *)MM_MALLOC( width * height * 3 );
	if( buffer == NULL )
	{
		printf( "Could not allocate memory block\n" );

		MM_FREE( uncompr );
		MM_FREE( ptrResource );

		return;
	}

	ConvertPaletteToRGB( buffer, (PW8)&uncompr[ 2 ], width * height, macPalette );

	TGA_write( filename, 24, width, height, buffer, 0, 1 );

	MM_FREE( buffer );
    MM_FREE( uncompr );
	MM_FREE( ptrResource );
}


/**
 * \brief Decodes art data from Macintosh resource fork.
 */
PRIVATE void decodeScreens( void )
{
	char path[ 256 ];

	setPalette( MACPAL );
    wt_snprintf( path, sizeof( path ), "%s%cmacplaypic.tga", DIRPATHPICS, PATH_SEP );
	DecodeScreen( 899914, 46893, path );

	setPalette( IDPAL );
	wt_snprintf( path, sizeof( path ), "%s%cidpic.tga", DIRPATHPICS, PATH_SEP );
	DecodeScreen( 899914, 46893, path );

	setPalette( INTERPAL );
	wt_snprintf( path, sizeof( path ), "%s%cintermissionpic.tga", DIRPATHPICS, PATH_SEP );
	DecodeScreen( 947583, 49624, path );


	DecodeBJIntermImages( 1018931, 20766 );


	setPalette( GAMEPAL );
	wt_snprintf( path, sizeof( path ), "%s%cgetpsychedpic.tga", DIRPATHPICS, PATH_SEP );
	DecodeScreen( 997983, 3256, path );


	DecodeBJMapImage( 2391746, 1281 );



	setPalette( YUMMYPAL );
	wt_snprintf( path, sizeof( path ), "%s%cyummypic.tga", DIRPATHPICS, PATH_SEP );
	DecodeScreen( 1002299, 9042, path );

	setPalette( TITLEPAL );
	wt_snprintf( path, sizeof( path ), "%s%ctitlepic.tga", DIRPATHPICS, PATH_SEP );
	DecodeScreen( 2067971, 167032, path );


}


///////////////////////////////////////////////////////////////////////////////
//
//	Wall Routines
//
///////////////////////////////////////////////////////////////////////////////


/**
 * \brief Realign wall image.
 * \param[in] src Source data.
 * \param[in] width Width of wall image in pixels.
 * \param[in] height Height of wall image in pixels.
 * \return NULL on error, otherwise pointer to memory block with realigned wall image data.
 * \note Caller is responsible for freeing returned memory block.
 */
PUBLIC W8 *obverseWall( const W8 *src, W16 width, W16 height )
{
	W8 *target;
	W16 w, h;

	target = (W8 *)MM_MALLOC( width * height );
	if( target == NULL )
	{
		printf( "Could not allocate memory block\n" );

		return NULL;
	}

	for( h = 0; h < height; ++h )
	for( w = 0; w < width; ++w )
	{
		target[ h + width * w ] = src[ h * width + w ];
	}

	return target;
}

/**
 * \brief Decode then save wall data as TARGA image file.
 * \param[in] offset Offset from the start of the resource file in bytes.
 * \param[in] length Length of data in bytes.
 * \param[out] retval Length of next block in bytes.
 * \param[in] filename Name of file to save as.
 * \return Nothing.
 */
PRIVATE void DecodeWall( W32 offset, W32 length, W32 *retval, const char *filename )
{
	W8 *ptrResource;
	W8 *uncompr;
	W8 *buffer;
	W8 *newwall;

	ptrResource = getResourceBlock( offset, length, retval );
	if( ! ptrResource )
	{
		return;
	}

	uncompr = (PW8)MM_MALLOC( 128 * 128 );
	if( uncompr == NULL )
	{
		printf( "Could not allocate memory block\n" );

		MM_FREE( ptrResource );

		return;
	}

	Decode_LZSS( uncompr, ptrResource, 128 * 128 );

	newwall = obverseWall( uncompr, 128, 128 );

	buffer = (W8 *)MM_MALLOC( 128 * 128 * 3 );
	if( buffer == NULL )
	{
		printf( "Could not allocate memory block\n" );

		MM_FREE( ptrResource );
		MM_FREE( uncompr );
		MM_FREE( newwall );
		return;
	}


	ConvertPaletteToRGB( buffer, newwall, 128 * 128, macPalette );

	TGA_write( filename, 24, 128, 128, buffer, 0, 1 );

	MM_FREE( buffer );
	MM_FREE( newwall );
	MM_FREE( uncompr );
	MM_FREE( ptrResource );
}

/**
 * \brief Extract wall image data from resource fork.
 */
PRIVATE void decodeWalls( void )
{
	W32 i;
	W32 noffset = 702256;
	W32 length = 6277;
	W32 retval;
	char name[ 256 ];

	for( i = 0 ; i < 35 ; ++i )
	{
        wt_snprintf( name, sizeof( name ), "%s%c%.3d.tga", DIRPATHWALLS, PATH_SEP, i );
		DecodeWall( noffset, length, &retval,  name );

		noffset += length + 4;
		length = retval;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	Sprite Routines
//
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
	W16 Topy;
	W16 Boty;
	W16 Shape;

} spriteRun;

/**
 * \brief Decode then save sprite data as TARGA image file.
 * \param[in] offset Offset from the start of the resource file in bytes.
 * \param[in] length Length of data block in bytes.
 * \param[out] retval Length of next data block in bytes.
 * \param[in] filename Name of file to save as.
 * \return Nothing.
 */
PRIVATE void DecodeSprite( W32 offset, W32 length, W32 *retval, const char *filename )
{
	W8 *ptrResource;
	W32 uncomprLength;
	W8 *uncompr;
	W8 *buffer;
	W16 *ptr;
	SW32 i, x, width, noffset;

	ptrResource = getResourceBlock( offset, length, retval );
	if( ! ptrResource )
	{
		return;
	}

	uncomprLength = ptrResource[ 0 ];
	uncomprLength |= ptrResource[ 1 ] << 8;

	uncompr = (PW8)MM_MALLOC( uncomprLength );
	if( uncompr == NULL )
	{
		printf( "Could not allocate memory block\n" );

		MM_FREE( ptrResource );

		return;
	}

	Decode_LZSS( uncompr, ptrResource+2, uncomprLength );


	buffer = (W8 *)MM_MALLOC(128 * 128 * 4);
	if( buffer == NULL )
	{
		printf( "Could not allocate memory block\n" );

		MM_FREE( ptrResource );
		MM_FREE( uncompr );

		return;
	}

	memset( buffer, 0, 128 * 128 * 4 );


	ptr = (PW16)uncompr;

	width = BigShort( ptr[ 0 ] );

	noffset = 64 - width / 2;
	for( x = 0 ; x < width ; ++x )
	{
		spriteRun *p = (spriteRun *)&ptr[ BigShort( ptr[ x + 1 ] ) / 2 ];

		while( p->Topy != 0xFFFF )
		{
			for( i = BigShort( p->Topy ) / 2; i < BigShort( p->Boty ) / 2; ++i )
			{
				*(buffer + (i * 128 + x + noffset) * 4 + 0) = macPalette[ uncompr[ BigShort( p->Shape ) + BigShort( p->Topy ) / 2 + (i - BigShort( p->Topy ) / 2) ] * 3 + 0 ];
				*(buffer + (i * 128 + x + noffset) * 4 + 1) = macPalette[ uncompr[ BigShort( p->Shape ) + BigShort( p->Topy ) / 2 + (i - BigShort( p->Topy ) / 2) ] * 3 + 1 ];
				*(buffer + (i * 128 + x + noffset) * 4 + 2) = macPalette[ uncompr[ BigShort( p->Shape ) + BigShort( p->Topy ) / 2 + (i - BigShort( p->Topy ) / 2) ] * 3 + 2 ];
				*(buffer + (i * 128 + x + noffset) * 4 + 3) = 255;
			}
			p++;
		}
	}


	TGA_write( filename, 32, 128, 128, buffer, 0, 1 );

	MM_FREE( buffer );
    MM_FREE( uncompr );
	MM_FREE( ptrResource );
}


/**
 * \brief Extract sprites from resource fork.
 */
PRIVATE void decodeSprites( void )
{
	W32 i;
	W32 offset = 106345;
	W32 length = 524;
	W32 retval;
	char name[ 256 ];

	for( i = 0 ; i < 163 ; ++i )
	{
        wt_snprintf( name, sizeof( name ), "%s%c%.3d.tga", DIRPATHSPRITES, PATH_SEP, i );
		DecodeSprite( offset, length, &retval, name );
		offset += length + 4;
		length = retval;
	}


	offset = 579104;
	length = 868;
	for( i = 163 ; i < 171 ; ++i )
	{
        wt_snprintf( name, sizeof( name ), "%s%c%.3d.tga", DIRPATHSPRITES, PATH_SEP, i );
		DecodeSprite( offset, length, &retval, name );
		offset += length + 4;
		length = retval;
	}

	offset = 2356530;
	length = 7153;
	for( i = 171 ; i < 175 ; ++i )
	{
        wt_snprintf( name, sizeof( name ), "%s%c%.3d.tga", DIRPATHSPRITES, PATH_SEP, i );
		DecodeSprite( offset, length, &retval, name );
		offset += length + 4;
		length = retval;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	Item Routines
//
///////////////////////////////////////////////////////////////////////////////


/**
 * \brief Convert item data into RGB raw data.
 * \param[in] data data chunk to decode.
 * \param[in] pal palette.
 * \return On success pointer to RGB data, otherwise NULL.
 * \note src and dest can point to the same memory block.
 */
PRIVATE W8 *DecodeItem( W8 *data, W8 *pal )
{
	W8 *buffer, *mask, *ptr;
	SW32 x, y, w, h;

	buffer = (W8 *)MM_MALLOC( 128 * 128 * 4 );
	if( buffer == NULL )
	{
		fprintf( stderr, "Could not allocate memory block\n" );

		return NULL;
	}

	memset( buffer, 0, 128 * 128 * 4 );

	x = data[ 0 ] << 8 | data[ 1 ];
	y = data[ 2 ] << 8 | data[ 3 ];
	w = data[ 4 ] << 8 | data[ 5 ];
	h = data[ 6 ] << 8 | data[ 7 ];

	data += 8;
	mask = data + w * h;
	ptr = buffer + 512 * y + x * 4;

	do
	{
		SW32 w2 = w;
		do
		{
			if( *mask == 0 )
			{
				ptr[ 0 ] = pal[ data[ 0 ] * 3 ];
				ptr[ 1 ] = pal[ data[ 0 ] * 3 + 1 ];
				ptr[ 2 ] = pal[ data[ 0 ] * 3 + 2 ];
				ptr[ 3 ] = 255;
			}
			ptr += 4;
			data++;
			mask++;

		} while( --w2 );

		ptr += 4 * (128 - w);

	} while( --h );

	return buffer;
}

/**
 * \brief Extract pic images from resource file.
 */
PRIVATE void decodeItems( void )
{
	W8 *ptrResource;
	W8 *ptrDst;
	W32 *ptrLong;
	W8 *buffer;
	W8 **gameItems;
	W32 junk;
	W32 uncomprLength;
	W32 i;
	char name[ 256 ];
	W16 *ptrShape1;
	W8 *ptrShape2;
	W16 width, height;
	W32 offset = 634799L;
	W32 length = 67453L;


	ptrResource = (PW8)getResourceBlock( offset, length, &junk );

	uncomprLength = BigLong( *((PW32)ptrResource) );
	gameItems = (W8 **)MM_MALLOC( uncomprLength );
	if( gameItems == NULL )
	{
		printf( "Could not allocate memory block\n" );

		MM_FREE( ptrResource );

		return;
	}

	Decode_LZSS( (PW8)gameItems, ptrResource+4, uncomprLength );

	MM_FREE( ptrResource );

	ptrLong = (PW32)gameItems;
	ptrDst = (PW8)gameItems;
	for( i = 0; i < 47; ++i )
	{
		uncomprLength = BigLong( ptrLong[ i ] );
		gameItems[ i ] = ptrDst + uncomprLength;
	}

	for( i = 0 ; i < 6 * 4 ; ++i )
	{
        wt_snprintf( name, sizeof( name ), "%s%cweapon%.2d.tga", DIRPATHPICS, PATH_SEP, i );
		buffer = DecodeItem( gameItems[ 12 + i ], macPalette );
		TGA_write( name, 32, 128, 128, buffer, 0, 1 );

		MM_FREE( buffer );
	}

	for( i = 0 ; i < 47 ; ++i )
	{
		ptrShape1 = (PW16)gameItems[ i ];
		width = BigShort( ptrShape1[ 0 ] );
		height = BigShort( ptrShape1[ 1 ] );
		ptrShape2 = (PW8)&ptrShape1[ 2 ];

		buffer = (W8 *)MM_MALLOC( width * height * 3 );
		if( buffer == NULL )
		{
			fprintf( stderr, "Could not allocate memory block\n" );

			continue;
		}

		ConvertPaletteToRGB( buffer, ptrShape2, width * height, macPalette );

        wt_snprintf( name, sizeof( name ), "%s%c%.2d.tga", DIRPATHPICS, PATH_SEP, i );
		TGA_write( name, 24, width, height, buffer, 0, 1 );

		MM_FREE( buffer );

		// Skip over weapon frames
		if( 11 == i )
		{
			i = 35;
		}
	}

	MM_FREE( gameItems );
}

///////////////////////////////////////////////////////////////////////////////
//
//	Midi Routines
//
///////////////////////////////////////////////////////////////////////////////

/**
 * \brief Extract midi files from resource fork.
 */
PRIVATE void decodeMidi( void )
{
	W8 *ptrResource;
	W32 retval;
	FILE *fhandle;
	char name[ 256 ];
	W32 i;
	W32 offset = 1899536L;
	W32 length = 8215L;

	for( i = 0 ; i < 8 ; ++i )
	{
		ptrResource = getResourceBlock( offset, length, &retval );
		if( ! ptrResource )
		{
			continue;
		}

		wt_snprintf( name, sizeof( name ), "%s%c%d.mid", DIRPATHMIDI, PATH_SEP, i );
		fhandle = fopen( name, "wb" );
		if( ! fhandle )
		{
			MM_FREE( ptrResource );
			continue;
		}

		fwrite( ptrResource, 1, length, fhandle );

		fclose( fhandle );
		MM_FREE( ptrResource );

		offset += length + 4;
		length = retval;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	PICT image
//
///////////////////////////////////////////////////////////////////////////////

#if 0

PRIVATE void DecodePICTimage( W32 offset, W32 length )
{
	FILE *hfOut;
	W8 *ptrResource;
	W32 junk;
	int i;

	ptrResource = getResourceBlock( offset, length, &junk );
	if( ! ptrResource )
	{
		return;
	}

	hfOut = fopen( "pict.pct", "wb" );
	if( ! hfOut )
	{
		return;
	}

	for( i ; i < 512 ; ++i )
	{
		fwrite( ptrResource, 1, 0, hfOut );
	}

	fwrite( ptrResource, 1, length, hfOut );

	fclose( hfOut );
}

#endif

///////////////////////////////////////////////////////////////////////////////
//
//	Parse MacBinary Header
//
///////////////////////////////////////////////////////////////////////////////

/**
 * \brief Parse mac binary header.
 * \return Returns true if this is Wolfenstein Mac binary file, otherwise false.
 */
PRIVATE wtBoolean parseMacBinaryHead( void )
{
	W32 temp32 = 0;
	char name[ 64 ];

//
// Check file name
//
	fseek( fResHandle, 1, SEEK_SET );

	// get file name length (range is 1 to 31)
	fread( &temp32, 1, 1, fResHandle );
	if( temp32 < 1 || temp32 > 31 )
	{
		return false;
	}

	fread( name, 1, temp32, fResHandle );
	name[ temp32 - 1 ] = '\0';
	if( strcmp( name, MACBINFILENAME ) != 0 )
	{
		return false;
	}

//
//	Check file type / creator
//
	fseek( fResHandle, 65, SEEK_SET );

	fread( &name, 1, 8, fResHandle );
	name[ 8 ] = '\0';
	if( strcmp( name, FILETYPECREATOR ) != 0 )
	{
		return false;
	}

//
//	Check Data Fork length
//
	fseek( fResHandle, 83, SEEK_SET );

	fread( &temp32, 1, 4, fResHandle );

	temp32 = BigLong( temp32 );
	if( temp32 != DATAFORKLENGTH )
	{
		return false;
	}

//
//	Check Resource Fork length
//
	fread( &temp32, 1, 4, fResHandle );

	temp32 = BigLong( temp32 );
	if( temp32 != RESFORKLENGTH )
	{
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Interface
//
///////////////////////////////////////////////////////////////////////////////

/**
 * \brief Interface to Macintosh data extractor.
 */
PUBLIC void Macintosh_Decoder( void )
{
	char *fname;
	char ext[ 256 ];

	wt_strlcpy( ext, MAC_FEXT, sizeof( ext ) );


	fname = FS_FindFirst( ext );
	FS_FindClose();

	if( fname == NULL )
	{
		// try again with lower case
		fname = FS_FindFirst( wt_strlwr( ext ) );
		FS_FindClose();

		if( fname == NULL )
		{
			fprintf( stderr, "Could not find any mac files for read\n" );

			return;
		}
	}

	fResHandle = fopen( fname, "rb" );
	if( fResHandle == NULL )
	{
		fprintf( stderr, "Could not open file (%s) for read\n", fname );

		return;
	}

	if( ! parseMacBinaryHead() )
	{
		fprintf( stderr, "Unknown MacBinary file\n" );

		fclose( fResHandle );

		return;
	}



	FS_CreateDirectory( DIRPATHPICS );
	FS_CreateDirectory( DIRPATHSPRITES );
	FS_CreateDirectory( DIRPATHWALLS );
	FS_CreateDirectory( DIRPATHMIDI );

	printf( "Wolfenstein MAC Decoding\n\n" );

	printf( "Extracting midi data..." );
	decodeMidi();
    printf( "Done\n" );

	printf( "Decoding screen data..." );
	decodeScreens();
    printf( "Done\n" );


	setPalette( GAMEPAL );

	printf( "Decoding wall data..." );
	decodeWalls();
    printf( "Done\n" );

	printf( "Decoding sprite data..." );
	decodeSprites();
    printf( "Done\n" );

	printf( "Decoding item data..." );
	decodeItems();
    printf( "Done\n" );

	fclose( fResHandle );
}



/**
 * \brief Interface to Wolfenstein 3DO data extractor.
 */
PUBLIC void wolf3do_decoder( void )
{

}
