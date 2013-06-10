/*

	Copyright (C) 2007-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file wolfcore_pm.c
 * \brief Wolfenstein 3-D page file decoder.
 * \author Michael Liebscher
 * \date 2007-2009
 * \note Portion of this code was derived from Wolfenstein 3-D, and was originally written by Id Software, Inc.
 */

#include <stdio.h>
#include <string.h>

#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../string/wtstring.h"
#include "../../memory/memory.h"
#include "../../loaders/wav.h"
#include "../../loaders/tga.h"
#include "../../image/image.h"
#include "../../image/hq2x.h"

#include "../../image/scalebit.h"

#include "../wolfenstein/wolf.h"


PUBLIC const W32 SAMPLERATE  =    7000;    /* In Hz */

extern wtBoolean _saveAudioAsWav;
extern W32 _filterScale;
extern W32 _filterScale_Sprites;

typedef	struct
{
    W32  offset;	/* Offset of chunk into file */
    W16  length;	/* Length of the chunk */

} PageList_t;


typedef struct
{
	W16 leftpix, rightpix;
	W16	dataofs[ 64 ];
    // table data after dataofs[ rightpix - leftpix + 1 ]

} t_compshape;


PRIVATE PageList_t	*PMPages = NULL;

PRIVATE FILE		*file_handle_page = NULL;


/**
 * \brief Setup page file for decoding.
 * \param[in] pagefname Source buffer to convert from
 * \param[out] nBlocks Destination buffer to convert to.
 * \param[out] SpriteStart Offset index for sprite data.
 * \param[out] SoundStart Offset index for sound data.
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean PageFile_Setup( const char *pagefname, W32 *nBlocks, W32 *SpriteStart, W32 *SoundStart )
{
	W32    i;
	W32  size;
	void *buf = NULL;
	W32  *offsetptr;
	char *temp_fileName = NULL;
	PageList_t *page;
	W16 *lengthptr;
	W16 tval;
	W32 PMNumBlocks;
	W32 PMSpriteStart;
	W32 PMSoundStart;


	if( ! pagefname || ! *pagefname )
	{
		fprintf( stderr, "[PageFile_Setup]: Invalid file name\n" );

		goto PMSetupFailure;
	}

	temp_fileName = (char *) MM_MALLOC( strlen( pagefname ) + 1 );
	if( temp_fileName == NULL )
	{
		goto PMSetupFailure;
	}

	wt_strlcpy( temp_fileName, pagefname, strlen( pagefname ) + 1 );


	/* Open page file */
	file_handle_page = fopen( wt_strupr( temp_fileName ), "rb" );
	if( file_handle_page == NULL )
	{
		file_handle_page = fopen( wt_strlwr( temp_fileName ), "rb" );
		if( file_handle_page == NULL )
		{
			fprintf( stderr, "Could not open file (%s) for read!\n", temp_fileName );

			goto PMSetupFailure;
		}
	}


	/* Read in header variables */
	fread( &tval, sizeof( W16 ), 1, file_handle_page );
	PMNumBlocks = LittleShort( tval );

	fread( &tval, sizeof( W16 ), 1, file_handle_page );
	PMSpriteStart = LittleShort( tval );

	fread( &tval, sizeof( W16 ), 1, file_handle_page );
	PMSoundStart = LittleShort( tval );


	/* Allocate and clear the page list */
	PMPages = (PageList_t *) MM_MALLOC( sizeof( PageList_t ) * PMNumBlocks );
	if( PMPages == NULL )
	{
		goto PMSetupFailure;
	}


	memset( PMPages, 0, sizeof( PageList_t ) * PMNumBlocks );


	/* Read in the chunk offsets */
	size = sizeof( W32 ) * PMNumBlocks;

	buf = MM_MALLOC( size );
	if( buf == NULL )
	{
		goto PMSetupFailure;
	}


	if( fread( buf, 1, size, file_handle_page ) == 0 )
	{
		fprintf( stderr, "Could not read chunk offsets from file (%s)\n", temp_fileName );
	}

	offsetptr = (PW32) buf;
	for( i = 0, page = PMPages; i < PMNumBlocks; i++, page++ )
	{
		page->offset = LittleLong( *offsetptr++ );
	}
	MM_FREE( buf );


	/* Read in the chunk lengths */
	size = sizeof( W16 ) * PMNumBlocks;

	buf = MM_MALLOC( size );
	if( buf == NULL )
	{
		goto PMSetupFailure;
	}

	if( fread( buf, 1, size, file_handle_page ) == 0 )
	{
		fprintf( stderr, "Could not read chunk lengths from file (%s)\n", temp_fileName );
	}

	lengthptr = (PW16)buf;
	for( i = 0, page = PMPages ; i < PMNumBlocks ; ++i, page++ )
	{
		page->length = LittleShort( *lengthptr++ );
	}

	MM_FREE( buf );
	MM_FREE( temp_fileName );

	*nBlocks = PMNumBlocks;
	*SpriteStart = PMSpriteStart;
	*SoundStart = PMSoundStart;

	return true;

