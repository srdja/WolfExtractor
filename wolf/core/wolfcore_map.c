/*

	Copyright (C) 2007 Michael Liebscher <johnnycanuck@users.sourceforge.net>

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
 * \file wolfcore_map.c
 * \brief Wolfenstein 3-D map file decoder.
 * \author Michael Liebscher 
 * \date 2007 
 */


#include <stdio.h>
#include <string.h>

#include "wolfcore.h"
#include "../../common/platform.h"
#include "../../common/common_utils.h"
#include "../../memory/memory.h"
#include "../../string/wtstring.h"
#include "../../filesys/file.h"
#include "../wolfcore_decoder.h"


PRIVATE FILE *map_file_handle = NULL;

PRIVATE W32	headerOffsets[ 256 ];


/**
 * \brief Setup map files for decoding.
 * \param[in] headFileName Name of file with header offsets.
 * \param[in] mapFileName Name of file with map data.
 * \param[in,out] RLEWtag Run length encoded word tag.
 * \param[in,out] nTotalMaps Number of maps in file.
 * \return On success true, otherwise false.
 * \note Must call function MapFile_Shutdown() when done.
 */
PUBLIC wtBoolean MapFile_Setup( const char *headFileName, const char *mapFileName, W16 *RLEWtag, W32 *nTotalMaps )
{
	FILE *fileHandle;
	SW32 fileSize;
	char *tempFileName;
	W32 TotalMaps;



	tempFileName = (char *) MM_MALLOC( strlen( headFileName ) + 1 );
	if( tempFileName == NULL )
	{
		return false;
	}
	
	
//
// Load map head file to get offsets and tile info.
//
	wt_strlcpy( tempFileName, headFileName, strlen( headFileName ) + 1 );

	fileHandle = fopen( wt_strupr( tempFileName ), "rb" );
	if( fileHandle == NULL )
	{
		fileHandle = fopen( wt_strlwr( tempFileName ), "rb" );
		if( fileHandle == NULL )
		{
			fprintf( stderr, "Could not open file (%s) for read!\n", tempFileName );

			MM_FREE( tempFileName );
			
			return false;
		}
	}

	fileSize = FS_FileLength( fileHandle );
	

	fread( RLEWtag, 2, 1, fileHandle );
	
	for( TotalMaps = 0 ; TotalMaps < fileSize ; ++TotalMaps )
	{
		fread( &headerOffsets[ TotalMaps ], 4, 1, fileHandle );		
		if( ! headerOffsets[ TotalMaps ] )
		{
			break;
		}
	}


	fclose( fileHandle );




	MM_FREE( tempFileName );
	tempFileName = (char *) MM_MALLOC( strlen( mapFileName ) + 1 );

	wt_strlcpy( tempFileName, mapFileName, strlen( mapFileName ) + 1 );


//
// Open map data file.
//
	map_file_handle = fopen( wt_strupr( tempFileName ), "rb");
	if( NULL == map_file_handle )
	{
		map_file_handle = fopen( wt_strlwr( tempFileName ), "rb");
		if( NULL == map_file_handle )
		{
			MM_FREE( tempFileName );

			return false;
		}
	}


	*nTotalMaps = TotalMaps;


	MM_FREE( tempFileName );


	return true;
}


/**
 * \brief Shutdown map file processing.
 */
PUBLIC void MapFile_Shutdown( void )
{
	if( map_file_handle )
	{
		fclose( map_file_handle );
		map_file_handle = NULL;
	}
}


/**
 * \brief Get map data chunk.
 * \param[in] chunkOffset Source buffer to convert from
 * \param[in] chunkLength Size of chunk data.
 * \return NULL on error, otherwise pointer to map data.
 * \note Caller must free allocated data.
 */
PUBLIC void *MapFile_getMapData( W32 chunkOffset, W32 chunkLength )
{
	void *mapdata;

	if( map_file_handle == NULL )
	{
		return NULL;
	}

	mapdata = MM_MALLOC( chunkLength );
	if( NULL == mapdata )
	{
		MM_OUTOFMEM( "MapFile_getMapData"  );

		return NULL;
	}

	fseek( map_file_handle, chunkOffset, SEEK_SET );

	fread( mapdata, 1, chunkLength, map_file_handle );


	return mapdata;
}

