/*

	Copyright (C) 2013 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file jaguar.c
 * \brief Atari Jaguar Wolfenstein data file decoder.
 * \author Michael Liebscher
 * \date 2013
 * \note Portion of this code was derived from DOOM, and was originally written by Id Software, Inc.
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <malloc.h>

#include "../../common/platform.h"
#include "../../memory/memory.h"
#include "../../wolf/mac/mac.h"
#include "../../loaders/wav.h"
#include "../../common/common_utils.h"
#include "../../loaders/tga.h"
#include "../../string/wtstring.h"
#include "../../filesys/file.h"

#include "jaguar.h"


#define PATH_MAPS       "jag_maps"
#define PATH_SPRITES    "jag_sprites"
#define PATH_WALLS      "jag_walls"
#define PATH_HUD        "jag_hud"
#define PATH_LABELS     "jag_labels"
#define PATH_SCREENS    "jag_screens"
#define PATH_SOUNDS     "jag_sounds"
#define PATH_MUSIC      "jag_music"

const char *jag_File_EXT[] =
{
    "*.J64",
    "*.JAG",

    NULL			/* Must be Last */
};

W32 ROM_WAD_OFFSET = 0x20000;


PRIVATE W8 *palette;


PRIVATE W8 redLUT[16][16] = {
   //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},    // 0
	{  34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 19, 0},    // 1
	{  68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 64, 43, 21, 0},    // 2
	{  102,102,102,102,102,102,102,102,102,102,102,95, 71, 47, 23, 0},    // 3
	{  135,135,135,135,135,135,135,135,135,135,130,104,78, 52, 26, 0},    // 4
	{  169,169,169,169,169,169,169,169,169,170,141,113,85, 56, 28, 0},    // 5
	{  203,203,203,203,203,203,203,203,203,183,153,122,91, 61, 30, 0},    // 6
	{  237,237,237,237,237,237,237,237,230,197,164,131,98, 65, 32, 0},    // 7
	{  255,255,255,255,255,255,255,255,247,214,181,148,15, 82, 49, 7},    // 8
	{  255,255,255,255,255,255,255,255,255,235,204,173,143,112,81, 51},   // 9
	{  255,255,255,255,255,255,255,255,255,255,227,198,170,141,113,85},   // A
	{  255,255,255,255,255,255,255,255,255,255,249,223,197,171,145,119},  // B
	{  255,255,255,255,255,255,255,255,255,255,255,248,224,200,177,153},  // C
	{  255,255,255,255,255,255,255,255,255,255,255,255,252,230,208,187},  // D
	{  255,255,255,255,255,255,255,255,255,255,255,255,255,255,240,221},  // E
	{  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255}   // F
};

PRIVATE W8 greenLUT[16][16] = {
   //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	{  0,  17, 34, 51,68, 85, 102,119,136,153,170,187,204,221,238,255},   // 0
	{  0,  19, 38, 57,77, 96, 115,134,154,173,192,211,231,250,255,255},   // 1
	{  0,  21, 43, 64,86, 107,129,150,172,193,215,236,255,255,255,255},   // 2
	{  0,  23, 47, 71,95, 119,142,166,190,214,238,255,255,255,255,255},   // 3
	{  0,  26, 52, 78,104,130,156,182,208,234,255,255,255,255,255,255},   // 4
	{  0,  28, 56, 85,113,141,170,198,226,255,255,255,255,255,255,255},   // 5
	{  0,  30, 61, 91,122,153,183,214,244,255,255,255,255,255,255,255},   // 6
	{  0,  32, 65, 98,131,164,197,230,255,255,255,255,255,255,255,255},   // 7
	{  0,  32, 65, 98,131,164,197,230,255,255,255,255,255,255,255,255},   // 8
	{  0,  30, 61, 91,122,153,183,214,244,255,255,255,255,255,255,255},   // 9
	{  0,  28, 56, 85,113,141,170,198,226,255,255,255,255,255,255,255},   // A
	{  0,  26, 52, 78,104,130,156,182,208,234,255,255,255,255,255,255},   // B
	{  0,  23, 47, 71,95, 119,142,166,190,214,238,255,255,255,255,255},   // C
	{  0,  21, 43, 64,86, 107,129,150,172,193,215,236,255,255,255,255},   // D
	{  0,  19, 38, 57,77, 96, 115,134,154,173,192,211,231,250,255,255},   // E
	{  0,  17, 34, 51,68, 85, 102,119,136,153,170,187,204,221,238,255}    // F
};