PMSetupFailure:

	MM_FREE( temp_fileName );
	MM_FREE( PMPages );
	MM_FREE( buf );

	return false;
}

/**
 * \brief Shutdown page cache.
 */
PUBLIC void PageFile_Shutdown( void )
{
	if( file_handle_page ) {
		fclose( file_handle_page );
		file_handle_page = NULL;
	}

	MM_FREE( PMPages );
}

/**
 * \brief Reads in data from Page file.
 * \param[out] buf Storage location for data.
 * \param[in] offset Number of bytes from beginning of file.
 * \param[in] length Maximum number of items to be read.
 * \return Nothing.
 */
PRIVATE void PageFile_ReadFromFile( W8 *buf, SW32 offset, W32 length )
{
	if( ! buf )
	{
		fprintf( stderr, "[PageFile_ReadFromFile]: NULL pointer\n" );

		return;
	}

	if( ! offset )
	{
		fprintf( stderr, "[PageFile_ReadFromFile]: Zero offset\n" );

		return;
	}





	if( fseek( file_handle_page, offset, SEEK_SET ) )
	{
		fprintf( stderr, "[PageFile_ReadFromFile]: Seek failed\n" );

		return;
	}

	if( ! fread( buf, 1, length, file_handle_page ) )
	{
		fprintf( stderr, "[PageFile_ReadFromFile]: Read failed\n" );

		return;
	}
}

/**
 * \brief Get Page file raw data.
 * \param[in] pagenum Page to load.
 * \param[in] length Length of data.
 * \return On success pointer to data block, otherwise NULL.
 * \note Caller is responsible for freeing allocated data by calling MM_FREE.
 */
PUBLIC void *PageFile_getPage( W32 pagenum, W32 *length )
{
    W8 *addr;
    PageList_t *page;
	W32  pageOffset;
    W16  pageLength;


	*length = 0;

    page = &PMPages[ pagenum ];


	pageOffset = LittleLong( page->offset );
    pageLength = LittleShort( page->length );


	if( pageLength == 0 )
	{
		return NULL;
	}

    addr = (PW8)MM_MALLOC( pageLength );
	if( addr == NULL )
	{
		return NULL;
	}

    PageFile_ReadFromFile( addr, pageOffset, pageLength );

	*length = pageLength;

    return addr;
}

/**
 * \brief Decodes raw wall data into RGB-24.
 * \param[in] data Raw wall data.
 * \param[in] palette Palette array.
 * \return On success pointer to raw image data block, otherwise NULL.
 * \note Caller is responsible for freeing allocated data by calling MM_FREE.
 */
