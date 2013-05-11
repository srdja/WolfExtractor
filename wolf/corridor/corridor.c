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
 * \file corridor.c
 * \brief  Corridor 7 data file decoder.
 * \author Michael Liebscher 
 * \date 2007-2013 
 */

#include <stdio.h>

#include "corridor.h"

#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../filesys/file.h"
#include "../core/wolfcore.h"

#include "../../string/wtstring.h"
#include "../../memory/memory.h"
#include "../../loaders/tga.h"
#include "../wolfcore_decoder.h"


extern W8 corridor_gamepal[];
extern W32 _filterScale;

/**
 * \brief Decodes Corridor 7 data 
 * \return Nothing.
 */
PUBLIC void corridor7_decoder( void )
{
	printf( "Corridor 7 Alien Invasion Decoding\n\n" );

	if( ! buildCacheDirectories() )
    {
        fprintf( stderr, "Unable to create cache directories\n" );

		return;
    }
	
	if( GFXFile_Setup( "VGADICT.CO7", "VGAHEAD.CO7", "VGAGRAPH.CO7" ) )
	{
		GFXFile_decodeFont( 1, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 2, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 3, 256, 128, DIR_PICS );

        GFXFile_decodeGFX( 4, 58, corridor_gamepal, DIR_PICS );
	}
	GFXFile_Shutdown();

	

	PageFile_ReduxDecodePageData( "GFXTILES.CO7", DIR_WALLS, DIR_SPRITES, DIR_DSOUND, corridor_gamepal );

/*
	if( AudioFile_Setup( "AUDIOHED.CO7", "AUDIOT.CO7" ) )
	{
		AudioFile_ReduxDecodeSound( 0, 0, DIR_SOUNDFX );

		AudioFile_ReduxDecodeMusic( 0, 0, DIR_MUSIC, NULL );
	}
	AudioFile_Shutdown();
*/
}

/**
 * \brief Decodes Corridor 7 Shareware data 
 * \return Nothing.
 */
PUBLIC void corridor7share_decoder( void )
{
	printf( "Corridor 7 Alien Invasion Shareware Decoding\n\n" );

	if( ! buildCacheDirectories() )
    {
        fprintf( stderr, "Unable to create cache directories\n" );

		return;
    }
	
	if( GFXFile_Setup( "VGADICT.DMO", "VGAHEAD.DMO", "VGAGRAPH.DMO" ) )
	{
		GFXFile_decodeFont( 1, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 2, 256, 128, DIR_PICS );
		GFXFile_decodeFont( 3, 256, 128, DIR_PICS );

		GFXFile_decodeGFX( 4, 42, corridor_gamepal, DIR_PICS );
	}
	GFXFile_Shutdown();

	PageFile_ReduxDecodePageData( "GFXTILES.DMO", DIR_WALLS, DIR_SPRITES, DIR_DSOUND, corridor_gamepal );


/*
	if( AudioFile_Setup( "AUDIOHED.DMO", "AUDIOT.DMO" ) )
	{
		AudioFile_ReduxDecodeSound( 0, 0, DIR_SOUNDFX );

		AudioFile_ReduxDecodeMusic( 0, 0, DIR_MUSIC, NULL );
	}
	AudioFile_Shutdown();
*/
}