PRIVATE W8 blueLUT[16][16] = {
   //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	{  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},  // 0
	{  255,255,255,255,255,255,255,255,255,255,255,255,255,255,240,221},  // 1
	{  255,255,255,255,255,255,255,255,255,255,255,255,252,230,208,187},  // 2
	{  255,255,255,255,255,255,255,255,255,255,255,248,224,200,177,153},  // 3
	{  255,255,255,255,255,255,255,255,255,255,249,223,197,171,145,119},  // 4
	{  255,255,255,255,255,255,255,255,255,255,227,198,170,141,113,85},   // 5
	{  255,255,255,255,255,255,255,255,255,235,204,173,143,112,81, 51},   // 6
	{  255,255,255,255,255,255,255,255,247,214,181,148,115,82, 49, 17},   // 7
	{  237,237,237,237,237,237,237,237,230,197,164,131,98, 65, 32, 0},    // 8
	{  203,203,203,203,203,203,203,203,203,183,153,122,91, 61, 30, 0},    // 9
	{  169,169,169,169,169,169,169,169,169,170,141,113,85, 56, 28, 0},    // A
	{  135,135,135,135,135,135,135,135,135,135,130,104,78, 52, 26, 0},    // B
	{  102,102,102,102,102,102,102,102,102,102,102,95, 71, 47, 23, 0},    // C
	{  68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 64, 43, 21, 0},    // D
	{  34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 19, 0},    // E
	{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}     // F
};



//
// TYPES
//
typedef struct
{
    // Should be "IWAD" or "PWAD".
    char identification[4];
    int	numlumps;
    int	infotableofs;

} wadinfo_t;


typedef struct
{
    int	filepos;
    int	size;
    char name[8];

} filelump_t;

//
// WADFILE I/O related stuff.
//
typedef struct
{
    char	name[8];
    FILE *handle;
    int	position;
    int	size;
} lumpinfo_t;

typedef unsigned char byte;

#define MAXWADFILES		12

lumpinfo_t*	lumpinfo = NULL;
int	numlumps;
void**	lumpcache;

FILE *wadFileHandles[ MAXWADFILES ];
int numWadFiles = 0;


/*
 * \brief Decode compressed lump data
 * \param[in] input Compressed lump data
 * \param[in] output Buffer to hold decompressed data
 * \param[in] length Length of decompressed data in bytes
 */
#define LENSHIFT 4
PRIVATE void decode( unsigned char *input, unsigned char *output, int length )
{
	W8 getidbyte = 0;
    int len;
    int pos;
    int i;
    W8 *source;
	W8 idbyte = 0;

	while ( 1 )
	{
		// get idbyte if necessary
		if ( !getidbyte )
		{
			idbyte = *input++;
		}
        getidbyte = (getidbyte + 1) & 0x7;

        if (idbyte & 0x1)   // decode
        {
            pos = *input++ << LENSHIFT;
            pos = pos | (*input >> LENSHIFT);

            source = output - pos;

            len = (*input++ & 0xF) + 1;
            if ( len == 1 )
			{
                break;
            }

            for ( i = 0 ; i < len ; i++ )
			{
                *output++ = *source++;
                length--;
			}
        }
		else
		{
            *output++ = *input++;
            length--;
        }
        idbyte = idbyte >> 1;
    }
}

/*
 * \brief Parse WAD file
 * \param[in] filename Name of WAD file to parse
 * \return On success true, otherwise false.
 */
