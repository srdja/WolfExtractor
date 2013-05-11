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
 * \file noah.c
 * \brief Super 3D Noah's Ark data file decoder.
 * \author Michael Liebscher 
 * \date 2007-2013
 */

#include <stdio.h>

#include "noah.h"

#include "../core/wolfcore.h"
#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../filesys/file.h"
#include "../../string/wtstring.h"
#include "../../memory/memory.h"
#include "../../loaders/tga.h"
#include "../../image/image.h"
#include "../wolfcore_decoder.h"



extern W8 noah_gamepal[];
extern W32 _filterScale;

/**
 * \brief Decodes Super 3D Noah's Ark data files.
 */
PUBLIC void super3dNoahsArk_decoder( void )
{
	W32 i;
	W32 width;
    W32 height;
	void *data;
	char fname[ 256 ];
	W8 *tempPalette;

	printf( "Super 3D Noah's Ark Decoding\n\n" );

	if( ! buildCacheDirectories() )
    {
		fprintf( stderr, "Unable to create cache directories\n" );

		return;
    }

	if( GFXFile_Setup( "VGADICT.N3D", "VGAHEAD.N3D", "VGAGRAPH.N3D" ) )
	{
		GFXFile_decodeFont( 1, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 2, 256, 128, DIR_PICS );


		GFXFile_decodeScript( 130, 131, DIR_GSCRIPTS );


		GFXFile_decodeGFX( 3, 125, noah_gamepal, DIR_PICS );


		GFXFile_cacheChunk( 127 );
		tempPalette = (PW8)GFXFile_getChunk( 127 );

		for( i = 80 ; i < 83 ; ++i )
		{
			data = GFXFile_decodeChunk_RGB24( i, &width, &height, tempPalette );
			if( data )
			{
                wt_snprintf( fname, sizeof( fname ), "%s%c%.3d.tga", DIR_PICS, PATH_SEP, i );

				RGB24_adjustBrightness( data, width * height * 3 );
				
				TGA_write( fname, 24, width, height, data, 0, 1 );
				
				MM_FREE( data );
			}
		}		

	}
	GFXFile_Shutdown();

	

	PageFile_ReduxDecodePageData( "VSWAP.N3D", DIR_WALLS, DIR_SPRITES, DIR_DSOUND, noah_gamepal );

/*
	if( AudioFile_Setup( "AUDIOHED.N3D", "AUDIOT.N3D" ) )
	{
		AudioFile_ReduxDecodeSound( 0, 0, DIR_SOUNDFX );

		AudioFile_ReduxDecodeMusic( 0, 0, DIR_MUSIC, NULL );
	}
	AudioFile_Shutdown();
*/
}

