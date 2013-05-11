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
 * \file spear.c
 * \brief Spear of Destiny data file decoder.
 * \author Michael Liebscher 
 * \date 2007-2013 
 */

#include <stdio.h>
#include <string.h>

#include "spear_def.h"

#include "../core/wolfcore.h"
#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../filesys/file.h"
#include "../../string/wtstring.h"
#include "../../memory/memory.h"
#include "../../loaders/tga.h"

#include "../../image/image.h"
#include "../../image/hq2x.h"
#include "../../pak/pak.h"
#include "../wolfcore_decoder.h"

#include "../wolfenstein/wolf.h"


extern W8 spear_gamepal[];
extern W32 _filterScale;
extern W32 _filterScale_Sprites;
extern wtBoolean _outputInDirectory;


wtBoolean bRedux = true;

extern W32 wolf_version;


PRIVATE const W32 CeilingColourSOD[] =
{
	0x6f6f,0x4f4f,0x1d1d,0xdede,0xdfdf,0x2e2e,0x7f7f,0x9e9e,0xaeae,0x7f7f,
	0x1d1d,0xdede,0xdfdf,0xdede,0xdfdf,0xdede,0xe1e1,0xdcdc,0x2e2e,0x1d1d,0xdcdc
};

PRIVATE char *SOD_songs[] =
{	
    "SOD_XFUNKIE",
    "SOD_DUNGEON",
    "SOD_XDEATH",
    "SOD_GETTHEM",
    "SOD_XTIPTOE",
    "SOD_GOINGAFT",
    "SOD_URAHERO",
    "SOD_XTHEEND",
    "SOD_NAZI_OMI",
    "SOD_POW",
    "SOD_TWELFTH",
    "SOD_SEARCHN",
    "SOD_SUSPENSE",
    "SOD_ZEROHOUR",
    "SOD_WONDERIN",
    "SOD_ULTIMATE",
    "SOD_ENDLEVEL",
    "SOD_XEVIL",
    "SOD_XJAZNAZI",
    "SOD_COPYPRO",
    "SOD_XAWARD",
    "SOD_XPUTIT",
    "SOD_XGETYOU",
    "SOD_XTOWER2",

    NULL
};


PRIVATE parTimes_t parTimesSOD[] =	
{ 
	 //
	 // SPEAR OF DESTINY TIMES
	 //
	 { 1.5,	"01:30" },
	 { 3.5,	"03:30" },
	 { 2.75,"02:45" },
	 { 3.5,	"03:30" },
	 { 0,	"??:??" },	// Boss 1
	 { 4.5,	"04:30" },
	 { 3.25,"03:15" },
	 { 2.75,"02:45" },
	 { 4.75,"04:45" },
	 { 0,	"??:??" },	// Boss 2
	 { 6.5,	"06:30" },
	 { 4.5,	"04:30" },
	 { 2.75,"02:45" },
	 { 4.5,	"04:30" },
	 { 6,	"06:00" },
	 { 0,	"??:??" },	// Boss 3
	 { 6,	"06:00" },
	 { 0,	"??:??" },	// Boss 4
	 { 0,	"??:??" },	// Secret level 1
	 { 0,	"??:??" },	// Secret level 2

};


/**
 * \brief Converts SOD graphic data.
 * \param[in] dict Name of dictionary file to load.
 * \param[in] head Name of header file to load. 
 * \param[in] graph Name of graphics file to load. 
 * \param[in] start Offset to start of sprite data
 * \param[in] end Offset to end of sprite data
 * \param[in] picNum Pointer to picNum_t structure to 
 * \param[in] GetReduxGFXFileName Call back function to get file names for graphics data.
 * \return On success true, otherwise false.
 */
PRIVATE wtBoolean spear_gfx( char *dict, char *head, char *graph, W32 start, W32 end, picNum_t *picNum, char *(*GetReduxGFXFileName)( W32 ) )
{

	wtBoolean bRetVal;
	W32 width, height;
	void *data;
	W32 i;
	char tempFileName[ 1024 ];

	
		
	bRetVal = GFXFile_Setup( dict, head, graph );
    if( ! bRetVal )
	{
        return false;
    }

	for( i = 1; i < start; ++i )
	{			
		GFXFile_decodeFont( i, 256, 128, DIR_PICS );
	}


	GFXFile_decodeScript( picNum->PN_ScriptStart, picNum->PN_ScriptEnd, DIR_GSCRIPTS );


	for( i = start ; i < end ; ++i )
	{
		GFXFile_cacheChunk( i );
        data = GFXFile_decodeChunk_RGB32( i, &width, &height, spear_gamepal );
		if( NULL == data )
		{
            continue;
        }

        if( bRedux )
        {
        	W32 id;
        	void *updata;
        	
        	id = 0;
            updata = wolfcore_ReduxGFX( i, data, &width, &height, &id, spear_gamepal, picNum );
            if( updata == NULL )
            {
            	MM_FREE( data );
            	
            	continue;
            }
            
            wt_snprintf( tempFileName, sizeof( tempFileName ), "%s%c%s.tga", DIR_PICS, PATH_SEP, GetReduxGFXFileName( i ) );
			
            RGB32toRGB24( updata, updata, width * height * 4 ); 
			TGA_write( tempFileName, 24, width, height, updata, 0, 1 );
		

            // updata and data could point to the same memory block.
            if( updata == data)
            {
			    MM_FREE( data );
            }
            else
            {
                MM_FREE( data );
                MM_FREE( updata );
            }
			
			i += id;                        
        }
		else
		{		
            wt_snprintf( tempFileName, sizeof( tempFileName ), "%s%c%.3d.tga", DIR_PICS, PATH_SEP, i );
			
            RGB32toRGB24( data, data, width * height * 4 ); 
			TGA_write( tempFileName, 24, width, height, data, 0, 1 );
		
			MM_FREE( data );
		}
		
	}		
	
	GFXFile_Shutdown();
	
	return bRetVal;
}