PRIVATE wtBoolean W_AddFile ( const char *filename )
{
	wadinfo_t	header;
	lumpinfo_t*	lump_p;
	int		i;
	FILE		*handle;
	int		length;
	int		startlump;
	int		numread;
	filelump_t	*fileinfo;
	filelump_t *	filelumpPointer;

	// open the file and add to directory
	if ( (handle = fopen( filename, "rb" )) == NULL) {
		fprintf( stderr, " couldn't open %s\n", filename );
		return false;
	}

	startlump = numlumps;

	// WAD file
	fseek( handle, ROM_WAD_OFFSET, SEEK_SET );
	numread = fread( &header, sizeof( header ), 1, handle );

	if ( strncmp( header.identification,"IWAD", 4 ) != 0 )
	{
		// Homebrew levels?
		if ( strncmp( header.identification, "PWAD", 4 ) != 0 )
		{
			fprintf( stderr,  "Wad file %s doesn't have IWAD or PWAD id\n", filename );
			fclose( handle );
			return false;
		}
	}

	header.numlumps = LittleLong( header.numlumps );
	header.infotableofs = LittleLong( header.infotableofs );
	length = header.numlumps * sizeof( filelump_t );
	fileinfo = (filelump_t *) MM_MALLOC( length );

	fseek( handle, ROM_WAD_OFFSET + header.infotableofs, SEEK_SET );
	numread = fread( fileinfo, length, 1, handle );

	numlumps += header.numlumps;



	// Fill in lumpinfo
	if ( lumpinfo == NULL ) {
		lumpinfo = (lumpinfo_t*)MM_MALLOC( numlumps * sizeof( lumpinfo_t ) );
	} else {
		lumpinfo = (lumpinfo_t*)MM_REALLOC( lumpinfo, numlumps * sizeof( lumpinfo_t ) );
	}

	if ( !lumpinfo ) {
		fprintf( stderr, "Couldn't realloc lumpinfo" );
		fclose( handle );
		MM_FREE( fileinfo );
		return false;
	}

	lump_p = &lumpinfo[ startlump ];

	wadFileHandles[ numWadFiles++ ] = handle;
	filelumpPointer = &fileinfo[ 0 ];

	for ( i = startlump ; i < numlumps ; i++, lump_p++, filelumpPointer++ )
	{
		lump_p->handle = handle;
		lump_p->position = LittleLong( filelumpPointer->filepos );
		lump_p->size = LittleLong( filelumpPointer->size );

		strncpy ( lump_p->name, filelumpPointer->name, 8 );
	}
	MM_FREE( lumpinfo );
	return true;
}

/*
 * \brief Frees all lump data
 */
PRIVATE void W_FreeLumps()
{
	if( lumpcache ) {
		int i;
		for ( i = 0; i < numlumps; i++ )
		{
			MM_FREE( lumpcache[i] );
		}
	}
	MM_FREE( lumpcache );
	lumpcache = NULL;

	MM_FREE( lumpinfo );
	lumpinfo = NULL;
	numlumps = 0;
}

/*
 * \brief Free this list of wad files so that a new list can be created
 */
PRIVATE void W_FreeWadFiles()
{
	int i;

	for ( i = 0 ; i < MAXWADFILES ; i++ )
	{
		if ( wadFileHandles[i] != NULL )
		{
			fclose( wadFileHandles[i] );
		}
		wadFileHandles[i] = NULL;
	}
	numWadFiles = 0;
}


/*
 * \brief Initialize data for WAD parsing.
 * \param[in] filename WAD file name
 * \return On success true, otherwise false.
 */
PRIVATE wtBoolean W_InitWADFile (const char* filename)
{
	int	size;

	if ( lumpinfo == NULL )
	{
		// open all the files, load headers, and count lumps
		numlumps = 0;

		// will be realloced as lumps are added
		lumpinfo = NULL;


		if( ! W_AddFile ( filename ) )
        {
            return false;
        }


		if (!numlumps)
        {
			fprintf( stderr, "W_InitWADFile: no files found" );
            return false;
        }

		// set up caching
		size = numlumps * sizeof( *lumpcache );
		lumpcache = (void**)MM_MALLOC( size );

		if ( !lumpcache )
        {
			fprintf( stderr, "Couldn't allocate lumpcache" );
            return false;
        }
		memset (lumpcache,0, size);
	}
	else
	{
		// set up caching
		size = numlumps * sizeof( *lumpcache );
		lumpcache = (void**)MM_MALLOC( size );

		if (!lumpcache)
        {
			fprintf( stderr, "Couldn't allocate lumpcache" );
            return false;
        }

		memset (lumpcache,0, size);
	}

    return true;
}

/*
 * \brief Shutdown WAD module.
 * \note Clears lumps and frees files
 */
