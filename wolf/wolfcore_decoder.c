/*

	Copyright (C) 2006-2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file wolfcore_decoder.c
 * \brief Wolfenstein 3-D front end to data file decoder.
 * \author Michael Liebscher
 * \date 2006-2013
 */

#include "wolfcore_decoder.h"
#include <zlib.h>

#include "../common/platform.h"
#include "../common/common_utils.h"
#include "../string/wtstring.h"
#include "../filesys/file.h"
#include "../memory/memory.h"

#include "wolfenstein/wolf.h"


PRIVATE const char *BASEDIR = "base/";


extern W32 _gameVersion;


W32 wolf_version = 0;


//	File extensions for various Wolfenstein 3-D powered games.
//	Order must match the order of WDExtFlags.
const char *WPFileExt[] =
{
	"*.WL6",	/* Wolfenstein 3-D */
	"*.WL1",	/* Wolfenstein 3-D Shareware */
	"*.REZ",	/* Wolfenstein 3-D for Macintosh */
	"*.3DO",	/* Wolfenstein 3-D for 3D0 */
	"*.JAG",	/* Wolfenstein 3-D for Jaguar */
	"*.SOD",	/* Spear of Destiny */
	"*.SDM",	/* Spear of Destiny Demo */
	"*.BS6",	/* Blake Stone: Aliens of Gold */
	"*.BS1",	/* Blake Stone: Aliens of Gold Shareware */
	"*.VSI",	/* Blake Stone: Planet Strike */
	"*.CO7",	/* Corridor 7 */
	"*.DMO",	/* Corridor 7 Shareware */
	"*.N3D",	/* Super 3D Noah's Ark */
	"*.BC",		/* Operation Body Count */
	"*.BC",		/* Operation Body Count Shareware */

	NULL		/* Must be Last */

};




typedef struct dataDecoder_s
{
	void (*decode)( void );

} dataDecoder_t;


extern void wolffull_decoder( void );
extern void wolfshare_decoder( void );
extern void Macintosh_Decoder( void );
extern void wolf3do_decoder( void );
extern void wolf_jaguar_decoder( void );
extern void spear_decoder( void );
extern void speardemo_decoder( void );
extern void blakestoneAGfull_decoder( void );
extern void blakestoneAGshare_decoder( void );
extern void blakestonePS_decoder( void );
extern void corridor7_decoder( void );
extern void corridor7share_decoder( void );
extern void super3dNoahsArk_decoder( void );
extern void obc_decoder( void );
extern void obcshare_decoder( void );


//	File decoders for various Wolfenstein 3-D powered games.
//	Order must match the order of WDExtFlags.
PRIVATE dataDecoder_t dd_decoder[] =
{
	{ wolffull_decoder },
	{ wolfshare_decoder },
	{ Macintosh_Decoder },
	{ wolf3do_decoder },
	{ wolf_jaguar_decoder },
	{ spear_decoder },
	{ speardemo_decoder },
	{ blakestoneAGfull_decoder },
	{ blakestoneAGshare_decoder },
	{ blakestonePS_decoder },
	{ corridor7_decoder },
	{ corridor7share_decoder },
	{ super3dNoahsArk_decoder },
	{ obc_decoder },
	{ obcshare_decoder },

	{ NULL }		/* Must be last */
};


PRIVATE W32 ddcodemax = sizeof( dd_decoder ) / sizeof( dd_decoder[ 0 ] );


/**
 * \brief Create cache directories.
 * \return On success true, otherwise false.
 */
PUBLIC wtBoolean buildCacheDirectories( void )
{
	if( ! FS_CreateDirectory( DIR_PICS ) )
		return false;

	if( ! FS_CreateDirectory( DIR_WALLS ) )
		return false;

	if( ! FS_CreateDirectory( DIR_SPRITES ) )
		return false;

	if( ! FS_CreateDirectory( DIR_DSOUND ) )
		return false;

	if( ! FS_CreateDirectory( DIR_SOUNDFX ) )
		return false;

	if( ! FS_CreateDirectory( DIR_MUSIC ) )
		return false;

	if( ! FS_CreateDirectory( DIR_MAPS ) )
		return false;

	if( ! FS_CreateDirectory( DIR_GSCRIPTS ) )
		return false;

	if( ! FS_CreateDirectory( DIR_SOD_SPRITES ) )
		return false;

	if( ! FS_CreateDirectory( DIR_SOD_SOUNDFX ) )
		return false;

	if( ! FS_CreateDirectory( DIR_SOD_DSOUND ) )
		return false;

	return true;
}