/**
 * \brief Decodes SOD full version. 
 * \return Nothing.
 */
PUBLIC void spear_decoder( void )
{	
	
    picNum_t picNum;
	W32 retCheck = 0;

    wolf_version = SPEAR_OF_DESTINY;

	printf( "Spear of Destiny Decoding\n\n" );
	
	if( ! buildCacheDirectories() )
    {
        fprintf( stderr, "Unable to create cache directories\n" );

		return;
    }
	
	picNum.PN_StatusBar = SOD_STATUSBARPIC;
    picNum.PN_NoKey = SOD_NOKEYPIC;
    picNum.PN_Blank = SOD_N_BLANKPIC;
    picNum.PN_Title1 = SOD_TITLE1PIC;
    picNum.PN_Title2 = SOD_TITLE2PIC;
    picNum.PN_TitlePalette = SOD_TITLEPALETTE;
    picNum.PN_0 = SOD_N_0PIC;
    picNum.PN_1 = SOD_N_1PIC;
    picNum.PN_9 = SOD_N_9PIC;
    picNum.PN_Colon = SOD_L_COLONPIC;
    picNum.PN_Num0 = SOD_L_NUM0PIC;
    picNum.PN_Num9 = SOD_L_NUM9PIC;
    picNum.PN_Percent = SOD_L_PERCENTPIC;
    picNum.PN_A = SOD_L_APIC;
    picNum.PN_Z = SOD_L_ZPIC;
    picNum.PN_Expoint = SOD_L_EXPOINTPIC;
    picNum.PN_Apostrophe = SOD_L_APOSTROPHEPIC;
    picNum.PN_IDGuys1 = SOD_IDGUYS1PIC;
    picNum.PN_IDGuys2 = SOD_IDGUYS2PIC;
    picNum.PN_EndScreen1 = SOD_ENDSCREEN1PIC;
    picNum.PN_EndScreen9 = SOD_ENDSCREEN9PIC;
	picNum.PN_HelpScript = 0;
	picNum.PN_ScriptStart = 168;
    picNum.PN_ScriptEnd = 169;
	
	
	retCheck += spear_gfx( "VGADICT.SOD", "VGAHEAD.SOD", "VGAGRAPH.SOD", 3, 149, &picNum, SOD_GetReduxGFXFileName );
	
	retCheck += PageFile_ReduxDecodePageData( "VSWAP.SOD", DIR_WALLS, DIR_SOD_SPRITES, DIR_SOD_DSOUND, spear_gamepal );
	
	if( bRedux )
    {
    	retCheck += MapFile_ReduxDecodeMapData( "MAPHEAD.SOD", "GAMEMAPS.SOD", DIR_MAPS, spear_gamepal, CeilingColourSOD, SOD_songs, parTimesSOD, "%s/s%.2d.map" );  
    }
	
	if( AudioFile_Setup( "AUDIOHED.SOD", "AUDIOT.SOD" ) )
	{
		retCheck += AudioFile_ReduxDecodeSound( 81, 162, DIR_SOD_SOUNDFX );

		retCheck += AudioFile_ReduxDecodeMusic( 243, 243+SOD_LASTMUSIC, DIR_MUSIC, SOD_songs );
	}
	AudioFile_Shutdown();


	if( 5 == retCheck && ! _outputInDirectory )
    {
    	PAK_builder( "spear.pak", 1, false );
    }

    // Check for SD1
    FS_FindClose();
    if( FS_FindFirst( "VSWAP.SD1" ) != NULL )
    {
        FS_FindClose();

        printf( "\n\n...SD1 Decoding\n" );
        
        MapFile_ReduxDecodeMapData( "MAPHEAD.SD1", "GAMEMAPS.SD1", DIR_MAPS, spear_gamepal, CeilingColourSOD, SOD_songs, parTimesSOD, "%s/s%.2d.map" );  
        PageFile_ReduxDecodePageData( "VSWAP.SD1", DIR_WALLS, DIR_SOD_SPRITES, DIR_SOD_DSOUND, spear_gamepal );

        PAK_builder( "spear_sd1.pak", 1, false );
    }
    // Check for SD2
    FS_FindClose();
    if( FS_FindFirst( "VSWAP.SD2" ) != NULL )
    {
        FS_FindClose();

        printf( "\n\n...SD2 Decoding\n" );

        MapFile_ReduxDecodeMapData( "MAPHEAD.SD2", "GAMEMAPS.SD2", DIR_MAPS, spear_gamepal, CeilingColourSOD, SOD_songs, parTimesSOD, "%s/s%.2d.map" );  
        PageFile_ReduxDecodePageData( "VSWAP.SD2", DIR_WALLS, DIR_SOD_SPRITES, DIR_SOD_DSOUND, spear_gamepal );

        PAK_builder( "spear_sd2.pak", 1, false );
    }

    // Check for SD3
    FS_FindClose();
    if( FS_FindFirst( "VSWAP.SD3" ) != NULL )
    {
        FS_FindClose();

        printf( "\n\n...SD3 Decoding\n" );

        MapFile_ReduxDecodeMapData( "MAPHEAD.SD3", "GAMEMAPS.SD3", DIR_MAPS, spear_gamepal, CeilingColourSOD, SOD_songs, parTimesSOD, "%s/s%.2d.map" );  
        PageFile_ReduxDecodePageData( "VSWAP.SD3", DIR_WALLS, DIR_SOD_SPRITES, DIR_SOD_DSOUND, spear_gamepal );

        PAK_builder( "spear_sd3.pak", 1, false );
    }
}