PRIVATE void W_Shutdown( void )
{
	W_FreeLumps();
	W_FreeWadFiles();
}

/*
 * \brief Gets lump id based on name.
 * \param[in] name Name of lump
 * \return On success lump id; otherwise, -1 if not found
 */
PRIVATE int W_CheckNumForName ( const char* name )
{
    const int NameLength = 9;

    union {
		char	s[9];
		int	    x[2];

    } name8;

    int	v1;
    int	v2;
	int i;
    lumpinfo_t*	lump_p;

    // make the name into two integers for easy compares
    strncpy (name8.s,name, NameLength - 1);

    // in case the name was a fill 8 chars
    name8.s[NameLength - 1] = 0;

    // case insensitive
	for ( i = 0; i < NameLength; ++i )
	{
		name8.s[i] = toupper( name8.s[i] );
	}

    v1 = name8.x[0];
    v2 = name8.x[1];


    // scan backwards so patch lump files take precedence
    lump_p = lumpinfo + numlumps;

    while (lump_p-- != lumpinfo)
    {
		if ( *(int *)lump_p->name == v1
			&& *(int *)&lump_p->name[4] == v2)
		{
			return lump_p - lumpinfo;
		}
    }

    // TFB. Not found.
    return -1;
}

/*
 * \brief Calls W_CheckNumForName, but bombs out if not found.
 * \param[in] name Name of lump
 * \return On success size of lump in bytes; otherwise, -1
 */
PRIVATE int W_GetNumForName ( const char *name )
{
    int	i;

    i = W_CheckNumForName ( name);

    if (i == -1)
    {
        fprintf( stderr, "W_GetNumForName: %s not found!", name );
    }

    return i;
}


/*
 * \brief Returns the buffer size needed to load the given lump.
 * \param[in] lumpId Lump id
 * \return Size of lump in bytes
 */
PRIVATE int W_LumpLength ( int lumpId )
{
    if ( lumpId >= numlumps )
    {
		fprintf( stderr, "W_LumpLength: %i >= numlumps", lumpId );
        return -1;
    }

    return lumpinfo[ lumpId ].size;
}

/**
 * \brief Loads the lump into the given buffer which must be >= W_LumpLength().
 * \param[in] lumpId Id of lump to cache
 * \param[in] dest Destination buffer to hold lump data
 */
PRIVATE void W_ReadLump ( int lump, void *dest )
{
    int	numread;
    lumpinfo_t*	l;
    FILE *handle;

    if (lump >= numlumps)
	{
		fprintf( stderr, "W_ReadLump: %i >= numlumps", lump );
        return;
	}

    l = lumpinfo + lump;

	handle = l->handle;


	if( fseek( handle, ROM_WAD_OFFSET + l->position, SEEK_SET )  != 0 )
	{
		fprintf( stderr, "W_ReadLump: Unable to read from file" );
        return;
	}

	numread = fread( dest, 1, l->size, handle );

    if ( l->name[ 0 ] & 0x80 ) // compressed
    {
        unsigned char *buffer = (unsigned char *)MM_MALLOC( l->size );

        decode( (unsigned char *) dest, buffer, l->size );

        memcpy( dest, buffer, l->size );
        MM_FREE( buffer );

        l->name[ 0 ] &= 0x7F; // Remove compressed flag from name
    }

}

/**
 * \brief Cache lump based on id
 * \param[in] lumpId Id of lump to cache
 * \return On success valid pointer to lump data; otherwise, NULL
 */
PRIVATE void *W_CacheLumpNum( int lumpId )
{

	if ( !lumpcache[ lumpId ] )
	{
		// read the lump in
        int size = W_LumpLength ( lumpId );
        if( size < 0 )
        {
            return NULL;
        }
		lumpcache[ lumpId ] = MM_MALLOC( size );
		W_ReadLump ( lumpId, lumpcache[ lumpId ] );
	}

	return lumpcache[ lumpId ];
}

/**
 * \brief Cache lump based on name
 * \param[in] name Name of lump to cache
 * \return On success valid pointer to lump data; otherwise, NULL
 */
PRIVATE void *W_CacheLumpName( const char *name )
{
    return W_CacheLumpNum ( W_GetNumForName( name ) );
}