/**
 * \brief Remove cache directories.
 */
PUBLIC void deleteCacheDirectories( void )
{
	if( ! FS_RemoveDirectory( DIR_PICS ) )
		fprintf( stderr, "Unable to remove directory (%s)\n", DIR_PICS );

	if( ! FS_RemoveDirectory( DIR_WALLS ) )
		fprintf( stderr, "Unable to remove directory (%s)\n", DIR_WALLS );

	if( ! FS_RemoveDirectory( DIR_SPRITES ) )
		fprintf( stderr, "Unable to remove directory (%s)\n", DIR_SPRITES );

	if( ! FS_RemoveDirectory( DIR_DSOUND ) )
		fprintf( stderr, "Unable to remove directory (%s)\n", DIR_DSOUND );

	if( ! FS_RemoveDirectory( DIR_SOUNDFX ) )
		fprintf( stderr, "Unable to remove directory (%s)\n", DIR_SOUNDFX );

	if( ! FS_RemoveDirectory( DIR_MUSIC ) )
		fprintf( stderr, "Unable to remove directory (%s)\n", DIR_MUSIC );

	if( ! FS_RemoveDirectory( DIR_MAPS ) )
		fprintf( stderr, "Unable to remove directory (%s)\n", DIR_MAPS );
}


#define CRC_AUDIOHED_WL6_ACTIVISION     0x2F3BCB70;
#define CRC_AUDIOT_WL6_ACTIVISION       0xC79EFD4C;
#define CRC_GAMEMAPS_WL6_ACTIVISION     0xADA5C827;
#define CRC_MAPHEAD_WL6_ACTIVISION      0x15803E9;
#define CRC_VGADICT_WL6_ACTIVISION      0xAA22054E;
#define CRC_VGAGRAPH_WL6_ACTIVISION     0xA4B1DE1D;
#define CRC_VGAHEAD_WL6_ACTIVISION      0xAB2B44D3;
#define CRC_VSWAP_WL6_ACTIVISION        0xE610C664;


#define CRC_AUDIOHED_WL6_APOGEE     0x2F3BCB70;
#define CRC_AUDIOT_WL6_APOGEE       0xC79EFD4C;
#define CRC_GAMEMAPS_WL6_APOGEE     0xADA5C827;
#define CRC_MAPHEAD_WL6_APOGEE      0x15803E9;
#define CRC_VGADICT_WL6_APOGEE      0x8B4AD0F8;
#define CRC_VGAGRAPH_WL6_APOGEE     0xE0DF1EA0;
#define CRC_VGAHEAD_WL6_APOGEE      0x27EE37C4;
#define CRC_VSWAP_WL6_APOGEE        0x859E8FFC;

typedef struct fileCRC_LUT_s {

	char *filename;
	W32 crc;

} fileCRC_LUT_t;

const fileCRC_LUT_t ACTIVISION_WL6_LUT[] =
{
	{ "AUDIOHED.WL6",   0x2F3BCB70 },
	{ "AUDIOT.WL6",     0xC79EFD4C },
	{ "GAMEMAPS.WL6",   0xADA5C827 },
	{ "MAPHEAD.WL6",    0x015803E9 },
	{ "VGADICT.WL6",    0xAA22054E },
	{ "VGAGRAPH.WL6",   0xA4B1DE1D },
	{ "VGAHEAD.WL6",    0xAB2B44D3 },
	{ "VSWAP.WL6",      0xE610C664 },

	{ NULL, 0 }
};

const fileCRC_LUT_t APOGEE_WL6_LUT[] =
{
	{ "AUDIOHED.WL6",   0x2F3BCB70 },
	{ "AUDIOT.WL6",     0xC79EFD4C },
	{ "GAMEMAPS.WL6",   0xADA5C827 },
	{ "MAPHEAD.WL6",    0x015803E9 },
	{ "VGADICT.WL6",    0x8B4AD0F8 },
	{ "VGAGRAPH.WL6",   0xE0DF1EA0 },
	{ "VGAHEAD.WL6",    0x27EE37C4 },
	{ "VSWAP.WL6",      0x859E8FFC },

	{ NULL, 0 }
};