/**
 * \brief Decodes SOD Shareware version. 
 * \return Nothing.
 */
PUBLIC void speardemo_decoder( void )
{	
	picNum_t picNum;
	W32 retCheck = 0;

    wolf_version = SPEAR_OF_DESTINY_DEMO;
    
	printf( "Spear of Destiny Demo Decoding\n\n" );

	if( ! buildCacheDirectories() )
    {
        fprintf( stderr, "Unable to create cache directories\n" );

		return;
    }
	
	picNum.PN_StatusBar = SDM_STATUSBARPIC;
    picNum.PN_NoKey = SDM_NOKEYPIC;
    picNum.PN_Blank = SDM_N_BLANKPIC;
    picNum.PN_Title1 = SDM_TITLE1PIC;
    picNum.PN_Title2 = SDM_TITLE2PIC;
    picNum.PN_TitlePalette = SDM_TITLEPALETTE;
    picNum.PN_0 = SDM_N_0PIC;
    picNum.PN_1 = SDM_N_1PIC;
    picNum.PN_9 = SDM_N_9PIC;
    picNum.PN_Colon = SDM_L_COLONPIC;
    picNum.PN_Num0 = SDM_L_NUM0PIC;
    picNum.PN_Num9 = SDM_L_NUM9PIC;
    picNum.PN_Percent = SDM_L_PERCENTPIC;
    picNum.PN_A = SDM_L_APIC;
    picNum.PN_Z = SDM_L_ZPIC;
    picNum.PN_Expoint = SDM_L_EXPOINTPIC;
    picNum.PN_Apostrophe = SDM_L_APOSTROPHEPIC;
    picNum.PN_IDGuys1 = 999;
    picNum.PN_IDGuys2 = 999;
    picNum.PN_EndScreen1 = 999;
    picNum.PN_EndScreen9 = 999;
	picNum.PN_HelpScript = 0;
	picNum.PN_ScriptStart = 0;
    picNum.PN_ScriptEnd = 0;
	
	retCheck += spear_gfx( "VGADICT.SDM", "VGAHEAD.SDM", "VGAGRAPH.SDM", 3, 127, &picNum, SDM_GetReduxGFXFileName );
	
	
	retCheck += PageFile_ReduxDecodePageData( "VSWAP.SDM", DIR_WALLS, DIR_SPRITES, DIR_DSOUND, spear_gamepal );
	
	if( bRedux )
    {
    	retCheck += MapFile_ReduxDecodeMapData( "MAPHEAD.SDM", "GAMEMAPS.SDM", DIR_MAPS, spear_gamepal, CeilingColourSOD, SOD_songs, parTimesSOD, "%s/s%.2d.map" );  
    }
	
	if( AudioFile_Setup( "AUDIOHED.SDM", "AUDIOT.SDM" ) )
	{
		retCheck += AudioFile_ReduxDecodeSound( 81, 162, DIR_SOUNDFX );

		retCheck += AudioFile_ReduxDecodeMusic( 243, 243 + SOD_LASTMUSIC, DIR_MUSIC, NULL );
	}
	AudioFile_Shutdown();


	if( 5 == retCheck && ! _outputInDirectory )
    {
    	PAK_builder( "speardmo.pak", 1, true );
    }

}