PRIVATE void setPalette( const char *paletteName )
{
	palette = (W8 *) W_CacheLumpNum(  W_GetNumForName ( paletteName ) );
}

/**
 * \brief Convert 256 palette data to RGB888.
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
 * \brief Convert 256 palette data to RGB8888.
 * \param[in,out] dst Destination buffer to convert to.
 * \param[in] src 256 palette data.
 * \param[in] length Length of source data in bytes.
 * \param[in] palette Pointer to 256*3 array.
 */
PRIVATE void ConvertPaletteToRGB32( W8 *dst, W8 *src, W32 length, W8 *palette )
{
	W32 srcIndex;
    W32 destIndex = 0;

	for( srcIndex = 0 ; srcIndex < length ; srcIndex++, destIndex += 4 )
	{

        if( src[ srcIndex ] != 0 )
        {
		    dst[ destIndex + 0 ] = palette[ src[ srcIndex ] * 3 + 0 ];
		    dst[ destIndex + 1 ] = palette[ src[ srcIndex ] * 3 + 1 ];
		    dst[ destIndex + 2 ] = palette[ src[ srcIndex ] * 3 + 2 ];
            dst[ destIndex + 3 ] = 255;
        }
	}
}

/**
 * \brief Convert image data from 16 bit CrY to RGB888.
 * \param[in] data Image data in 16 bit CrY format
 * \param[in] out[in,out] out Buffer to hold RGB888 data.
 * \param[in] length Length of data in bytes.
 * \note
 */
PRIVATE void ConvertCRY16ToRGB24( void *data, void *out, SW32 length )
{
	W16 *src16;
	W8 *dest8;
    W32 cyan, red, intensity;
	W32 color;
    SW32 i;

	src16 = (PW16) data;
	dest8 = (PW8)out;

	for( i = 0 ; i < length ; i++ )
	{
		color = BigShort( src16[ i ] );

        cyan = (color & 0xF000) >> 12,
		red = (color & 0x0F00) >> 8,
		intensity = (color & 0x00FF);

		*(dest8++) = (((W32)redLUT[cyan][red]) * intensity) >> 8;    // R
		*(dest8++) = (((W32)greenLUT[cyan][red]) * intensity) >> 8;  // G
		*(dest8++) = (((W32)blueLUT[cyan][red]) * intensity) >> 8;   // B
	}

}


/**
 * \brief Extract range of sprite lumps from WAD file and convert to RGB8888 TGA.
 * \param[in] lumpStart Start of lumpIds
 * \param[in] lumpEnd LumpId in WAD file of screen to decode.
 * \param[in] offset Offset to start of image data
 * \param[in] outputPath Path to save TGA image to.
 * \note
 */
PRIVATE void decodeSprite( int lumpStart, int lumpEnd, int offset, const char *outputPath )
{
	W8 *buffer;
	W8 *ptrResource8;
    W8 *src;
    W8 *dptr;
	char filename[ 256 ];
	SW32 lumpId, x, y, width, height;
    SW32 x_offset, y_offset;

	for( lumpId = lumpStart ; lumpId <= lumpEnd ; lumpId++ )
	{
		ptrResource8 = (PW8)W_CacheLumpNum( lumpId );

		x_offset = ptrResource8[ 0 ];
		y_offset = ptrResource8[ 1 ];

		width = ptrResource8[ 2 ];
		height = ptrResource8[ 3 ];


		buffer = (W8 *)MM_MALLOC( 128 * 128 * 4 );
        memset( buffer, 0, 128 * 128 * 4 );

        src = ptrResource8 + offset;
        dptr = buffer + (y_offset * 128 * 4) + (x_offset * 4);
	    for( y = 0 ; y < height ; y++ )
	    {
		    for( x = 0 ; x < width ; x++, src++ )
		    {
                if( *src != 0 )
                {
		            dptr[ 0 ] = palette[ *src * 3 + 0 ];
		            dptr[ 1 ] = palette[ *src * 3 + 1 ];
		            dptr[ 2 ] = palette[ *src * 3 + 2 ];
                    dptr[ 3 ] = 255;
                }
			    dptr += 4;
		    }

		    dptr += (128 * 4) - width * 4;
	    }

        wt_snprintf( filename, sizeof( filename ), "%s%c%.8s.tga", outputPath, PATH_SEP, lumpinfo[ lumpId ].name );
		TGA_write( filename, 32, 128, 128, buffer, 0, 1 );

		MM_FREE( buffer );
	}

}