const fileCRC_LUT_t ID_SOFTWARE_V14_WL6_LUT[] =
{
	{ "AUDIOHED.WL6",   0x2F3BCB70 },
	{ "AUDIOT.WL6",     0xC79EFD4C },
	{ "GAMEMAPS.WL6",   0xADA5C827 },
	{ "MAPHEAD.WL6",    0x015803E9 },
	{ "VGADICT.WL6",    0x8B4AD0F8 },
	{ "VGAGRAPH.WL6",   0xE0DF1EA0 },
	{ "VGAHEAD.WL6",    0x27EE37C4 },
	{ "VSWAP.WL6",      0x859E8FFC },

	{ NULL, 0 }
};

const fileCRC_LUT_t SHARE_V10_WL1_LUT[] =
{
	{ "AUDIOHED.WL1",   0x384A9496 },
	{ "AUDIOT.WL1",     0xF65031A7 },
	{ "MAPTEMP.WL1",    0xF323BCE2 },
	{ "MAPHEAD.WL1",    0xE35F606A },
	{ "VGADICT.WL1",    0x3796B7E2 },
	{ "VGAGRAPH.WL1",   0xF431F4A4 },
	{ "VGAHEAD.WL1",    0x1343524F },
	{ "VSWAP.WL1",      0xF97FE230 },

	{ NULL, 0 }
};

const fileCRC_LUT_t SHARE_V11_WL1_LUT[] =
{
	{ "AUDIOHED.WL1",   0x52133DC4 },
	{ "AUDIOT.WL1",     0x73569F4D },
	{ "GAMEMAPS.WL1",   0x7750E1D4 },
	{ "MAPHEAD.WL1",    0x205253AC },
	{ "VGADICT.WL1",    0xFCAD3538 },
	{ "VGAGRAPH.WL1",   0x9CB49FEA },
	{ "VGAHEAD.WL1",    0xD3B22C3C },
	{ "VSWAP.WL1",      0xBB85F24C },

	{ NULL, 0 }
};

const fileCRC_LUT_t SHARE_V14_WL1_LUT[] =
{
	{ "AUDIOHED.WL1",   0x52133DC4 },
	{ "AUDIOT.WL1",     0x87BFC399 },
	{ "GAMEMAPS.WL1",   0xCC53D341 },
	{ "MAPHEAD.WL1",    0x088F7551 },
	{ "VGADICT.WL1",    0x358AB877 },
	{ "VGAGRAPH.WL1",   0x746DA9F0 },
	{ "VGAHEAD.WL1",    0xA60A98F0 },
	{ "VSWAP.WL1",      0x85EFAC55 },

	{ NULL, 0 }
};

#define WL6_FILE_COUNT  8




PRIVATE void CheckFiles_WolfensteinFull( )
{
	W32 crc;
	void *buffer;
	int i;
	SW32 length;
	W32 countActivision = 0;
	W32 countApogee = 0;
	W32 countId_V14 = 0;


	for( i = 0 ; i < WL6_FILE_COUNT ; i++ )
	{
		length = FS_FileLoad( ACTIVISION_WL6_LUT[ i ].filename, &buffer );
		if( length < 0 )
			continue;

		crc = crc32( 0L, buffer, length );
		if( crc == ACTIVISION_WL6_LUT[ i ].crc  ) {
			countActivision++;
		}
		if( crc == APOGEE_WL6_LUT[ i ].crc  ) {
			countApogee++;
		}

		MM_FREE( buffer );
	}

	if( countActivision == WL6_FILE_COUNT ) {
		wolf_version = ACTIVISION_WL6_V14;
	}
	if( countApogee == WL6_FILE_COUNT ) {
		wolf_version = APOGEE_WL6_V14G;
	}
}