PUBLIC void *PageFile_decodeWall_RGB24( W8 *data, W8 *palette )
{
	W32 temp;
	W8 *buffer;
	W8 *ptr;
	W32 x, y;

	buffer = (PW8) MM_MALLOC( 64 * 64 * 3 );
	if( buffer == NULL )
	{
		return NULL;
	}

	for( x = 0 ; x < 64 ; ++x )
	{
		for( y = 0 ; y < 64 ; ++y )
		{
			temp = ( data[ (x << 6) + y ] ) * 3;

			ptr = buffer + ( ( (y << 6) + x ) * 3 );

			ptr[ 0 ] = palette[ temp + 0 ];		/* R */
			ptr[ 1 ] = palette[ temp + 1 ];		/* G */
			ptr[ 2 ] = palette[ temp + 2 ];		/* B */
		}
	}

	return (void *)buffer;
}

/**
 * \brief Decodes raw wall data into RGB-32.
 * \param[in] data Raw wall data.
 * \param[in] palette Palette array.
 * \return On success pointer to raw image data block, otherwise NULL.
 * \note Caller is responsible for freeing allocated data by calling MM_FREE.
 */
PUBLIC void *PageFile_decodeWall_RGB32( W8 *data, W8 *palette )
{
	W32 temp;
	W8 *buffer;
	W8 *ptr;
	W32 x, y;

	buffer = (PW8)MM_MALLOC( 64 * 64 * 4 );
	if( NULL == buffer )
	{
		return NULL;
	}

	for( x = 0 ; x < 64 ; ++x )
	{
		for( y = 0 ; y < 64 ; ++y )
		{
			temp = ( data[ (x << 6) + y ] ) * 3;

			ptr = buffer + ( ( (y << 6) + x ) * 4 );

			ptr[ 0 ] = palette[ temp + 0 ];		/* R */
			ptr[ 1 ] = palette[ temp + 1 ];		/* G */
			ptr[ 2 ] = palette[ temp + 2 ];		/* B */

            ptr[ 3 ] = 0xFF;                    /* A */
		}
	}

	return (void *)buffer;
}

/**
 * \brief Decodes raw sprite data into RGB-24.
 * \param[in] data Raw sprite data.
 * \param[in] palette Palette array.
 * \return On success pointer to raw image data block, otherwise NULL.
 * \note Caller is responsible for freeing allocated data by calling MM_FREE.
 */
PUBLIC void *PageFile_decodeSprite_RGB24( W8 *data, W8 *palette )
{
	W32 i;
	W32 temp;
	W32 x, y;
	W8 *buffer;
	W8 *ptr;
	W16 *cmdptr;
    SW16 *linecmds;
	t_compshape *shape;

	W16 leftpix, rightpix;

	buffer = (PW8)MM_MALLOC( 64 * 64 * 3 );
	if( NULL == buffer )
	{
		return NULL;
	}

	/* all transparent at the beginning */
	for( x = 0 ; x < (64 * 64 * 3) ; x += 3 )
	{
		ptr = buffer + x;

		ptr[ 0 ] = 0xFF;		/* R */
		ptr[ 1 ] = 0x00;		/* G */
		ptr[ 2 ] = 0xFF;		/* B */
	}

	shape = (t_compshape *)data;

	leftpix = LittleShort( shape->leftpix );
	rightpix = LittleShort( shape->rightpix );

	cmdptr = shape->dataofs;
	for( x = leftpix ; x <= rightpix ; ++x )
	{
		linecmds = (PSW16)(data + LittleShort( *cmdptr ));
		cmdptr++;
		for( ; LittleShort( *linecmds ) ; linecmds += 3 )
		{
			i = (LittleShort( linecmds[ 2 ] ) / 2) + LittleShort( linecmds[ 1 ] );
			for( y = (W32)(LittleShort( linecmds[ 2 ] ) / 2) ; y < (W32)(LittleShort( linecmds[ 0 ] ) / 2) ; ++y, ++i )
			{
				temp = data[ i ] * 3;

				ptr = buffer + (y * 64 + x) * 3;

				ptr[ 0 ] = palette[ temp + 0 ];		/* R */
				ptr[ 1 ] = palette[ temp + 1 ];		/* G */
				ptr[ 2 ] = palette[ temp + 2 ];		/* B */
			}
		}
	}

	return (void *)buffer;
}