/**
 * \brief Extract screen lump from WAD file and convert to TGA.
 * \param[in] lumpId Lump Id of screen to decode.
 * \note
 */
PRIVATE void decodeScreenLump( int lumpId )
{
    W16 *ptrResource16;
	W8 *imageBuffer;
	int width, height;
    char filename[ 256 ];

    ptrResource16 = (PW16)W_CacheLumpNum( lumpId );

	width = BigShort( ptrResource16[ 0 ] );
	height = BigShort( ptrResource16[ 1 ] );

	imageBuffer = (PW8) MM_MALLOC( width * height * 4 );
    memset( imageBuffer, 0, width * height * 4 );

	ConvertPaletteToRGB32( imageBuffer, (PW8)(ptrResource16+8), width * height, palette );

    wt_snprintf( filename, sizeof( filename ), "%s%c%.8s.tga", PATH_SCREENS, PATH_SEP, lumpinfo[ lumpId ].name );

    TGA_write( filename, 32, width, height, imageBuffer, 0, 1 );

	MM_FREE( imageBuffer );
}

/**
 * \brief Extract Brief and convert to TGA.
 */
PRIVATE void decodeBriefScreen( )
{
	W16 *ptrResource16;
	W8 *imageBuffer;
	W8 *briefScreenBuffer;
    W8 *src8;
    W16 *src16;
	int width, height;
    char filename[ 256 ];
    int i;

	setPalette( "BRIEFPAL" );

	ptrResource16 = (PW16)W_CacheLumpNum( 359 );

	width = BigShort( ptrResource16[ 0 ] );
	height = BigShort( ptrResource16[ 1 ] );

	imageBuffer = (PW8) MM_MALLOC( width * height * 2 );
    briefScreenBuffer = (PW8) MM_MALLOC( width * height * 3 );

    src8 = (PW8)(ptrResource16+8);
    src16 = (PW16) imageBuffer;

	for( i = 0 ; i < width * height ; i++ )
	{
		*(src16++) = BigShort( palette[ src8[ i ] * 2 ] << 8 | palette[ src8[ i ] * 2 + 1 ] );
	}

    wt_snprintf( filename, sizeof( filename ), "%s%c%.8s.tga", PATH_SCREENS, PATH_SEP, lumpinfo[ 359 ].name );
    ConvertCRY16ToRGB24( imageBuffer, briefScreenBuffer, width * height );
	TGA_write( filename, 24, width, height, briefScreenBuffer, 0, 1 );

	MM_FREE( imageBuffer );
    MM_FREE( briefScreenBuffer );
}

/**
 * \brief Extract WolfTitle and convert to TGA.
 */
PRIVATE void decodeWolfTitle()
{
	W16 *ptrResource16;
	W32 *imageBuffer32;
	int width, height;
    char filename[ 256 ];


	ptrResource16 = (PW16)W_CacheLumpNum( 362 );

	width = BigShort( ptrResource16[ 0 ] );
	height = BigShort( ptrResource16[ 1 ] );

	imageBuffer32 = (PW32) MM_MALLOC( width * height * 3 );

	ConvertCRY16ToRGB24( ptrResource16+12, imageBuffer32, width * height );
    wt_snprintf( filename, sizeof( filename ), "%s%c%.8s.tga", PATH_SCREENS, PATH_SEP, lumpinfo[ 362 ].name );
    TGA_write( filename, 24, width, height, imageBuffer32, 0, 1 );

	MM_FREE( imageBuffer32 );
}

/**
 * \brief Extract HUD data from WAD file and convert to TGA.
 */
PRIVATE void decodeScreens( )
{
	W32 *ptrResource32;

	// Decode mission brief screen
	decodeBriefScreen();

	setPalette( "RGBPALS" );

    // Decode LOGO
    decodeScreenLump( 356 );

	// Decode Meet the Cast label
    decodeScreenLump( 357 );

	// Decode Credits Screen
    decodeScreenLump( 358 );

	// Decode Wolf Title
	decodeWolfTitle();

	// Decode BallMap
	ptrResource32 = (PW32)W_CacheLumpNum( 361 );


}

