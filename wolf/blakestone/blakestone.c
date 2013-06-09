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
 * \file blakestone.c
 * \brief Blake Stone Aliens of Gold/Planet Strike data file decoder.
 * \author Michael Liebscher
 * \date 2007-2013
 */

#include <stdio.h>

#include "blakestone.h"

#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../filesys/file.h"
#include "../../image/image.h"
#include "../core/wolfcore.h"

#include "../../string/wtstring.h"
#include "../../memory/memory.h"
#include "../../loaders/tga.h"
#include "../wolfcore_decoder.h"


extern W8 blakestone_gamepal[];
extern W32 _filterScale;

/**
 * \brief Decodes Blake Stone Aliens of Gold Full Version data.
 * \return Nothing.
 */
PUBLIC void blakestoneAGfull_decoder( void )
{
	W32 width, height;
	void *data;
	char fname[ 1024 ];

	W8 *tempPalette;

	printf( "\n\nBlake Stone: Aliens of Gold Decoding\n" );

	if( ! buildCacheDirectories() )
    {
        printf( "Unable to create cache directories\n" );

		return;
    }

	if( GFXFile_Setup( "VGADICT.BS6", "VGAHEAD.BS6", "VGAGRAPH.BS6" ) )
	{
		GFXFile_decodeFont( 1, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 2, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 3, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 4, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 5, 256, 128, DIR_PICS );

		GFXFile_decodeScript( 181, 223, DIR_GSCRIPTS );

		GFXFile_decodeGFX( 6, 164, blakestone_gamepal, DIR_PICS );


		GFXFile_cacheChunk( 168 );
		tempPalette = (PW8)GFXFile_getChunk( 168 );

		data = GFXFile_decodeChunk_RGB24( 29, &width, &height, tempPalette );
		if( data )
		{
			wt_snprintf( fname, sizeof( fname ), "%s%c%.3d.tga", DIR_PICS, PATH_SEP, 29 );

			RGB24_adjustBrightness( data, width * height * 3 );

			TGA_write( fname, 24, width, height, data, 0, 1 );

			MM_FREE( data );
		}


		GFXFile_cacheChunk( 167 );
		tempPalette = (PW8)GFXFile_getChunk( 167 );

		data = GFXFile_decodeChunk_RGB24( 30, &width, &height, tempPalette );
		if( data )
		{
            wt_snprintf( fname, sizeof( fname ), "%s%c%.3d.tga", DIR_PICS, PATH_SEP, 30 );

			RGB24_adjustBrightness( data, width * height * 3 );

			TGA_write( fname, 24, width, height, data, 0, 1 );

			MM_FREE( data );
		}

	}
	GFXFile_Shutdown();



	PageFile_ReduxDecodePageData( "VSWAP.BS6", DIR_WALLS, DIR_SPRITES, DIR_DSOUND, blakestone_gamepal );

/*
	if( AudioFile_Setup( "AUDIOHED.BS6", "AUDIOT.BS6" ) )
	{
		AudioFile_ReduxDecodeSound( 0, 200, DIR_SOUNDFX );

		AudioFile_ReduxDecodeMusic( 200, 210, DIR_MUSIC, NULL );
	}
	AudioFile_Shutdown();
*/
}

/**
 * \brief Decodes Blake Stone Aliens of Gold Shareware data.
 * \return Nothing.
 */