PRIVATE void CheckFiles_WolfensteinShare( )
{
	W32 crc;
	void *buffer;
	int i;
	SW32 length;
	W32 count_V10 = 0;
	W32 count_V11 = 0;
	W32 count_V14 = 0;

	for( i = 0 ; i < WL6_FILE_COUNT ; i++ )
	{
		length = FS_FileLoad( SHARE_V10_WL1_LUT[ i ].filename, &buffer );
		if( length < 0 ) {
			length = FS_FileLoad( SHARE_V11_WL1_LUT[ i ].filename, &buffer );
			if( length < 0 ){
				continue;
			}
		}

		crc = crc32( 0L, buffer, length );
		if( crc == SHARE_V10_WL1_LUT[ i ].crc  )
		{
			count_V10++;
		}
		if( crc == SHARE_V11_WL1_LUT[ i ].crc  )
		{
			count_V11++;
		}
		if ( crc == SHARE_V14_WL1_LUT[ i ].crc )
		{
			count_V14++;
		}
		MM_FREE( buffer );
	}

	if( count_V10 == WL6_FILE_COUNT )
	{
		wolf_version = WL1_V10;
	}
	else if( count_V11 == WL6_FILE_COUNT )
	{
		wolf_version = WL1_V11;
	}
	else if( count_V14 == WL6_FILE_COUNT  )
	{
		wolf_version = WL1_V14;
	}
}

/**
 * Check which version of SOD this is.
 *
 * @FIXME STUB.
 */
PRIVATE void CheckFiles_SOD( )
{
	SW32 retValue;
	W32 crc;
	void *buffer;
	int i;
	W32 flag;


	wolf_version = SPEAR_OF_DESTINY;

// Figure out which version this is: SOD, Return to Danger, or Ultimate Challenge


#define CRC_VSWAP_SOD       0x591CC4DC;

#define CRC_VSWAP_SD1       0x591CC4DC;
#define CRC_VSWAP_SD2       0x861DCBF7;
#define CRC_VSWAP_SD3       0x1DB521A;


}


/**
 * \brief Check files for integrity.
 * \param[in,out] flag Flag value returned from CheckForDataFiles
 * \return Nothing.
 * \note Must call CheckForDataFiles() before calling this method.
 */
PRIVATE void CheckFilesForIntegrity( W32 *flag )
{
	if( *flag & FND_WOLF_FULL  ) {
		CheckFiles_WolfensteinFull();
	} else if( *flag & FND_WOLF_SHARE ) {
		CheckFiles_WolfensteinShare();
	} else if( *flag & FND_SPEAR_FULL ) {
		CheckFiles_SOD();
	}
}

/**
 * \brief Check to see which data files are present.
 * \param[in,out] flag Zero nothing found. See WDExtFlags for more details.
 * \return Nothing.
 */
PRIVATE void CheckForDataFiles( W32 *flag )
{
	char ext[ 13 ];
	W32 i;

	*flag = 0;

	i = 0;
	while( WPFileExt[ i ] )
	{
		wt_strlcpy( ext, WPFileExt[ i ], sizeof( ext ) );


		/* check for upper case */
		if( FS_FindFirst( wt_strupr( ext ) ) )
		{
			*flag |= BIT( i );
		}
		FS_FindClose();


		/* check for lower case */
		if( FS_FindFirst( wt_strlwr( ext ) ) )
		{
			*flag |= BIT( i );
		}
		FS_FindClose();

		++i;
	}

}

/**
 * \brief Wolfenstein data decoder.
 * \return Nothing.
 * \note 1. Change into base directory.
 *       2. Look for data files.
 *       3. Decode data files accordingly.
 */
PUBLIC void wolfDataDecipher( void )
{
	W32 wolfExt_Flag = 0;
	W32 temp;
	W32 i;


	/* Change into base directory */
	if( ! FS_ChangeCurrentDirectory( BASEDIR ) )
	{
		printf( "Unable to change into directory (%s)\n", BASEDIR );

		return;
	}

	/* Look for data files in the base directory */
	CheckForDataFiles( &wolfExt_Flag );


	if( wolfExt_Flag == 0 )
	{
		printf( "No data files found!\n" );

		return;
	}


	CheckFilesForIntegrity( &wolfExt_Flag );


	/* Decode the data files */
	for( i = 0 ; i < 32 ; ++i )
	{
		temp = (wolfExt_Flag >> i) & 0x1;

		if( temp == 0 || i >= ddcodemax )
		{
			continue;
		}

		 dd_decoder[ i ].decode();
	}

}