/**
 * \brief Extract map icon data from WAD file and convert to TGA.
 */
PRIVATE void decodeHUD_MapIcon()
{
    FILE *fhandle;
    int width, height, x, y;
	char filename[ 256 ];
    W16 *ptr16;
    W8 *buffer;
    W8 *ptrResource = (W8 *)W_CacheLumpNum( 298 );
    ptr16 = (PW16)ptrResource;

    width = 8;
    height = 100;

    //buffer = (W8 *)MM_MALLOC(width * height * 3 );
    //ConvertPaletteToRGB( buffer, ptrResource+8, lumpinfo[ 298 ].size - 8, palette );

	//wt_snprintf( filename, sizeof( filename ), "%s/%.8s.dump", PATH_HUD, lumpinfo[ 298 ].name );
    //TGA_write( "MAPICONS.tga", 24, width, height, buffer, 0, 1 );

    //MM_FREE( buffer );

	//fhandle = fopen( filename, "wb" );
	//fwrite( ptrResource, 1, lumpinfo[ 298 ].size, fhandle );
    //fclose( fhandle );
}

/**
 * \brief Extract HUD data from WAD file and convert to TGA.
 */
PRIVATE void decodeHUD()
{

    // decode weapons
    decodeSprite( 274, 297, 8, PATH_HUD );

    decodeHUD_MapIcon();

    // decode numbers, ammo, faces
    decodeSprite( 299, 353, 8, PATH_HUD );


}

/**
 * \brief Extract label data from WAD file and convert to TGA.
 */
PRIVATE void decodeLabels( )
{
    int lumpId;
	W8 *buffer;
	W16 *ptr16;
	char filename[ 256 ];
	int width, height;

	for( lumpId = 224 ; lumpId <= 272 ; lumpId++ )
	{
		W8 *ptrResource = (W8 *)W_CacheLumpNum( lumpId );

		ptr16 = (PW16)ptrResource;
		width = BigShort( ptr16[ 0 ] );
		height = BigShort( ptr16[ 1 ] );

		buffer = (W8 *)MM_MALLOC( width * height * 4 );
		memset( buffer, 0,  width * height * 4 );
		ConvertPaletteToRGB32( buffer, (PW8)(ptr16+8), width * height, palette );

        wt_snprintf( filename, sizeof( filename ), "%s%c%.8s.tga", PATH_LABELS, PATH_SEP, lumpinfo[ lumpId ].name );
		TGA_write( filename, 32, width, height, buffer, 0, 1 );

		MM_FREE( buffer );
	}
}

/**
 * \brief Extract wall data from WAD file and convert to TGA.
 */
PRIVATE void decodeWalls()
{
	int lumpIndex;
	W8 *buffer;
	W8 *newwall;
	char filename[ 256 ];


	for( lumpIndex = 186 ; lumpIndex <= 221 ; lumpIndex++ )
	{
		W8 *ptrResource = (W8 *)W_CacheLumpNum( lumpIndex );

		newwall = obverseWall( ptrResource, 128, 128 );

		buffer = (W8 *)MM_MALLOC( 128 * 128 * 3 );
		ConvertPaletteToRGB( buffer, newwall, 128 * 128, palette );

        wt_snprintf( filename, sizeof( filename ), "%s%c%.8s.tga", PATH_WALLS, PATH_SEP, lumpinfo[ lumpIndex ].name );
        TGA_write( filename, 24, 128, 128, buffer, 0, 1 );

		MM_FREE( buffer );
		MM_FREE( newwall );
	}
}

/**
 * \brief Extract sprite data from WAD file and convert to TGA.
 */
PRIVATE void decodeSprites()
{
    decodeSprite( 31, 184, 8, PATH_SPRITES );
}

/**
 * \brief Extract digital sound from WAD file and convert to WAV.
 */
