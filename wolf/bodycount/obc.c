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
 * \file obc.c
 * \brief Operation: Body Count data file decoder.
 * \author Michael Liebscher 
 * \date 2007-2013
 */

#include <stdio.h>

#include "obc.h"

#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../filesys/file.h"
#include "../core/wolfcore.h"

#include "../../string/wtstring.h"
#include "../../memory/memory.h"
#include "../../loaders/tga.h"
#include "../wolfcore_decoder.h"


extern W8 obc_menupal[];
extern W8 obc_gamepal[];
extern W32 _filterScale;


/**
 * \brief Decodes Operation: Body Count data files.
 */
PUBLIC void obc_decoder( void )
{
	W32 i;
	W32 width, height;
	void *data;
	char tempFileName[ 1024 ];
	
	
	printf( "Operation: Body Count Decoding\n\n" );

	if( ! buildCacheDirectories() )
    {
        fprintf( stderr, "Unable to create cache directories\n" );

		return;
    }

    if( GFXFile_Setup( "VGADICT.BC", "VGAHEAD.BC", "VGAGRAPH.BC" ) )
	{
		GFXFile_decodeFont( 1, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 2, 256, 128, DIR_PICS );


		GFXFile_decodeGFX( 3, 55, obc_gamepal, DIR_PICS );
		
		for( i = 3 ; i < 10 ; ++i )
		{
			data = GFXFile_decodeChunk_RGB24( i, &width, &height, obc_menupal );
			if( data )
			{
                wt_snprintf( tempFileName, sizeof( tempFileName ), "%s%c%.3d.tga", DIR_PICS, PATH_SEP, i );
				
				TGA_write( tempFileName, 24, width, height, data, 0, 1 );
				
				MM_FREE( data );
			}
		}
	}
	GFXFile_Shutdown();

	
	PageFile_ReduxDecodePageData( "GFXTILES.BC", DIR_WALLS, DIR_SPRITES, DIR_DSOUND, obc_gamepal );


/*
	if( AudioFile_Setup( "AUDIOHED.BC", "AUDIOT.BC" ) )
	{
		AudioFile_ReduxDecodeSound( 0, 0, DIR_SOUNDFX );

		AudioFile_ReduxDecodeMusic( 0, 0, DIR_MUSIC, NULL );
	}
	AudioFile_Shutdown();
*/
}

/**
 * \brief Decodes Operation: Body Count Shareware data files.
 */
PUBLIC void obcshare_decoder( void )
{
//	printf( "\n\nOperation: Body Count Shareware Decoding\n" );

//	printf( "\n\nOperation: Body Count Shareware [Work in Progress]\n" );

}