/**
 * \brief Decodes raw sprite data into RGB32.
 * \param[in] data Raw sprite data.
 * \param[in] palette Palette array.
 * \return On success pointer to raw image data block, otherwise NULL.
 * \note Caller is responsible for freeing allocated data by calling MM_FREE.
 */
PUBLIC void *PageFile_decodeSprite_RGB32( W8 *data, W8 *palette )
{
	W32 i;
	W32 temp;
	W32 x, y;
	W8 *buffer;
	W8 *ptr;
	W16 *cmdptr;
	SW16 *linecmds;

	t_compshape *shape;

	W16 leftpix, rightpix;

	buffer = (PW8)MM_MALLOC( 64 * 64 * 4 );
	if( NULL == buffer )
	{
		return NULL;
	}


	for( x = 0 ; x < (64 * 64 * 4) ; x += 4 )
	{
		ptr = buffer + x;

		ptr[ 0 ] = 0xFF;	/* R */
		ptr[ 1 ] = 0x00;	/* G */
		ptr[ 2 ] = 0xFF;	/* B */
        ptr[ 3 ] = 0x00;	/* A */
	}

	shape = (t_compshape *)data;

	leftpix = LittleShort( shape->leftpix );
	rightpix = LittleShort( shape->rightpix );

	cmdptr = shape->dataofs;
	for( x = leftpix ; x <= rightpix ; ++x )
	{
        linecmds = (PSW16)(data + LittleShort( *cmdptr ));
		cmdptr++;
		for( ; LittleShort( *linecmds ) ; linecmds += 3 )
		{
			i = (LittleShort( linecmds[ 2 ] ) / 2) + LittleShort( linecmds[ 1 ] );
			for( y = (W32)(LittleShort( linecmds[ 2 ] ) / 2) ; y < (W32)(LittleShort( linecmds[ 0 ] ) / 2) ; ++y, ++i )
			{
				temp = data[ i ] * 3;

				ptr = buffer + (y * 64 + x) * 4;

				ptr[ 0 ] = palette[ temp + 0 ];		/* R */
				ptr[ 1 ] = palette[ temp + 1 ];		/* G */
				ptr[ 2 ] = palette[ temp + 2 ];		/* B */
                ptr[ 3 ] = 0xFF;		            /* A */
			}
		}
	}

	return (void *)buffer;
}

/**
 * \brief Remap sprite index number based on game version
 * \param[in] index Sprite index.
 * \return Remapped sprite index.
 */
PRIVATE int GetSpriteMappedIndex( int index )
{
    int returnValue = index;

    if( wolf_version == ACTIVISION_WL6_V14 || wolf_version == ID_SOFTWARE_WL6_V14 )
    {

	    if( index >= 50 && index < 385 )
	    {
		    returnValue = index + 4;
	    }
	    else if( index >= 385 && index < 408 )
	    {
		    returnValue = index + 23;
	    }
	    else if( index >= 392 && index < 416 )
	    {
		    returnValue = index + 23;
	    }
	    else if( index >= 416 )
	    {
		    returnValue = index + 98;
	    }
    }
    else if ( wolf_version == APOGEE_WL6_V14G )
    {
        if( index >= 50 && index < 385 )
	    {
		    returnValue = index + 4;
	    }
    }
    else if( wolf_version == WL1_V10 )
    {
        if( index >= 49 && index < 369)
	    {
		    returnValue = index + 5;
	    }
        else if( index >= 369 )
        {
            returnValue = index + 141;
        }
    }
    else if( wolf_version == WL1_V11 )
    {
        if( index >= 50 && index < 414)
	    {
		    returnValue = index + 4;
	    }
        if( index >= 414 )
	    {
		    returnValue = index + 100;
	    }
    }
    else if( wolf_version >= SPEAR_OF_DESTINY )
    {
        if( index >= 326 )
	    {
		    returnValue = index + 113;
	    }
    }

	return returnValue;
}