/**
 * \brief Convert map to Redux file format.
 * \param[in] fmaphead Map header file name.
 * \param[in] fmap Map file name.
 * \param[in] path Path to save redux map to.
 * \param[in] palette Pointer to palette array.
 * \param[in] ceilingColour Array of ceiling colours.
 * \param[in] musicFileName Array of music titles.
 * \param[in] parTimes Struct with the parTimes.
 * \return On success true, otherwise false.
 * \note Caller must free allocated data.
 */
PUBLIC wtBoolean MapFile_ReduxDecodeMapData( const char *fmaphead, const char *fmap, const char *path,
                                             W8 *palette, const W32 *ceilingColour, char *musicFileName[], parTimes_t *parTimes, char *format )
{
	W16 Rtag;
	W32 totalMaps;
	
	W32 i;
	FILE *fout;
	char filename[ 256 ];
	W32 offset[ 3 ];
	W32 offsetin[ 3 ];
	W16 length[ 3 ];
	W8 sig[ 5 ];
	W16 w, h;
	char name[ 32 ];
	char musicName[ 64 ];
	SW32 jmp;
	W32 ceiling;
	W32 floor;
	W32 palOffset;
    W32 temp;
	float ftime;
	char *stime;
	W8 *data;
	
	
	printf( "Decoding Map Data..." );

	
	if( ! MapFile_Setup( fmaphead, fmap, &Rtag, &totalMaps ) )
	{
		return false;
	}
	
	
	for( i = 0 ; i < totalMaps ; ++i )
	{
        if( fseek( map_file_handle, headerOffsets[ i ], SEEK_SET ) != 0 )
        {
            break;
        }


		wt_snprintf( filename, sizeof( filename ), format, path, i );

		fout = fopen( filename, "wb");
        if( NULL == fout )
        {
            continue;
        }
                
        
        // Get ceiling colour
        palOffset = (ceilingColour[ i ] & 0xff) * 3;
		ceiling = (palette[ palOffset ] << 16) | (palette[ palOffset + 1 ] << 8) | palette[ palOffset + 2 ];
		
		
	
        // Get floor colour
		palOffset = 0x19 * 3;
		floor = (palette[ palOffset ] << 16 ) | (palette[ palOffset + 1 ] << 8) | palette[ palOffset + 2 ];
        
        
        wt_snprintf( musicName, sizeof( musicName ), "%s/%s.ogg", DIR_MUSIC, musicFileName[ i ] );
        
        
        ftime = parTimes[ i ].time;
		stime = parTimes[ i ].timestr;
        
        
        //
        // Read in map data
        //               
        
        fread( &offsetin, sizeof( W32 ), 3, map_file_handle );
        offsetin[ 0 ] = LittleLong( offsetin[ 0 ] );
        offsetin[ 1 ] = LittleLong( offsetin[ 1 ] );
        offsetin[ 2 ] = LittleLong( offsetin[ 2 ] );
        fread( &length, sizeof( W16 ), 3, map_file_handle );
        length[ 0 ] = LittleShort( length[ 0 ] );
        length[ 1 ] = LittleShort( length[ 1 ] );
        length[ 2 ] = LittleShort( length[ 2 ] );
        
        fread( &w, sizeof( W16 ), 1, map_file_handle );
        w = LittleShort( w );
        fread( &h, sizeof( W16 ), 1, map_file_handle );
        h = LittleShort( h );
    
        fread( name, sizeof( W8 ), 16, map_file_handle );        	
        fread( sig, sizeof( W8 ), 4, map_file_handle );
        
        
        //	
        // Output header
        //
        // Map file header signature
        fwrite( sig, sizeof( W8 ), 4, fout );	

        // RLE Word tag
        Rtag = LittleShort( Rtag );
        fwrite( &Rtag, sizeof( W16 ), 1, fout );	
    
        // Max Width
        w = LittleShort( w );
        fwrite( &w, sizeof( W16 ), 1, fout );
        
        // Max Height
        h = LittleShort( h );
        fwrite( &h, sizeof( W16 ), 1, fout );	
    
        // Ceiling Colour
        ceiling = LittleLong( ceiling );
        fwrite( &ceiling, sizeof( W32 ), 1, fout );		
    
        // Floor Colour
        floor = LittleLong( floor );
        fwrite( &floor, sizeof( W32 ), 1, fout );
        
        // Length of layers
        temp = LittleShort( length[ 0 ] );
        fwrite( &temp, sizeof( W16 ), 1, fout );	// Length One
        temp = LittleShort( length[ 1 ] );
        fwrite( &temp, sizeof( W16 ), 1, fout );	// Length Two	
        temp = LittleShort( length[ 2 ] );
        fwrite( &temp, sizeof( W16 ), 1, fout );	// Length Three
    
        jmp = ftell( fout );
    
        temp = 0;
        fwrite( &temp, sizeof( W32 ), 1, fout );	// Offset One
        fwrite( &temp, sizeof( W32 ), 1, fout );	// Offset Two	
        fwrite( &temp, sizeof( W32 ), 1, fout );	// Offset Three
        
        
        // Map name length	
        temp = strlen( name );
        fwrite( &temp, sizeof( W16 ), 1, fout );
    
        // Music name length	
        temp = strlen( musicName );
        fwrite( &temp, sizeof( W16 ), 1, fout );		
    
        // Par time Float
        ftime = LittleFloat( ftime );
        fwrite( &ftime, sizeof( float ), 1, fout );	
    
        // Par time string
        fwrite( stime, sizeof( W8 ), 5 , fout );	
    
        // Map name
        fwrite( name, sizeof( W8 ), strlen( name ), fout );	
    
        // Music file name
        fwrite( musicName, sizeof( W8 ), strlen( musicName ), fout );	
        
    
        
    
    
        data = (PW8) MM_MALLOC( length[ 0 ] );
        if( data == NULL )
        {
            continue;
        }
    
        offset[ 0 ] = ftell( fout );
    
        fseek( map_file_handle, offsetin[ 0 ], SEEK_SET );
        
        fread( data, 1, length[ 0 ], map_file_handle );    
        fwrite( data, 1, length[ 0 ], fout );
        
        MM_FREE( data );
    
    
        data = (PW8) MM_MALLOC( length[ 1 ] );
        if( data == NULL )
        {
			MapFile_Shutdown();

            return 0;
        }
    
        offset[ 1 ] = ftell( fout );
    
        fseek( map_file_handle, offsetin[ 1 ], SEEK_SET );
        
        fread( data, 1, length[ 1 ], map_file_handle );    
        fwrite( data, 1, length[ 1 ], fout );
    
        MM_FREE( data );
    
    
        data = (PW8) MM_MALLOC( length[ 2 ] );
        if( data == NULL )
        {
			MapFile_Shutdown();

            return 0;
        }
    
        offset[ 2 ] = ftell( fout );
    
        fseek( map_file_handle, offsetin[ 2 ], SEEK_SET );
        fread( data, 1, length[ 2 ], map_file_handle );
    
        fwrite( data, 1, length[ 2 ], fout );	
    
        MM_FREE( data );
    
    
        fseek( fout, jmp, SEEK_SET );
    
        temp = LittleLong( offset[ 0 ] );
        fwrite( &temp, sizeof( W32 ), 1, fout );	// Offset One
        
        temp = LittleLong( offset[ 1 ] );
        fwrite( &temp, sizeof( W32 ), 1, fout );	// Offset Two	
        
        temp = LittleLong( offset[ 2 ] );
        fwrite( &temp, sizeof( W32 ), 1, fout );	// Offset Three
    
        
        fclose( fout );	

	}


	MapFile_Shutdown();


    printf( "Done\n" );

	
	return true;
}