PRIVATE void decodeSounds()
{
    int lumpIndex;
	W8 *ptrResource;
	char filename[ 256 ];

    for( lumpIndex = 367 ; lumpIndex <= 390 ; lumpIndex++ )
    {
        ptrResource = (W8 *)W_CacheLumpNum( lumpIndex );

        wt_snprintf( filename, sizeof( filename ), "%s%c%.8s.wav", PATH_SOUNDS, PATH_SEP, lumpinfo[ lumpIndex ].name );
        wav_write( filename, ptrResource, lumpinfo[ lumpIndex ].size, 1, 22050, 1 );
    }
}

/**
 * \brief Extract music from WAD file and store as midi files.
 */
PRIVATE void decodeMusic()
{
    int lumpIndex;
	W8 *ptrResource;
	char filename[ 256 ];
    FILE *handle;

	for( lumpIndex = 392 ; lumpIndex <= 407 ; lumpIndex++ )
    {
		if( lumpIndex == 394 )	// Skip Music label
		{
			continue;
		}

        ptrResource = (W8 *)W_CacheLumpNum( lumpIndex );

        wt_snprintf( filename, sizeof( filename ), "%s%c%.8s.mid", PATH_MUSIC, PATH_SEP, lumpinfo[ lumpIndex ].name );

        handle = fopen( filename, "wb" );
        fwrite( ptrResource, 1, lumpinfo[ lumpIndex ].size, handle );
        fclose( handle );
    }
}

/**
 * \brief Extract maps from WAD file and dump to file system.
 */
PRIVATE void decodeMaps()
{
    int lumpIndex;
	W8 *ptrResource;
	char filename[ 256 ];
    FILE *handle;

    for( lumpIndex = 0 ; lumpIndex <= 29 ; lumpIndex++ )
    {
        ptrResource = (W8 *)W_CacheLumpNum( lumpIndex );

        wt_snprintf( filename, sizeof( filename ), "%s%c%.8s.map", PATH_MAPS, PATH_SEP, lumpinfo[ lumpIndex ].name );

        handle = fopen( filename, "wb" );
        fwrite( ptrResource, 1, lumpinfo[ lumpIndex ].size, handle );
        fclose( handle );
    }
}


/**
 * \brief Interface to Atari Jaguar data extractor.
 */
PUBLIC void wolf_jaguar_decoder( void )
{
	char *fname;
	char ext[ 256 ];
	int i;

    i = 0;
    while( jag_File_EXT[ i ] )
    {
	    wt_strlcpy( ext, jag_File_EXT[ i ], sizeof( ext ) );
	    fname = FS_FindFirst( ext );
	    FS_FindClose();

	    if( fname == NULL )
	    {
		    // try again with lower case
		    fname = FS_FindFirst( wt_strlwr( ext ) );
		    FS_FindClose();
	    }

        if( fname != NULL )
	    {
		    break;
	    }

        i++;
    }

    if( fname == NULL )
    {
	    fprintf( stderr, "Could not find any ROM files for read\n" );

	    return;
    }

	if( ! W_InitWADFile( fname ) )
    {
        W_Shutdown();
        return;
    }


    FS_CreateDirectory( PATH_MAPS );
	FS_CreateDirectory( PATH_SPRITES );
	FS_CreateDirectory( PATH_WALLS );
	FS_CreateDirectory( PATH_HUD );
    FS_CreateDirectory( PATH_LABELS );
    FS_CreateDirectory( PATH_SCREENS );
    FS_CreateDirectory( PATH_SOUNDS );
    FS_CreateDirectory( PATH_MUSIC );

	setPalette( "RGBPALS" );

	printf( "Atari Jaguar Wolfenstein ROM Decoding\n\n" );

    printf( "Decoding map data... " );
    decodeMaps();
    printf( "Done\n" );

    printf( "Decoding sprite data... " );
    decodeSprites();
    printf( "Done\n" );

	printf( "Decoding wall data... " );
	decodeWalls();
	printf( "Done\n" );

	printf( "Decoding HUD data... " );
	decodeHUD();
	printf( "Done\n" );

	printf( "Decoding label data... " );
    decodeLabels();
    printf( "Done\n" );

    printf( "Decoding screen data... " );
	decodeScreens();
    printf( "Done\n" );

    printf( "Decoding sound data... " );
    decodeSounds();
    printf( "Done\n" );

    printf( "Decoding music data... " );
    decodeMusic();
    printf( "Done\n" );

    W_Shutdown();
}