/**
 * \brief Remap wall index number based on game version
 * \param[in] index Wall index.
 * \return Remapped wall index.
 */
PRIVATE int GetWallMappedIndex( int index )
{
	int returnValue = index;

    if( wolf_version == ACTIVISION_WL6_V14 || wolf_version == ID_SOFTWARE_WL6_V14 || wolf_version == APOGEE_WL6_V14G )
    {
        if( index >= 98 )
	    {
		    returnValue = index + 28;
	    }
    }
    else if( wolf_version == WL1_V10 )
    {
        if( index >= 56 )
	    {
		    returnValue = index + 70;
	    }
    }
    else if( wolf_version == WL1_V11 )
    {

    }

	return returnValue;
}

/**
 * \brief Redux the Page file data.
 * \param[in] vsfname data file name.
 * \param[in] wallPath Path to save wall data.
 * \param[in] spritePath Path to save sprite data.
 * \param[in] soundPath Path to save sound data.
 * \param[in] palette Palette array.
 * \return On success true, otherwise false.
 * \note Caller is responsible for freeing allocated data by calling MM_FREE.
 */
PUBLIC wtBoolean PageFile_ReduxDecodePageData( const char *vsfname, const char *wallPath, const char *spritePath, const char *soundPath, W8 *palette )
{
	void *data;
	void *decdata;
	W32 length;
	char tempFileName[ 1024 ];
	W32 i;
	W32 SpriteStart, NumBlocks, SoundStart;
	W32 soundBufferSize;
	W8 *soundBuffer;
	W32 totallength;


	printf( "Decoding Page Data..." );

	if( ! PageFile_Setup( vsfname, &NumBlocks, &SpriteStart, &SoundStart ) )
	{
		PageFile_Shutdown();

		return false;
	}

    // ////////////////////////////////////////////////////////////////////////
    // Decode Walls

	for( i = 0 ; i < SpriteStart ; ++i )
	{
		data = PageFile_getPage( i, &length );
		if( data == NULL )
		{
			continue;
		}

		decdata = PageFile_decodeWall_RGB32( (PW8)data, palette );
		if( decdata == NULL )
		{
			fprintf( stderr, "[PageFile_ReduxDecodePageData]: Unable to decode wall (%d).\n", i );

			MM_FREE( data );

			continue;
		}


		if( _filterScale > 0 )
		{
			void *scaledImgBuf;

			scaledImgBuf = (void *) MM_MALLOC( 128 * 128 * 4 );
			if( NULL == scaledImgBuf )
			{
				MM_FREE( data );
				MM_FREE( decdata );
				continue;
			}


			// Scale2x
		        if( _filterScale == 1 )
			{
		                scale( 2, (void *)scaledImgBuf, 128 * 4, decdata, 64 * 4, 4, 64, 64 );
				RGB32toRGB24( (const PW8)scaledImgBuf, (PW8)scaledImgBuf, 128 * 128 * 4 );
			} else {
		                // hq2x
		                RGB32toRGB24( (const PW8)decdata, (PW8)decdata, 64 * 64 * 4 );
		                RGB24toBGR565( decdata, decdata, 64 * 64 * 3 );
				hq2x_32( (PW8)decdata, (PW8)scaledImgBuf, 64, 64, 64 * 2 * 4  );
        		        RGB32toRGB24( (const PW8)scaledImgBuf, (PW8)scaledImgBuf, 128 * 128 * 4 );

        		}

    		        wt_snprintf( tempFileName, sizeof( tempFileName ), "%s%c%.3d.tga", wallPath, PATH_SEP, GetWallMappedIndex( i ) );
			TGA_write( tempFileName, 24, 128, 128, scaledImgBuf, 0, 1 );

			MM_FREE( scaledImgBuf );

		} else {
		        wt_snprintf( tempFileName, sizeof( tempFileName ), "%s%c%.3d.tga", wallPath, PATH_SEP, GetWallMappedIndex( i ) );

		        RGB32toRGB24( (const PW8)decdata, (PW8)decdata, 64 * 64 * 4 );
			TGA_write( tempFileName, 24, 64, 64, decdata, 0, 1 );
		}


		MM_FREE( data );
		MM_FREE( decdata );
	}


    // ////////////////////////////////////////////////////////////////////////
    // Decode Sprites

	for( i = SpriteStart ; i < SoundStart ; ++i )
	{
		data = PageFile_getPage( i, &length );
		if( data == NULL )
		{
			continue;
		}

		decdata = PageFile_decodeSprite_RGB32( (PW8)data, palette );
		if( decdata == NULL )
		{
			MM_FREE( data );

			continue;
		}

		if( _filterScale_Sprites > 0 )
		{
			W8 *scaledImgBuf;

			scaledImgBuf = (PW8) MM_MALLOC( 128 * 128 * 4 );
			if( NULL == scaledImgBuf ) {
				MM_FREE( data );
				MM_FREE( decdata );
				continue;
			}
			if( _filterScale_Sprites == 1 ) {
				scale( 2, (void *)scaledImgBuf, 128 * 4, decdata, 64 * 4, 4, 64, 64 );
			} else {
			// hq2x
				RGB32toRGB24( (const PW8)decdata, (PW8)decdata, 64 * 64 * 4 );
				RGB24toBGR565( decdata, decdata, 64 * 64 * 3 );
				hq2x_32( (PW8)decdata, (PW8)scaledImgBuf, 64, 64, 64 * 2 * 4  );
				ReduxAlphaChannel_hq2x( scaledImgBuf, 128, 128 );
			}

			wt_snprintf( tempFileName, sizeof( tempFileName ), "%s%c%.3d.tga", spritePath, PATH_SEP, GetSpriteMappedIndex( i - SpriteStart ) );
			TGA_write( tempFileName, 32, 128, 128, scaledImgBuf, 0, 1 );
			MM_FREE( scaledImgBuf );
		} else {
			wt_snprintf( tempFileName, sizeof( tempFileName ), "%s%c%.3d.tga", spritePath, PATH_SEP, GetSpriteMappedIndex( i - SpriteStart )  );
			TGA_write( tempFileName, 32, 64, 64, decdata, 0, 1 );
		}
		MM_FREE( data );
		MM_FREE( decdata );
	}


    // ////////////////////////////////////////////////////////////////////////
    // Decode SFX

	soundBufferSize = 20 * 4096;
	soundBuffer = (PW8) MM_MALLOC( soundBufferSize );
	if( soundBuffer == NULL )
	{
		PageFile_Shutdown();

		return false;
	}

	totallength = 0;
	for( i = SoundStart ; i < NumBlocks ; ++i )
	{
		data = PageFile_getPage( i, &length );
		if( data == NULL )
		{
			continue;
		}


		if( (totallength + length) > soundBufferSize )
		{
			fprintf( stderr, "[PageFile_ReduxDecodePageData]: Buffer not large enough to hold sound data!\n" );


			MM_FREE( data );
			MM_FREE( soundBuffer );

			return false;
		}

	  	MM_MEMCPY( soundBuffer + totallength, data, length );

		totallength += length;


		if( length < 4096 )
		{

			wt_snprintf( tempFileName, sizeof( tempFileName ), "%s%c%.3d.wav", soundPath, PATH_SEP, i - SoundStart );
			wav_write( tempFileName, soundBuffer, totallength, 1, SAMPLERATE, 1 );


			totallength = 0;

		}


		MM_FREE( data );


	}
	MM_FREE( soundBuffer );

	PageFile_Shutdown();
	MM_FREE( data );

	printf( "Done\n" );

	return true;
}