PUBLIC void blakestoneAGshare_decoder( void )
{
	W32 width, height;
	void *data;
	char fname[ 1024 ];

	W8 *tempPalette;


	printf( "Blake Stone: Aliens of Gold Shareware Decoding\n\n" );

	if( ! buildCacheDirectories() )
    {
        fprintf( stderr, "Unable to create cache directories\n" );

		return;
    }


	if( GFXFile_Setup( "VGADICT.BS1", "VGAHEAD.BS1", "VGAGRAPH.BS1" ) )
	{

		GFXFile_decodeFont( 1, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 2, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 3, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 4, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 5, 256, 128, DIR_PICS );

        GFXFile_decodeScript( 181, 212, DIR_GSCRIPTS );

		GFXFile_decodeGFX( 6, 168, blakestone_gamepal, DIR_PICS );

		GFXFile_cacheChunk( 171 );
		tempPalette = (PW8)GFXFile_getChunk( 171 );

		data = GFXFile_decodeChunk_RGB24( 29, &width, &height, tempPalette );
		if( data )
		{
			wt_snprintf( fname, sizeof( fname ), "%s%c%.3d.tga", DIR_PICS, PATH_SEP, 29 );

			RGB24_adjustBrightness( data, width * height * 3 );

			TGA_write( fname, 24, width, height, data, 0, 1 );

			MM_FREE( data );
		}


		GFXFile_cacheChunk( 175 );
		tempPalette = (PW8)GFXFile_getChunk( 175 );

		data = GFXFile_decodeChunk_RGB24( 30, &width, &height, tempPalette );
		if( data )
		{
			wt_snprintf( fname, sizeof( fname ), "%s%c%.3d.tga", DIR_PICS, PATH_SEP, 30 );

			RGB24_adjustBrightness( data, width * height * 3 );

			TGA_write( fname, 24, width, height, data, 0, 1 );

			MM_FREE( data );
		}

	}
	GFXFile_Shutdown();


	PageFile_ReduxDecodePageData( "VSWAP.BS1", DIR_WALLS, DIR_SPRITES, DIR_DSOUND, blakestone_gamepal );

/*
	if( AudioFile_Setup( "AUDIOHED.BS1", "AUDIOT.BS1" ) )
	{
		AudioFile_ReduxDecodeSound( 0, 0, DIR_SOUNDFX );

		AudioFile_ReduxDecodeMusic( 0, 0, DIR_MUSIC, NULL );
	}
	AudioFile_Shutdown();
*/
}


/**
 * \brief Decodes Blake Stone Planet Strike data.
 * \return Nothing.
 */
PUBLIC void blakestonePS_decoder( void )
{
	W32 width, height;
	void *data;
	char fname[ 256 ];

	W8 *tempPalette;


	printf( "Blake Stone: Planet Strike Decoding\n\n" );

	if( ! buildCacheDirectories() )
    {
        fprintf( stderr, "Unable to create cache directories\n" );

		return;
    }

	if( GFXFile_Setup( "VGADICT.VSI", "VGAHEAD.VSI", "VGAGRAPH.VSI" ) )
	{
		GFXFile_decodeFont( 1, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 2, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 3, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 4, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 5, 256, 128, DIR_PICS );


        GFXFile_decodeScript( 216, 248, DIR_GSCRIPTS );


		GFXFile_decodeGFX( 6, 197, blakestone_gamepal, DIR_PICS );



		GFXFile_cacheChunk( 201 );
		tempPalette = (PW8)GFXFile_getChunk( 201 );

		data = GFXFile_decodeChunk_RGB24( 53, &width, &height, tempPalette );
		if( data )
		{
			wt_snprintf( fname, sizeof( fname ), "%s%c%.3d.tga", DIR_PICS, PATH_SEP, 53 );

			RGB24_adjustBrightness( data, width * height * 3 );

			TGA_write( fname, 24, width, height, data, 0, 1 );

			MM_FREE( data );
		}


		GFXFile_cacheChunk( 203 );
		tempPalette = (PW8)GFXFile_getChunk( 203 );

		data = GFXFile_decodeChunk_RGB24( 143, &width, &height, tempPalette );
		if( data )
		{
			wt_snprintf( fname, sizeof( fname ), "%s%c%.3d.tga", DIR_PICS, PATH_SEP, 143 );

			RGB24_adjustBrightness( data, width * height * 3 );

			TGA_write( fname, 24, width, height, data, 0, 1 );

			MM_FREE( data );
		}

		data = GFXFile_decodeChunk_RGB24( 144, &width, &height, tempPalette );
		if( data ) {
			wt_snprintf( fname, sizeof( fname ), "%s%c%.3d.tga", DIR_PICS, PATH_SEP, 144 );

			RGB24_adjustBrightness( data, width * height * 3 );

			TGA_write( fname, 24, width, height, data, 0, 1 );

			MM_FREE( data );
		}

	}
	GFXFile_Shutdown();



	PageFile_ReduxDecodePageData( "VSWAP.VSI", DIR_WALLS, DIR_SPRITES, DIR_DSOUND, blakestone_gamepal );

/*
	if( AudioFile_Setup( "AUDIOHED.VSI", "AUDIOT.VSI" ) )
	{
		AudioFile_ReduxDecodeSound( 0, 0, DIR_SOUNDFX );

		AudioFile_ReduxDecodeMusic( 0, 0, DIR_MUSIC, NULL );
	}
	AudioFile_Shutdown();
*/
}
